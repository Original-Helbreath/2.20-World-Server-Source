#pragma once

// The calls the legacy game logic makes into the World's host (WORLD-005), implemented in world_host.cpp. Plain
// types only, so Game.cpp can include this without Asio or Schemata.

#include <cstdint>

// Saves the player in slot iClientH to players.db. Non-zero when saved; zero (logged) when it could not be.
int world_save_player(int iClientH);

// Tells the Login server the account's player is in the game.
void world_session_started(std::int64_t account_id, const char * character_name);

// Tells the Login server the account left the World. reason is an hb::net::link::session_end_reason value.
void world_session_ended(std::int64_t account_id, char reason);
