#pragma once

#include "character_store.h"
#include "common_types.h"
#include "link_messages.h"
#include "login_messages.h"

#include <cstdint>
#include <string_view>
#include <vector>

// The World's answers to the Login server's character requests (WORLD-006): the select-screen list, create and
// delete, from players.db through character_store. The World is authoritative for every rule the Login server only
// pre-checks (plan M13, M14); a new character gets the 2.20 first-entry defaults (WLS HandleCreateNewCharacter.c).
namespace hb::server::world
{
	inline constexpr std::uint8_t max_character_slots = 4;
	inline constexpr std::int32_t max_delete_level = 49;  // a character at level 50 or above cannot be deleted

	struct character_list_answer
	{
		std::uint8_t max_slots = max_character_slots;
		std::vector<hb::net::character_summary> summaries;
	};

	// Format checks a create request must pass: name 1..10 ASCII letters or digits, stats 10..14 adding up to 70,
	// sex 1..2, skin 1..3, hair style, hair colour and underwear colour 0..15 (each fills a 4-bit field of appr1).
	hb::net::login::character_result check_new_character(const hb::net::link::create_character_request& request);

	// The row a new character is stored as: map "default" at -1,-1, level 1, luck 10, the request's stats and looks,
	// HP/MP/SP at the game's maximum for those stats, and the 2.20 starting kit (trousers worn, plus the shirt for a
	// male character). The request must already have passed check_new_character.
	character_record make_new_character(const hb::net::link::create_character_request& request);

	// A stored character as one select-screen slot.
	hb::net::character_summary to_summary(const character& stored);

	class character_service
	{
	public:
		explicit character_service(character_store& store) : m_store(store) {}

		// An account's characters; on a database fault the list is empty and the fault is logged.
		character_list_answer list(std::uint64_t account_id);

		hb::net::login::character_result create(const hb::net::link::create_character_request& request);

		// Deletes only a character the account owns and only below level 50.
		hb::net::login::character_result remove(const hb::net::link::delete_character_request& request);

	private:
		character_store& m_store;
	};
}
