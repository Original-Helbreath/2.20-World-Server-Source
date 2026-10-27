#pragma once

#include "world_client_socket.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace hb::net::world
{
	struct world_enter_request;
	enum class world_enter_result : std::uint8_t;
}

// The World's game-client port on the shared hb::net layer (WORLD-005), replacing the legacy XSocket listener.
// The first frame of a connection must be world_enter_request (within 10 s); after an ok world_enter_response every
// frame is a world_game_message from the client and a client_game_message to it, each carrying one legacy game message.
//
// Asio stays out of this header. Nothing runs inside the network layer's callbacks: they queue what arrived, and
// poll() hands it to the handlers afterwards, on the calling thread, in arrival order. So the game code the handlers
// call may delete clients and close connections freely.
namespace hb::server::world
{
	class client_session;

	struct gate_events
	{
		// A connection presented its ticket. The handler answers with refuse() or admit() before returning.
		std::function<void(const std::shared_ptr<client_session>&, const hb::net::world::world_enter_request&)> on_enter;
		// One legacy game message from the player in this client slot.
		std::function<void(int client_index, const std::vector<std::byte>& message)> on_message;
		// The player's connection closed (by the peer, an error or a timeout), not after the game closed it itself.
		std::function<void(int client_index)> on_closed;
	};

	class client_gate
	{
	public:
		explicit client_gate(gate_events events);
		~client_gate();
		client_gate(const client_gate&) = delete;
		client_gate& operator=(const client_gate&) = delete;

		// Binds the port; false with the reason on failure.
		bool listen(const std::string& address, std::uint16_t port, std::size_t max_connections, std::string& error);

		// Runs whatever network work is ready without blocking, then delivers the queued events.
		void poll();

		// Closes every connection and stops listening; drains for a moment so the closes go out.
		void stop();

		// Answers the ticket with a refusal and hangs up once it is sent.
		static void refuse(client_session& session, hb::net::world::world_enter_result result);

		// The socket the game will own for this session. Until admit() it discards what the game sends, so nothing the
		// game emits while loading the character reaches the client ahead of world_enter_response.
		static std::unique_ptr<world_client_socket> make_socket(const std::shared_ptr<client_session>& session);

		// Sends world_enter_response ok, binds the session to its client slot and lets the socket send.
		static void admit(client_session& session, int client_index);
	};
}
