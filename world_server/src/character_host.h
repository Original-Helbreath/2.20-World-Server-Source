#pragma once

#include <string>

// Owns players.db and the character service for the console host (WORLD-006), so the legacy translation units
// (main.cpp builds with the game's settings) never include Schemata.
namespace hb::server::world
{
	class character_service;

	// Opens players.db and creates or heals the character tables; null (with the reason logged) on a database fault.
	character_service* open_characters(const std::string& players_database);

	// The store open_characters opened (WORLD-005: loading and saving players); null when nothing is open.
	class character_store;
	character_store* characters_store();

	// Releases what open_characters made. Safe to call when nothing is open.
	void close_characters();
}
