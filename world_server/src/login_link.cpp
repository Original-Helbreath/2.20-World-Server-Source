#include "login_link.h"

#include "character_service.h"
#include "log.h"
// By path: a plain "client.h" would find the game's own Client.h in this directory first.
#include "../../shared/net/client.h"
#include "message_codec.h"
#include "message_id.h"
#include "periodic_timer.h"

#include <chrono>
#include <cstddef>
#include <span>
#include <thread>
#include <vector>
#include <utility>

namespace hb::server::world
{
	namespace
	{
		namespace link = hb::net::link;
		using hb::net::close_reason;
		using hb::net::message_id;
		using namespace std::chrono_literals;

		constexpr std::chrono::milliseconds retry_delay = 2000ms;
		constexpr std::chrono::milliseconds status_interval = 10000ms;
		constexpr std::chrono::milliseconds drain_limit = 1000ms;
		constexpr std::uint32_t heartbeats_before_loss = 3;

		static_assert(world_name_max_length == sizeof(hb::net::world_text));
		static_assert(link_key_bytes == sizeof(hb::net::link_key));

		std::string_view to_text(link::register_result result)
		{
			switch (result)
			{
			case link::register_result::ok:
				return "ok";
			case link::register_result::bad_key:
				return "wrong link key or address not allowed";
			case link::register_result::world_name_taken:
				return "world name already registered on another link";
			case link::register_result::unknown_world:
				return "world name is not listed in the Login server's config";
			case link::register_result::version_mismatch:
				return "version mismatch";
			}
			return "unknown result";
		}

		// A wrong key or name does not fix itself, so there is no point in retrying; a name still held by a
		// link the Login has not yet noticed dead does.
		bool is_permanent(link::register_result result)
		{
			return result == link::register_result::bad_key || result == link::register_result::unknown_world
				|| result == link::register_result::version_mismatch;
		}
	}

	// Everything below runs on the thread that calls poll() and stop().
	struct login_link::impl
	{
		impl(const world_config& configuration, link_registration registering, link_events handlers)
			: config(configuration)
			, registration(std::move(registering))
			, events(std::move(handlers))
		{
		}

		std::chrono::milliseconds heartbeat_interval() const
		{
			return std::chrono::seconds(config.heartbeat_seconds);
		}

		void begin();
		void on_connect(const std::shared_ptr<hb::net::connection>& connection);
		void on_failure(std::string_view error);
		void on_frame(hb::net::connection& from, const hb::net::frame& received);
		void on_register_response(hb::net::connection& from, const hb::net::frame& received);
		void on_character_list(hb::net::connection& from, const hb::net::frame& received);
		void on_create_character(hb::net::connection& from, const hb::net::frame& received);
		void on_delete_character(hb::net::connection& from, const hb::net::frame& received);
		void on_session_grant(hb::net::connection& from, const hb::net::frame& received);
		void on_kick_account(hb::net::connection& from, const hb::net::frame& received);
		void on_close(hb::net::connection& from, close_reason reason, std::string_view detail);
		void start_timers(hb::net::connection& from);
		void stop_timers();
		void send_heartbeat();
		void send_status(bool force);

		asio::io_context context;
		world_config config;
		link_registration registration;
		link_events events;
		std::shared_ptr<hb::net::client> client;
		std::shared_ptr<hb::net::connection> current;
		std::shared_ptr<hb::net::periodic_timer> heartbeat;
		std::shared_ptr<hb::net::periodic_timer> status_timer;
		bool registered = false;
		bool refused = false;
		bool stopping = false;
		std::uint32_t failures = 0;
		std::uint32_t sequence = 0;
		link_report last_report;
	};

	void login_link::impl::begin()
	{
		hb::net::client_options options;
		options.host = config.login_address;
		options.port = config.login_port;
		options.reconnects = true;
		options.retry_delay = retry_delay;
		client = hb::net::client::create(context, options);
		hb::log::info("connecting to the Login server at {}:{}", config.login_address, config.login_port);
		client->start([this](const std::shared_ptr<hb::net::connection>& connection) { on_connect(connection); },
			[this](std::string_view error) { on_failure(error); });
	}

