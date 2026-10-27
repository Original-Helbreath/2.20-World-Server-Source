#include "client_gate.h"

#include "log.h"
#include "message_codec.h"
#include "message_id.h"
#include "pending_limits.h"
#include "server.h"
#include "world_messages.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <deque>
#include <span>
#include <thread>
#include <utility>

namespace hb::server::world
{
	namespace
	{
		using hb::net::close_reason;
		using hb::net::message_id;
		using namespace std::chrono_literals;

		constexpr std::chrono::milliseconds enter_deadline = 10000ms;
		constexpr std::chrono::milliseconds drain_limit = 1000ms;
		constexpr std::size_t peer_address_chars = 15;

		// The XSocket results the game checks (XSocket.h); repeated here so this file needs no Windows header.
		constexpr int xsock_socket_closed = -127;
		constexpr int xsock_message_too_large = -132;
	}

	// One accepted connection. Touched only on the thread that calls client_gate::poll().
	class client_session
	{
	public:
		enum class state
		{
			awaiting_enter,  // nothing received yet
			entering,        // world_enter_request queued for the handler
			admitted,        // world_enter_response ok sent; game messages flow
		};

		explicit client_session(std::shared_ptr<hb::net::connection> link) : connection(std::move(link)) {}

		std::shared_ptr<hb::net::connection> connection;
		state current = state::awaiting_enter;
		int client_index = -1;       // the game's slot while a socket is bound; -1 once the game let go
		bool socket_bound = false;   // a world_client_socket exists for this session
		bool pending = false;        // counted in the gate's pending_limits until it enters or closes
		std::string address;         // peer address, as counted
	};

	namespace
	{
		// The game's view of one session (world_client_socket.h).
		class session_socket final : public world_client_socket
		{
		public:
			explicit session_socket(std::shared_ptr<client_session> session) : m_session(std::move(session))
			{
				m_session->socket_bound = true;
			}

			// The game deleted its client: whatever was queued goes out, then the connection closes. Close events that
			// follow no longer name the slot, which a new player may take.
			~session_socket() override
			{
				m_session->socket_bound = false;
				m_session->client_index = -1;
				if (m_session->current == client_session::state::admitted)
					m_session->connection->close_after_send(close_reason::local_close, "player removed");
			}

			int iSendMsg(char * cData, unsigned long dwSize, char) override
			{
				if (!m_session->connection->is_open())
					return xsock_socket_closed;
				if (m_session->current != client_session::state::admitted)
					return static_cast<int>(dwSize);  // loading: nothing reaches the client before world_enter_response
				std::vector<std::byte> bytes;
				const std::span<const std::byte> body(reinterpret_cast<const std::byte*>(cData), dwSize);
				if (!hb::net::encode_frame(hb::net::to_wire(message_id::client_game_message), body,
						hb::net::connection_options{}.max_body_bytes, bytes))
					return xsock_message_too_large;
				m_session->connection->send_encoded(std::move(bytes));
				return static_cast<int>(dwSize);
			}

			int iGetPeerAddress(char * pAddrString) override
			{
				const std::string& address = m_session->connection->remote_address();
				const std::size_t length = (std::min)(address.size(), peer_address_chars);
				std::memcpy(pAddrString, address.data(), length);
				pAddrString[length] = '\0';
				return 0;
			}

		private:
			std::shared_ptr<client_session> m_session;
		};

		struct gate_event
		{
			enum class kind
			{
				enter,
				message,
				closed,
			};
			kind what = kind::message;
			std::shared_ptr<client_session> session;
			hb::net::world::world_enter_request request{};
			std::vector<std::byte> body;
		};
	}

	struct client_gate_state
	{
		asio::io_context context;
		std::shared_ptr<hb::net::server> server;
		std::deque<gate_event> queue;
		gate_events events;
		pending_limits pending;
	};

	namespace
	{
		client_gate_state* g_state = nullptr;

		void release_pending(client_session& session)
		{
			if (!session.pending)
				return;
			session.pending = false;
			g_state->pending.remove(session.address);
		}

