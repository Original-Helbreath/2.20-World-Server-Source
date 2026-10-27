#pragma once

#include "log.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

// world_server.json (JSON with comments, WORLD-003): how this World reaches and identifies itself to the Login
// server (and how loud it logs). The legacy gserver.cfg keeps the game's own settings until those move into world.db.
namespace hb::server::world
{
	// The wire field (hb::net::world_text) is 32 characters; login_link.cpp checks the two agree.
	inline constexpr std::size_t world_name_max_length = 32;
	inline constexpr std::size_t link_key_bytes = 32;

	struct world_config
	{
		std::string login_address = "127.0.0.1";  // the Login server's World link
		std::uint16_t login_port = 3840;          // its listen.world_port
		std::string world_name;                   // must match a worlds[] name in login_server.json
		std::array<std::uint8_t, link_key_bytes> link_key{};  // shared secret, 64 hex digits in the file; never logged
		std::uint32_t heartbeat_seconds = 5;      // link_heartbeat every this long; 3 missed from Login = loss
		hb::log::level log_level = hb::log::level::info;  // debug also logs every heartbeat
		std::string players_database = "data/players.db"; // shared with the Login server (its database.path)
		std::string world_database = "data/world.db";     // the templates (WORLD-008); built by world_import
	};

	// False with a message in error when the file is missing, is not valid JSON, has an unknown key or has a
	// value out of range. The config is left untouched on failure.
	bool load_world_config(const std::filesystem::path& path, world_config& config, std::string& error);

	// True for the all-zero key the sample files ship with.
	bool is_sample_key(const world_config& config);
}
