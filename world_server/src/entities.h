#pragma once

#include <schemata/schemata.h>

#include <cstddef>
#include <cstdint>
#include <string>

// The World server's tables in players.db (plan sect. 3): the character and its items, skills and magic. The Login
// server owns `accounts` and `login_events` in the same file; each server's model declares only its own tables, so this
// model never names, creates, alters or drops them.
//
// The fields follow the 2.20 character file the game server wrote (_iComposePlayerDataFileContents), one column per
// value. Legacy packed values (appr1..4, item attribute) are kept as the game logic uses them for now; they become
// flags when the client packets are converted.
namespace hb::server::world
{
	constexpr std::size_t max_character_name_length = 10;
	constexpr std::size_t max_map_name_length = 10;
	constexpr std::size_t max_guild_name_length = 20;
	constexpr std::size_t max_profile_length = 255;
	constexpr std::size_t max_item_name_length = 20;

	// One character. `id` is the character's key for every child table.
	struct character
	{
		std::int64_t id = 0;
		// The Login server's accounts.id: a foreign key into a table this model does not map (see describe below).
		std::int64_t account_id = 0;
		std::string name;
		// Lowercase name: the unique key, so "Bob" and "bob" are one character.
		std::string name_key;
		std::string profile;
		std::string home_location;

		std::string guild_name;
		std::int32_t guild_guid = -1;
		std::int32_t guild_rank = -1;

		std::string map_name;
		std::int32_t x = 0;
		std::int32_t y = 0;

		std::int32_t hp = 0;
		std::int32_t mp = 0;
		std::int32_t sp = 0;
		std::int32_t level = 1;
		std::int32_t rating = 0;
		std::int32_t strength = 0;
		std::int32_t intelligence = 0;
		std::int32_t vitality = 0;
		std::int32_t dexterity = 0;
		std::int32_t magic = 0;
		std::int32_t charisma = 0;
		std::int32_t luck = 0;
		std::int32_t exp = 0;
		// Pending level-up stat points (2.20 LU_Str ... LU_Char).
		std::int32_t lu_strength = 0;
		std::int32_t lu_vitality = 0;
		std::int32_t lu_dexterity = 0;
		std::int32_t lu_intelligence = 0;
		std::int32_t lu_magic = 0;
		std::int32_t lu_charisma = 0;
		std::int32_t enemy_kill_count = 0;
		std::int32_t pk_count = 0;
		std::int32_t reward_gold = 0;
		std::int32_t down_skill_index = -1;
		std::int32_t id_num1 = 0;
		std::int32_t id_num2 = 0;
		std::int32_t id_num3 = 0;

		std::int32_t sex = 0;
		std::int32_t skin = 0;
		std::int32_t hair_style = 0;
		std::int32_t hair_color = 0;
		std::int32_t underwear = 0;
		std::int32_t appr1 = 0;
		std::int32_t appr2 = 0;
		std::int32_t appr3 = 0;
		std::int32_t appr4 = 0;
		std::int32_t appr_color = 0;

		std::int32_t hunger_status = 100;
		std::int32_t time_left_shut_up = 0;
		std::int32_t time_left_rating = 0;
		std::int32_t time_left_force_recall = 0;
		std::int32_t time_left_firm_staminar = 0;
		std::int32_t admin_user_level = 0;
		std::int32_t penalty_block_year = 0;
		std::int32_t penalty_block_month = 0;
		std::int32_t penalty_block_day = 0;

		std::int32_t quest = 0;
		std::int32_t quest_id = 0;
		std::int32_t current_quest_count = 0;
		std::int32_t quest_reward_type = 0;
		std::int32_t quest_reward_amount = 0;
		bool quest_completed = false;
		std::int32_t contribution = 0;
		std::int32_t war_contribution = 0;
		std::int32_t special_event_id = 0;
		std::int32_t super_attack_left = 0;
		std::int32_t fightzone_number = 0;
		std::int32_t reserve_time = 0;
		std::int32_t fightzone_ticket_number = 0;
		std::int32_t special_ability_time = 0;
		std::string locked_map_name;
		std::int32_t locked_map_time = 0;
		std::int32_t crusade_duty = 0;
		std::int64_t crusade_guid = 0;
		std::int32_t construction_point = 0;
		std::int32_t dead_penalty_time = 0;
		std::int32_t party_id = 0;
		std::int32_t gizon_item_upgrade_left = 0;