		void on_frame(const std::shared_ptr<client_session>& session, const hb::net::frame& received)
		{
			client_gate_state& state = *g_state;
			switch (session->current)
			{
			case client_session::state::awaiting_enter:
			{
				gate_event event;
				event.what = gate_event::kind::enter;
				if (!hb::net::decode(received, event.request))
				{
					session->connection->close(close_reason::protocol_error, "the first frame must be a world_enter_request");
					return;
				}
				session->current = client_session::state::entering;
				event.session = session;
				state.queue.push_back(std::move(event));
				return;
			}
			case client_session::state::entering:
			case client_session::state::admitted:
				if (received.header.message_id != hb::net::to_wire(message_id::world_game_message))
				{
					session->connection->close(close_reason::protocol_error, "only world_game_message after the ticket");
					return;
				}
				if (!is_acceptable_game_message(received.body.size()))
				{
					session->connection->close(close_reason::protocol_error, "game message shorter than its id and type, or too long");
					return;
				}
				{
					gate_event event;
					event.what = gate_event::kind::message;
					event.session = session;
					event.body = received.body;
					state.queue.push_back(std::move(event));
				}
				return;
			}
		}
	}

	client_gate::client_gate(gate_events events)
	{
		auto state = new client_gate_state();
		state->events = std::move(events);
		g_state = state;
	}

	client_gate::~client_gate()
	{
		stop();
		delete g_state;
		g_state = nullptr;
	}

	bool client_gate::listen(const std::string& address, std::uint16_t port, std::size_t max_connections, std::string& error)
	{
		client_gate_state& state = *g_state;
		hb::net::server_options options;
		options.bind_address = address;
		options.port = port;
		// Players plus the separately capped connections still waiting to enter.
		options.max_connections = max_connections + max_pending_connections;
		state.server = hb::net::server::create(state.context);
		return state.server->listen(options,
			[](const std::shared_ptr<hb::net::connection>& connection) {
				auto session = std::make_shared<client_session>(connection);
				session->address = connection->remote_address();
				if (!g_state->pending.try_add(session->address))
				{
					hb::log::warning("client {} refused: too many connections waiting to enter", session->address);
					connection->close(close_reason::protocol_error, "too many pending connections");
					return;
				}
				session->pending = true;
				connection->set_deadline(enter_deadline);
				connection->start(
					[session](hb::net::connection&, const hb::net::frame& received) { on_frame(session, received); },
					[session](hb::net::connection&, close_reason reason, std::string_view detail) {
						hb::log::debug("client {} closed: {}{}{}", session->connection->remote_address(), hb::net::to_string(reason),
							detail.empty() ? "" : ", ", detail);
						release_pending(*session);
						gate_event event;
						event.what = gate_event::kind::closed;
						event.session = session;
						g_state->queue.push_back(std::move(event));
					});
			},
			error);
	}

	void client_gate::poll()
	{
		client_gate_state& state = *g_state;
		state.context.restart();
		state.context.poll();
		while (!state.queue.empty())
		{
			gate_event event = std::move(state.queue.front());
			state.queue.pop_front();
			client_session& session = *event.session;
			switch (event.what)
			{
			case gate_event::kind::enter:
				if (session.connection->is_open() && state.events.on_enter)
					state.events.on_enter(event.session, event.request);
				break;
			case gate_event::kind::message:
				if (session.client_index > 0 && state.events.on_message)
					state.events.on_message(session.client_index, event.body);
				break;
			case gate_event::kind::closed:
				if (session.client_index > 0 && state.events.on_closed)
					state.events.on_closed(session.client_index);
				break;
			}
		}
	}

	void client_gate::stop()
	{
		if (g_state == nullptr || !g_state->server)
			return;
		g_state->server->stop();
		const auto limit = std::chrono::steady_clock::now() + drain_limit;
		while (g_state->server->connection_count() > 0 && std::chrono::steady_clock::now() < limit)
		{
			g_state->context.restart();
			g_state->context.poll();
			std::this_thread::sleep_for(5ms);
		}
		g_state->queue.clear();
		g_state->server.reset();
		g_state->pending = pending_limits{};
	}

	void client_gate::refuse(client_session& session, hb::net::world::world_enter_result result)
	{
		(void)session.connection->send(hb::net::world::world_enter_response{ result });
		session.connection->close_after_send(close_reason::protocol_error, "world_enter refused");
	}

	std::unique_ptr<world_client_socket> client_gate::make_socket(const std::shared_ptr<client_session>& session)
	{
		return std::make_unique<session_socket>(session);
	}

	void client_gate::admit(client_session& session, int client_index)
	{
		(void)session.connection->send(hb::net::world::world_enter_response{ hb::net::world::world_enter_result::ok });
		session.connection->set_deadline(std::chrono::milliseconds::zero());
		session.client_index = client_index;
		session.current = client_session::state::admitted;
		release_pending(session);
	}
}