	void login_link::impl::on_failure(std::string_view error)
	{
		if (stopping)
			return;
		// One warning per outage; the retries every 2 s stay at debug.
		if (failures++ == 0)
			hb::log::warning("{} (retrying every {} s)", error, retry_delay.count() / 1000);
		else
			hb::log::debug("{}", error);
	}

	void login_link::impl::on_connect(const std::shared_ptr<hb::net::connection>& connection)
	{
		current = connection;
		failures = 0;
		sequence = 0;
		registered = false;
		hb::log::info("connected to the Login server at {}:{}", config.login_address, config.login_port);

		// The register answer, and then any frame, must come within three heartbeats of silence.
		const std::chrono::milliseconds loss_timeout = heartbeat_interval() * heartbeats_before_loss;
		connection->set_idle_timeout(loss_timeout);
		connection->set_deadline(loss_timeout);
		connection->start(
			[this](hb::net::connection& from, const hb::net::frame& received) { on_frame(from, received); },
			[this](hb::net::connection& from, close_reason reason, std::string_view detail) { on_close(from, reason, detail); });

		link::world_register request{};
		if (!request.world.assign(config.world_name) || !request.client_host.assign(registration.client_host))
		{
			hb::log::error("world name or client address does not fit the register packet");
			connection->close(close_reason::protocol_error, "register packet");
			return;
		}
		request.client_port = registration.client_port;
		request.key = config.link_key;
		request.max_players = registration.max_players;
		(void)connection->send(request);
	}

	void login_link::impl::on_frame(hb::net::connection& from, const hb::net::frame& received)
	{
		if (!registered)
		{
			on_register_response(from, received);
			return;
		}
		const std::uint16_t id = received.header.message_id;
		switch (static_cast<message_id>(id))
		{
		case message_id::link_heartbeat_from_login:
		{
			link::link_heartbeat beat{};
			if (!hb::net::decode_heartbeat(received, message_id::link_heartbeat_from_login, beat))
				from.close(close_reason::protocol_error, "malformed heartbeat");
			else
				hb::log::debug("heartbeat {} from the Login server", beat.sequence);
			return;  // any whole frame has already reset the idle timer
		}
		case message_id::link_character_list_request:
			on_character_list(from, received);
			return;
		case message_id::link_create_character_request:
			on_create_character(from, received);
			return;
		case message_id::link_delete_character_request:
			on_delete_character(from, received);
			return;
		case message_id::link_session_grant:
			on_session_grant(from, received);
			return;
		case message_id::link_kick_account:
			on_kick_account(from, received);
			return;
		default:
			hb::log::warning("unexpected message 0x{:04x} from the Login server", id);
			return;
		}
	}

	void login_link::impl::on_character_list(hb::net::connection& from, const hb::net::frame& received)
	{
		link::character_list_request request{};
		if (!hb::net::decode(received, request))
		{
			from.close(close_reason::protocol_error, "malformed character_list_request");
			return;
		}
		character_list_answer answer;
		if (events.characters != nullptr)
			answer = events.characters->list(request.account_id);
		link::character_list_response head{};
		head.request_id = request.request_id;
		head.max_slots = answer.max_slots;
		head.count = static_cast<std::uint8_t>(answer.summaries.size());
		std::vector<std::byte> bytes;
		if (!hb::net::encode_character_list(head, std::span<const hb::net::character_summary>(answer.summaries),
				hb::net::connection_options{}.max_body_bytes, bytes))
		{
			hb::log::error("character list for account {} does not fit the packet", request.account_id);
			head.count = 0;
			if (!hb::net::encode_character_list(head, std::span<const hb::net::character_summary>(), hb::net::connection_options{}.max_body_bytes, bytes))
			{
				// The Login server would wait for an answer that never comes; the link is reset instead.
				hb::log::error("empty character list for account {} does not encode; resetting the Login link", request.account_id);
				from.close(close_reason::protocol_error, "character_list_response encode");
				return;
			}
		}
		from.send_encoded(std::move(bytes));
	}