		schemata::timestamp created_at{};
		schemata::timestamp saved_at{};
	};

	// One item the character carries or keeps in the bank (2.20 character-item / character-bank-item).
	struct character_item
	{
		std::int64_t id = 0;
		std::int64_t character_id = 0;
		bool in_bank = false;
		// Position in the bag or the bank list; unique per character and place.
		std::int32_t slot = 0;
		std::string name;
		std::int64_t count = 1;
		std::int32_t touch_effect_type = 0;
		std::int32_t touch_effect_value1 = 0;
		std::int32_t touch_effect_value2 = 0;
		std::int32_t touch_effect_value3 = 0;
		std::int32_t color = 0;
		std::int32_t spec_effect_value1 = 0;
		std::int32_t spec_effect_value2 = 0;
		std::int32_t spec_effect_value3 = 0;
		std::int32_t cur_life_span = 0;
		std::int64_t attribute = 0;
		// Bag only: worn, and the item's position in the client's bag window.
		bool equipped = false;
		std::int32_t pos_x = 40;
		std::int32_t pos_y = 30;
	};

	// One skill with progress (2.20 skill-mastery and skill-SSN). Only skills with progress have a row.
	struct character_skill
	{
		std::int64_t id = 0;
		std::int64_t character_id = 0;
		std::int32_t skill_id = 0;
		std::int32_t mastery = 0;
		std::int32_t ssn = 0;
	};

	// One magic the character has learned (2.20 magic-mastery). Only learned magic has a row.
	struct character_magic
	{
		std::int64_t id = 0;
		std::int64_t character_id = 0;
		std::int32_t magic_id = 0;
		std::int32_t mastery = 1;
	};
}

template <>
struct schemata::Entity<hb::server::world::character>
{
	static void describe(schemata::EntityBuilder<hb::server::world::character>& e)
	{
		using hb::server::world::character;
		namespace world = hb::server::world;
		e.table("characters");
		SCH_KEY(e, id).identity();
		// accounts belongs to the Login server and is never mapped here: deleting an account deletes its characters. If the
		// Login server has not created the table yet the key is deferred and the next start adds it (WORLD-007).
		e.references_unowned({ "account_id" }, "accounts", { "id" }).on_delete(schemata::ReferentialAction::cascade);
		SCH_PROP(e, account_id).indexed();
		SCH_PROP(e, name).max_length(world::max_character_name_length);
		SCH_PROP(e, name_key).max_length(world::max_character_name_length).unique();
		SCH_PROP(e, profile).max_length(world::max_profile_length);
		SCH_PROP(e, home_location).max_length(world::max_map_name_length);
		SCH_PROP(e, guild_name).max_length(world::max_guild_name_length);
		SCH_PROP(e, guild_guid);
		SCH_PROP(e, guild_rank);
		SCH_PROP(e, map_name).max_length(world::max_map_name_length);
		SCH_PROP(e, x);
		SCH_PROP(e, y);
		SCH_PROP(e, hp);
		SCH_PROP(e, mp);
		SCH_PROP(e, sp);
		SCH_PROP(e, level);
		SCH_PROP(e, rating);
		SCH_PROP(e, strength);
		SCH_PROP(e, intelligence);
		SCH_PROP(e, vitality);
		SCH_PROP(e, dexterity);
		SCH_PROP(e, magic);
		SCH_PROP(e, charisma);
		SCH_PROP(e, luck);
		SCH_PROP(e, exp);
		SCH_PROP(e, lu_strength);
		SCH_PROP(e, lu_vitality);
		SCH_PROP(e, lu_dexterity);
		SCH_PROP(e, lu_intelligence);
		SCH_PROP(e, lu_magic);
		SCH_PROP(e, lu_charisma);
		SCH_PROP(e, enemy_kill_count);
		SCH_PROP(e, pk_count);
		SCH_PROP(e, reward_gold);
		SCH_PROP(e, down_skill_index);
		SCH_PROP(e, id_num1);
		SCH_PROP(e, id_num2);
		SCH_PROP(e, id_num3);
		SCH_PROP(e, sex);
		SCH_PROP(e, skin);
		SCH_PROP(e, hair_style);
		SCH_PROP(e, hair_color);
		SCH_PROP(e, underwear);
		SCH_PROP(e, appr1);
		SCH_PROP(e, appr2);
		SCH_PROP(e, appr3);
		SCH_PROP(e, appr4);
		SCH_PROP(e, appr_color);
		SCH_PROP(e, hunger_status);
		SCH_PROP(e, time_left_shut_up);
		SCH_PROP(e, time_left_rating);
		SCH_PROP(e, time_left_force_recall);
		SCH_PROP(e, time_left_firm_staminar);
		SCH_PROP(e, admin_user_level);
		SCH_PROP(e, penalty_block_year);
		SCH_PROP(e, penalty_block_month);
		SCH_PROP(e, penalty_block_day);
		SCH_PROP(e, quest);
		SCH_PROP(e, quest_id);
		SCH_PROP(e, current_quest_count);
		SCH_PROP(e, quest_reward_type);
		SCH_PROP(e, quest_reward_amount);
		SCH_PROP(e, quest_completed).default_sql("0");
		SCH_PROP(e, contribution);
		SCH_PROP(e, war_contribution);
		SCH_PROP(e, special_event_id);
		SCH_PROP(e, super_attack_left);
		SCH_PROP(e, fightzone_number);
		SCH_PROP(e, reserve_time);
		SCH_PROP(e, fightzone_ticket_number);
		SCH_PROP(e, special_ability_time);
		SCH_PROP(e, locked_map_name).max_length(world::max_map_name_length);
		SCH_PROP(e, locked_map_time);
		SCH_PROP(e, crusade_duty);
		SCH_PROP(e, crusade_guid);
		SCH_PROP(e, construction_point);
		SCH_PROP(e, dead_penalty_time);
		SCH_PROP(e, party_id);
		SCH_PROP(e, gizon_item_upgrade_left);
		SCH_PROP(e, created_at);
		SCH_PROP(e, saved_at);
	}
};

