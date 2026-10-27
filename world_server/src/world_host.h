#pragma once

// The World's session handling (WORLD-005): the Login server's grants, the game-client port and the players' entry,
// saving and leaving. main.cpp drives it; no Asio, Schemata or Windows types here.

#include "login_link.h"

#include <cstdint>
#include <string>

namespace hb::server::world
{
	class character_store;

	// Opens the game-client port. False (logged) when it cannot be bound.
	bool host_start(const std::string& bind_address, std::uint16_t port, std::size_t max_players, character_store& store);

	// The Login link the session messages go out on (null to detach).
	void host_attach_link(login_link* link);

	// Network work and queued events of the client port; expired grants are dropped.
	void host_poll();

	// Saves and removes every player, telling the Login server; then closes the port.
	void host_stop();

	// login_link events.
	std::uint8_t host_session_grant(const session_grant_request& grant);
	void host_kick(std::uint64_t account_id, std::uint8_t reason);
	void host_announce_players();  // session_started for every player, after (re)registering
}
