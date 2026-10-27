#pragma once

#include "database.h"
#include "entities.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Loading and saving a character with everything that belongs to it, in one transaction. The game's own character
// structures are mapped to and from character_record by the code that loads and saves players on enter and logout.
namespace hb::server::world
{
	// A character and its child rows as the game holds them: slots in order, skills and magic with progress only.
	struct character_record
	{
		character header;
		std::vector<character_item> items;
		std::vector<character_skill> skills;
		std::vector<character_magic> magic;
	};

	enum class load_result
	{
		loaded,
		not_found,
		failed
	};

	class character_store
	{
	public:
		explicit character_store(database& db) : m_database(db) {}

		// Lowercase key a name is stored and found under.
		static std::string name_key(std::string_view name);

		bool exists(std::string_view name, std::string& error);

		// Reads a character by name, any case. Items are ordered bag then bank, by slot.
		load_result load(std::string_view name, character_record& record, std::string& error);

		// Inserts a new character (header.id == 0; the id is set on return) or updates an existing one, and replaces
		// its items, skills and magic, all or nothing. saved_at is stamped here; created_at is stamped on insert.
		bool save(character_record& record, std::string& error);

		// Deletes a character; its items, skills and magic go with it through the foreign keys.
		bool remove(std::string_view name, std::string& error);

	private:
		database& m_database;
	};
}