template <>
struct schemata::Entity<hb::server::world::character_item>
{
	static void describe(schemata::EntityBuilder<hb::server::world::character_item>& e)
	{
		using hb::server::world::character;
		using hb::server::world::character_item;
		e.table("character_items");
		SCH_KEY(e, id).identity();
		SCH_PROP(e, character_id);
		e.belongs_to<character>(&character_item::character_id).on_delete(schemata::ReferentialAction::cascade);
		SCH_PROP(e, in_bank).default_sql("0");
		SCH_PROP(e, slot);
		SCH_PROP(e, name).max_length(hb::server::world::max_item_name_length);
		SCH_PROP(e, count);
		SCH_PROP(e, touch_effect_type);
		SCH_PROP(e, touch_effect_value1);
		SCH_PROP(e, touch_effect_value2);
		SCH_PROP(e, touch_effect_value3);
		SCH_PROP(e, color);
		SCH_PROP(e, spec_effect_value1);
		SCH_PROP(e, spec_effect_value2);
		SCH_PROP(e, spec_effect_value3);
		SCH_PROP(e, cur_life_span);
		SCH_PROP(e, attribute);
		SCH_PROP(e, equipped).default_sql("0");
		SCH_PROP(e, pos_x);
		SCH_PROP(e, pos_y);
		e.index({ "character_id", "in_bank", "slot" }, true);
	}
};

template <>
struct schemata::Entity<hb::server::world::character_skill>
{
	static void describe(schemata::EntityBuilder<hb::server::world::character_skill>& e)
	{
		using hb::server::world::character;
		using hb::server::world::character_skill;
		e.table("character_skills");
		SCH_KEY(e, id).identity();
		SCH_PROP(e, character_id);
		e.belongs_to<character>(&character_skill::character_id).on_delete(schemata::ReferentialAction::cascade);
		SCH_PROP(e, skill_id);
		SCH_PROP(e, mastery);
		SCH_PROP(e, ssn);
		e.index({ "character_id", "skill_id" }, true);
	}
};

template <>
struct schemata::Entity<hb::server::world::character_magic>
{
	static void describe(schemata::EntityBuilder<hb::server::world::character_magic>& e)
	{
		using hb::server::world::character;
		using hb::server::world::character_magic;
		e.table("character_magic");
		SCH_KEY(e, id).identity();
		SCH_PROP(e, character_id);
		e.belongs_to<character>(&character_magic::character_id).on_delete(schemata::ReferentialAction::cascade);
		SCH_PROP(e, magic_id);
		SCH_PROP(e, mastery);
		e.index({ "character_id", "magic_id" }, true);
	}
};