	void login_link::impl::on_create_character(hb::net::connection& from, const hb::net::frame& received)
	{
		link::create_character_request request{};
		if (!hb::net::decode(received, request))
		{
			from.close(close_reason::protocol_error, "malformed create_character_request");
			return;
		}
		link::character_change_response response{};
		response.request_id = request.request_id;
		response.result = events.characters != nullptr ? events.characters->create(request) : hb::net::login::character_result::world_unavailable;
		(void)from.send(response);
	}

	void login_link::impl::on_delete_character(hb::net::connection& from, const hb::net::frame& received)
	{
		link::delete_character_request request{};
		if (!hb::net::decode(received, request))
		{
			from.close(close_reason::protocol_error, "malformed delete_character_request");
			return;
		}
		link::character_change_response response{};
		response.request_id = request.request_id;
		response.result = events.characters != nullptr ? events.characters->remove(request) : hb::net::login::character_result::world_unavailable;
		(void)from.send(response);
	}

	void login_link::impl::on_session_grant(hb::net::connection& from, const hb::net::frame& received)
	{
		link::session_grant grant{};
		if (!hb::net::decode(received, grant))
		{
			from.close(close_reason::protocol_error, "malformed session_grant");
			return;
		}
		session_grant_request request;
		request.account_id = grant.account_id;
		request.character = std::string(grant.character.view());
		request.ticket = grant.ticket;
		request.expires_in_seconds = grant.expires_in_seconds;
		link::session_grant_response response{};
		response.request_id = grant.request_id;
		response.result = events.on_session_grant ? static_cast<link::grant_result>(events.on_session_grant(request))
												  : link::grant_result::shutting_down;
		if (!link::is_known(response.result))
			response.result = link::grant_result::shutting_down;
		(void)from.send(response);
	}

	void login_link::impl::on_kick_account(hb::net::connection& from, const hb::net::frame& received)
	{
		link::kick_account kick{};
		if (!hb::net::decode(received, kick))
		{
			from.close(close_reason::protocol_error, "malformed kick_account");
			return;
		}
		if (events.on_kick)
			events.on_kick(kick.account_id, static_cast<std::uint8_t>(kick.reason));
	}

	void login_link::impl::on_register_response(hb::net::connection& from, const hb::net::frame& received)
	{
		link::world_register_response response{};
		if (!hb::net::decode(received, response))
		{
			from.close(close_reason::protocol_error, "the first answer must be a world_register_response");
			return;
		}
		if (response.result != link::register_result::ok)
		{
			const std::string_view why = to_text(response.result);
			if (is_permanent(response.result))
			{
				refused = true;
				hb::log::error("the Login server refused this World: {}", why);
				client->stop();
				if (events.on_refused)
					events.on_refused(why);
			}
			else
			{
				hb::log::warning("the Login server refused this World: {} (retrying)", why);
				from.close(close_reason::protocol_error, why);
			}
			return;
		}
		registered = true;
		from.set_deadline(std::chrono::milliseconds::zero());  // registered: the heartbeats keep it alive now
		start_timers(from);
		hb::log::info("registered with the Login server as {}", config.world_name);
		if (events.on_registered)
			events.on_registered();
		send_status(true);
	}

	void login_link::impl::on_close(hb::net::connection&, close_reason reason, std::string_view detail)
	{
		stop_timers();
		const bool was_registered = std::exchange(registered, false);
		current.reset();
		if (stopping || refused)
			return;
		const char* separator = detail.empty() ? "" : ", ";
		if (was_registered)
		{
			hb::log::warning("Login link lost: {}{}{}", hb::net::to_string(reason), separator, detail);
			if (events.on_lost)
				events.on_lost();
		}
		else
		{
			hb::log::warning("Login link closed before registering: {}{}{}", hb::net::to_string(reason), separator, detail);
		}
	}

