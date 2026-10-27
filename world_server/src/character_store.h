#pragma once

#include "database.h"
#include "entities.h"
#include "template_references.h"

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
		// templates resolves the character rows' keys into world.db (skill ids) before a load is returned or a save
		// is written; an unresolved key fails the call.
		character_store(database& db, const template_references& templates) : m_database(db), m_templates(templates) {}

		// Lowercase key a name is stored and found under.
		static std::string name_key(std::string_view name);

		bool exists(std::string_view name, std::string& error);

		// The headers (no child rows) of an account's characters, oldest first. False (with the reason) on a fault.
		bool list_for_account(std::int64_t account_id, std::vector<character>& characters, std::string& error);

		// Reads a character by name, any case. Items are ordered bag then bank, by slot.
		load_result load(std::string_view name, character_record& record, std::string& error);

		// Inserts a new character (header.id == 0; the id is set on return) or updates an existing one, and replaces
		// its items, skills and magic, all or nothing. saved_at is stamped here; created_at is stamped on insert.
		bool save(character_record& record, std::string& error);

		// Deletes a character; its items, skills and magic go with it through the foreign keys.
		bool remove(std::string_view name, std::string& error);

	private:
		bool resolve_templates(const character_record& record, std::string& error) const;

		database& m_database;
		const template_references& m_templates;
	};
}
