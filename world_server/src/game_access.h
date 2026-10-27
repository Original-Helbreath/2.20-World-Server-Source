#pragma once

// What the World's host (world_host.cpp) asks of the legacy game logic (WORLD-005), implemented in game_access.cpp
// next to CGame. Plain types only: the host builds strict and never includes Game.h.

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class world_client_socket;

namespace hb::server::world
{
	struct character_record;

	enum class game_state
	{
		accepting,
		not_ready,      // game files or the Login link not up yet
		full,
		shutting_down,
	};

	game_state game_entry_state();

	// Takes a client slot for an entering player and loads the record into it, with every rule a 2.20 character load
	// ran. Nothing is sent to the client yet. Returns the slot; on failure (error says why) returns -1 and the socket
	// is released (the game deleted it or never took it).
	int game_prepare_player(std::unique_ptr<world_client_socket> socket, std::int64_t account_id, const character_record& record,
		std::string& error);

	// Saves and removes any player of this account, or with this character name, still in the game (session_ended
	// kicked). Called before the World-full check, so a re-entry is never refused because of its own old session.
	void game_replace_account(std::int64_t account_id, std::string_view character);

	// Finishes the entry of a prepared player: sends INITPLAYER confirm and marks the player in the game (which sends
	// session_started). The world_enter_response ok must already be on its way.
	void game_finish_player(int client_index);

	void game_client_message(int client_index, const std::vector<std::byte>& message);
	void game_client_closed(int client_index);

	// The player in this slot as a record; false when the slot holds no player that finished entering.
	bool game_capture_player(int client_index, character_record& record);

	// Removes the account's player, saving it, with the session end reason (hb::net::link::session_end_reason).
	void game_remove_account(std::int64_t account_id, std::uint8_t end_reason);

	// Saves and removes every player (shutdown).
	void game_remove_all(std::uint8_t end_reason);

	// Every player in the game: account id and character name.
	void game_for_each_player(const std::function<void(std::int64_t account_id, std::string_view character)>& visit);
}