	void login_link::impl::start_timers(hb::net::connection& from)
	{
		heartbeat = hb::net::periodic_timer::create(from.executor());
		heartbeat->start(heartbeat_interval(), [this] { send_heartbeat(); });
		status_timer = hb::net::periodic_timer::create(from.executor());
		status_timer->start(status_interval, [this] { send_status(true); });
	}

	void login_link::impl::stop_timers()
	{
		if (heartbeat)
			heartbeat->stop();
		if (status_timer)
			status_timer->stop();
		heartbeat.reset();
		status_timer.reset();
	}

	void login_link::impl::send_heartbeat()
	{
		if (!current || !registered)
			return;
		std::vector<std::byte> bytes;
		if (hb::net::encode_heartbeat(message_id::link_heartbeat_from_world, link::link_heartbeat{ ++sequence },
				hb::net::connection_options{}.max_body_bytes, bytes))
		{
			current->send_encoded(std::move(bytes));
			hb::log::debug("heartbeat {} sent", sequence);
		}
	}

	void login_link::impl::send_status(bool force)
	{
		if (!current || !registered || !events.report)
			return;
		const link_report report = events.report();
		if (!force && report == last_report)
			return;
		last_report = report;
		link::world_status status{};
		status.is_accepting_players = report.is_accepting_players ? 1 : 0;
		status.is_shutting_down = report.is_shutting_down ? 1 : 0;
		status.player_count = report.player_count;
		status.max_players = report.max_players;
		(void)current->send(status);
	}

	login_link::login_link(const world_config& config, link_registration registration, link_events events)
		: m_impl(std::make_unique<impl>(config, std::move(registration), std::move(events)))
	{
	}

	login_link::~login_link() = default;

	void login_link::start()
	{
		m_impl->begin();
	}

	void login_link::poll()
	{
		m_impl->context.restart();
		m_impl->context.poll();
		if (m_impl->registered)
			m_impl->send_status(false);
	}

	void login_link::stop()
	{
		impl& state = *m_impl;
		state.stopping = true;
		if (!state.client)
			return;
		state.stop_timers();
		// WORLD-005: the shutdown's session_ended frames are queued but not yet written. Close after sending, and poll
		// (bounded) until they are out, before the client stops: its close drops whatever is still queued.
		if (state.current)
		{
			state.current->close_after_send(close_reason::shutdown, "World stopping");
			const auto flush_limit = std::chrono::steady_clock::now() + drain_limit;
			while (state.current && std::chrono::steady_clock::now() < flush_limit)
			{
				state.context.restart();
				state.context.poll();
				if (state.current)
					std::this_thread::sleep_for(5ms);
			}
		}
		state.client->stop();
		const auto limit = std::chrono::steady_clock::now() + drain_limit;
		while ((state.current || state.client->current()) && std::chrono::steady_clock::now() < limit)
		{
			state.context.restart();
			state.context.poll();
			std::this_thread::sleep_for(5ms);
		}
	}

	bool login_link::is_registered() const
	{
		return m_impl->registered;
	}

	void login_link::send_session_started(std::uint64_t account_id, std::string_view character)
	{
		if (!m_impl->current || !m_impl->registered)
		{
			hb::log::debug("session_started for account {} dropped: Login link down", account_id);
			return;
		}
		link::session_started message{};
		message.account_id = account_id;
		if (!message.character.assign(character))
		{
			hb::log::error("character name {} does not fit session_started", character);
			return;
		}
		(void)m_impl->current->send(message);
	}

	void login_link::send_session_ended(std::uint64_t account_id, std::uint8_t reason)
	{
		if (!m_impl->current || !m_impl->registered)
		{
			hb::log::debug("session_ended for account {} dropped: Login link down", account_id);
			return;
		}
		link::session_ended message{};
		message.account_id = account_id;
		message.reason = static_cast<link::session_end_reason>(reason);
		(void)m_impl->current->send(message);
	}
}
