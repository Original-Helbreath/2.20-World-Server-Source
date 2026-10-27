#include "character_service.h"

#include "log.h"

#include <array>
#include <chrono>
#include <string>

namespace hb::server::world
{
	namespace
	{
		using hb::net::login::character_result;

		constexpr std::uint8_t min_stat = 10;
		constexpr std::uint8_t max_stat = 14;
		constexpr unsigned stat_total = 70;
		constexpr std::uint8_t max_nibble = 15;

		bool is_valid_name(std::string_view name)
		{
			if (name.empty() || name.size() > max_character_name_length)
				return false;
			for (const char c : name)
			{
				const bool letter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
				const bool digit = c >= '0' && c <= '9';
				if (!letter && !digit)
					return false;
			}
			return true;
		}

		bool is_valid_stats(const hb::net::character_stats& stats)
		{
			const std::array<std::uint8_t, 6> values{ stats.strength, stats.vitality, stats.dexterity, stats.intelligence, stats.magic,
				stats.charisma };
			unsigned total = 0;
			for (const std::uint8_t value : values)
			{
				if (value < min_stat || value > max_stat)
					return false;
				total += value;
			}
			return total == stat_total;
		}

		// One starting-kit item as the 2.20 file wrote it: count 1, no effects, life span 300.
		character_item kit_item(std::int32_t slot, std::string name, std::int32_t color, bool equipped)
		{
			character_item item;
			item.slot = slot;
			item.name = std::move(name);
			item.count = 1;
			item.color = color;
			item.cur_life_span = 300;
			item.equipped = equipped;
			return item;
		}

		std::uint8_t nibble(std::int32_t value, int shift)
		{
			return static_cast<std::uint8_t>((static_cast<std::uint32_t>(value) >> shift) & 0x0Fu);
		}

		hb::net::minute_stamp to_minute_stamp(const schemata::timestamp& when)
		{
			const auto day = std::chrono::floor<std::chrono::days>(when);
			const std::chrono::year_month_day date(day);
			const std::chrono::hh_mm_ss<std::chrono::minutes> time(std::chrono::floor<std::chrono::minutes>(when - day));
			hb::net::minute_stamp stamp{};
			stamp.year = static_cast<std::uint16_t>(static_cast<int>(date.year()));
			stamp.month = static_cast<std::uint8_t>(static_cast<unsigned>(date.month()));
			stamp.day = static_cast<std::uint8_t>(static_cast<unsigned>(date.day()));
			stamp.hour = static_cast<std::uint8_t>(time.hours().count());
			stamp.minute = static_cast<std::uint8_t>(time.minutes().count());
			return stamp;
		}
	}

	character_result check_new_character(const hb::net::link::create_character_request& request)
	{
		if (!request.name.is_zero_padded() || !is_valid_name(request.name.view()))
			return character_result::invalid_name;
		if (!is_valid_stats(request.stats))
			return character_result::invalid_stats;
		if (request.sex < 1 || request.sex > 2 || request.skin < 1 || request.skin > 3)
			return character_result::failed;
		if (request.hair_style > max_nibble || request.hair_colour > max_nibble || request.underwear_colour > max_nibble)
			return character_result::failed;
		return character_result::ok;
	}

	character_record make_new_character(const hb::net::link::create_character_request& request)
	{
		character_record record;
		character& c = record.header;
		c.account_id = static_cast<std::int64_t>(request.account_id);
		c.name = std::string(request.name.view());
		c.name_key = character_store::name_key(c.name);
		c.home_location = "NONE";
		c.guild_name = "NONE";
		c.guild_rank = -1;
		c.map_name = "default";
		c.x = -1;
		c.y = -1;
		c.level = 1;
		c.strength = request.stats.strength;
		c.vitality = request.stats.vitality;
		c.dexterity = request.stats.dexterity;
		c.intelligence = request.stats.intelligence;
		c.magic = request.stats.magic;
		c.charisma = request.stats.charisma;
		c.luck = 10;
		c.exp = 0;
		c.sex = request.sex;
		c.skin = request.skin;
		c.hair_style = request.hair_style;
		c.hair_color = request.hair_colour;
		c.underwear = request.underwear_colour;
		c.appr1 = (request.hair_style << 8) | (request.hair_colour << 4) | request.underwear_colour;
		// The 2.20 file's HP/MP/SP formulas were not recovered; these are the game's own maxima
		// (CGame::iGetMaxHP and the MP/SP limits in Game.cpp) for a level-1 character with these stats.
		c.hp = c.vitality * 3 + c.level * 2 + c.strength / 2;
		c.mp = c.magic * 2 + c.level * 2 + c.intelligence / 2;
		c.sp = c.strength * 2 + c.level * 2;

		// The shirt's colour came from a value the decompiled WLS does not name; 0 until it is recovered.
		const bool male = request.sex == 1;
		record.items.push_back(kit_item(0, "Dagger", 0, false));
		record.items.push_back(kit_item(1, "RecallScroll", 0, false));
		record.items.push_back(kit_item(2, "RedPotion", 0, false));
		record.items.push_back(kit_item(3, "BluePotion", 0, false));
		record.items.push_back(kit_item(4, "GreenPotion", 0, false));
		record.items.push_back(kit_item(5, "KneeTrousers(M)", 0, true));
		record.items.push_back(kit_item(6, male ? "Shirt(M)" : "Chemise(W)", 0, male));
		return record;
	}

	hb::net::character_summary to_summary(const character& stored)
	{
		hb::net::character_summary s{};
		(void)s.name.assign(stored.name);
		s.is_valid = 1;
		s.sex = static_cast<std::uint8_t>(stored.sex);
		s.skin = static_cast<std::uint8_t>(stored.skin);
		s.level = static_cast<std::uint16_t>(stored.level);
		s.experience = static_cast<std::uint32_t>(stored.exp);
		s.stats = { static_cast<std::uint8_t>(stored.strength), static_cast<std::uint8_t>(stored.vitality),
			static_cast<std::uint8_t>(stored.dexterity), static_cast<std::uint8_t>(stored.intelligence),
			static_cast<std::uint8_t>(stored.magic), static_cast<std::uint8_t>(stored.charisma) };
		hb::net::character_appearance& a = s.appearance;
		a.hair_style = nibble(stored.appr1, 8);
		a.hair_colour = nibble(stored.appr1, 4);
		a.underwear_colour = nibble(stored.appr1, 0);
		a.weapon = static_cast<std::uint8_t>((static_cast<std::uint32_t>(stored.appr2) >> 4) & 0xFFu);
		a.shield = nibble(stored.appr2, 0);
		a.body_armour = nibble(stored.appr3, 12);
		a.pants = nibble(stored.appr3, 8);
		a.helm = nibble(stored.appr3, 4);
		a.arm_armour = nibble(stored.appr3, 0);
		a.boots = nibble(stored.appr4, 12);
		a.mantle = nibble(stored.appr4, 8);
		a.hides_body_armour = (stored.appr4 & 0x80) != 0 ? 1 : 0;
		a.weapon_colour = nibble(stored.appr_color, 28);
		a.shield_colour = nibble(stored.appr_color, 24);
		a.armour_colour = nibble(stored.appr_color, 20);
		a.mantle_colour = nibble(stored.appr_color, 16);
		a.arm_colour = nibble(stored.appr_color, 12);
		a.pants_colour = nibble(stored.appr_color, 8);
		a.boots_colour = nibble(stored.appr_color, 4);
		a.helm_colour = nibble(stored.appr_color, 0);
		(void)s.map_name.assign(stored.map_name);
		s.last_saved = to_minute_stamp(stored.saved_at);
		return s;
	}

	character_list_answer character_service::list(std::uint64_t account_id)
	{
		character_list_answer answer;
		std::vector<character> stored;
		std::string error;
		if (!m_store.list_for_account(static_cast<std::int64_t>(account_id), stored, error))
		{
			hb::log::error("character list for account {} failed: {}", account_id, error);
			return answer;
		}
		for (const character& c : stored)
		{
			if (answer.summaries.size() == max_character_slots)
				break;
			answer.summaries.push_back(to_summary(c));
		}
		return answer;
	}

	character_result character_service::create(const hb::net::link::create_character_request& request)
	{
		const character_result format = check_new_character(request);
		if (format != character_result::ok)
			return format;
		const std::string_view name = request.name.view();
		std::string error;
		const bool taken = m_store.exists(name, error);
		if (!error.empty())
		{
			hb::log::error("create character {}: {}", name, error);
			return character_result::failed;
		}
		if (taken)
			return character_result::name_taken;
		std::vector<character> owned;
		if (!m_store.list_for_account(static_cast<std::int64_t>(request.account_id), owned, error))
		{
			hb::log::error("create character {}: {}", name, error);
			return character_result::failed;
		}
		if (owned.size() >= max_character_slots)
			return character_result::slots_full;
		character_record record = make_new_character(request);
		if (!m_store.save(record, error))
		{
			hb::log::error("create character {}: {}", name, error);
			return character_result::failed;
		}
		hb::log::info("character {} created for account {}", name, request.account_id);
		return character_result::ok;
	}

	character_result character_service::remove(const hb::net::link::delete_character_request& request)
	{
		const std::string_view name = request.name.view();
		character_record record;
		std::string error;
		switch (m_store.load(name, record, error))
		{
		case load_result::not_found:
			return character_result::no_such_character;
		case load_result::failed:
			hb::log::error("delete character {}: {}", name, error);
			return character_result::failed;
		case load_result::loaded:
			break;
		}
		if (record.header.account_id != static_cast<std::int64_t>(request.account_id))
			return character_result::no_such_character;
		if (record.header.level > max_delete_level)
			return character_result::level_too_high;
		if (!m_store.remove(name, error))
		{
			hb::log::error("delete character {}: {}", name, error.empty() ? std::string("not removed") : error);
			return character_result::failed;
		}
		hb::log::info("character {} deleted for account {}", record.header.name, request.account_id);
		return character_result::ok;
	}
}
