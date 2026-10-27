// Round trip players.db record -> CClient -> record (WORLD-005). Built with the game's legacy settings, as
// player_mapping.cpp is, because it constructs the game's CClient and CItem.
#include "player_mapping.h"
#include "test_runner.h"

#include "Client.h"
#include "Item.h"

#include <cstring>
#include <format>
#include <memory>
#include <string>

namespace tests
{
	namespace
	{
		using namespace hb::server::world;

		character_record sample_record()
		{
			character_record r;
			character& h = r.header;
			h.id = 77;
			h.account_id = 5;
			h.name = "Hero1";
			h.name_key = "hero1";
			h.profile = "a profile";
			h.home_location = "aresden";
			h.guild_name = "Knights";
			h.guild_guid = 1234;
			h.guild_rank = 3;
			h.map_name = "aresden";
			h.x = 120;
			h.y = 98;
			h.hp = 40;
			h.mp = 30;
			h.sp = 20;
			h.level = 12;
			h.rating = 4;
			h.strength = 20;
			h.intelligence = 11;
			h.vitality = 15;
			h.dexterity = 14;
			h.magic = 12;
			h.charisma = 11;
			h.luck = 10;
			h.exp = 4567;
			h.lu_strength = 1;
			h.lu_vitality = 2;
			h.lu_dexterity = 0;
			h.lu_intelligence = 0;
			h.lu_magic = 0;
			h.lu_charisma = 0;
			h.enemy_kill_count = 6;
			h.pk_count = 1;
			h.reward_gold = 300;
			h.down_skill_index = 4;
			h.id_num1 = 101;
			h.id_num2 = 202;
			h.id_num3 = 303;
			h.sex = 1;
			h.skin = 2;
			h.hair_style = 3;
			h.hair_color = 5;
			h.underwear = 6;
			h.appr1 = 0x0356;
			h.appr2 = 0x0AB7;
			h.appr3 = 0x1234;
			h.appr4 = 0x0680;
			h.appr_color = 0x12345678;
			h.hunger_status = 88;
			h.time_left_shut_up = 7;
			h.time_left_rating = 8;
			h.time_left_force_recall = 9;
			h.time_left_firm_staminar = 10;
			h.admin_user_level = 0;
			h.penalty_block_year = 2025;
			h.penalty_block_month = 2;
			h.penalty_block_day = 3;
			h.quest = 11;
			h.quest_id = 12;
			h.current_quest_count = 13;
			h.quest_reward_type = 14;
			h.quest_reward_amount = 15;
			h.quest_completed = true;
			h.contribution = 16;
			h.war_contribution = 17;
			h.special_event_id = 18;
			h.super_attack_left = 19;
			h.fightzone_number = 20;
			h.reserve_time = 21;
			h.fightzone_ticket_number = 22;
			h.special_ability_time = 23;
			h.locked_map_name = "arejail";
			h.locked_map_time = 24;
			h.crusade_duty = 2;
			h.crusade_guid = 25;
			h.construction_point = 26;
			h.dead_penalty_time = 27;
			h.party_id = 28;
			h.gizon_item_upgrade_left = 29;

			for (int i = 0; i < 3; ++i)
			{
				character_item item;
				item.slot = i;
				item.name = std::format("Item{}", i);
				item.count = 1 + i;
				item.touch_effect_type = i;
				item.touch_effect_value1 = 10 + i;
				item.touch_effect_value2 = 20 + i;
				item.touch_effect_value3 = 30 + i;
				item.color = i;
				item.spec_effect_value1 = 40 + i;
				item.spec_effect_value2 = 50 + i;
				item.spec_effect_value3 = 60 + i;
				item.cur_life_span = 300 - i;
				item.attribute = 0x00010000 * (i + 1);
				item.equipped = i == 1;
				item.pos_x = 41 + i;
				item.pos_y = 31 + i;
				r.items.push_back(item);
			}
			character_item banked;
			banked.in_bank = true;
			banked.slot = 0;
			banked.name = "Gold";
			banked.count = 12345;
			banked.cur_life_span = 1;
			r.items.push_back(banked);

			r.skills.push_back(character_skill{ 0, 0, 2, 45, 120 });
			r.skills.push_back(character_skill{ 0, 0, 9, 0, 7 });
			r.magic.push_back(character_magic{ 0, 0, 0, 1 });
			r.magic.push_back(character_magic{ 0, 0, 31, 1 });
			return r;
		}

		// A test stand-in for the game's item config: any name is known and is equipment.
		CItem * make_item(const character_item& stored)
		{
			CItem * item = new CItem();
			std::strncpy(item->m_cName, stored.name.c_str(), sizeof(item->m_cName) - 1);
			item->m_cItemType = DEF_ITEMTYPE_EQUIP;
			apply_item_values(*item, stored);
			return item;
		}

		bool same_items(const character_record& a, const character_record& b)
		{
			if (a.items.size() != b.items.size())
				return false;
			for (std::size_t i = 0; i < a.items.size(); ++i)
			{
				const character_item& x = a.items[i];
				const character_item& y = b.items[i];
				if (x.in_bank != y.in_bank || x.slot != y.slot || x.name != y.name || x.count != y.count
					|| x.touch_effect_type != y.touch_effect_type || x.touch_effect_value1 != y.touch_effect_value1
					|| x.touch_effect_value2 != y.touch_effect_value2 || x.touch_effect_value3 != y.touch_effect_value3
					|| x.color != y.color || x.spec_effect_value1 != y.spec_effect_value1 || x.spec_effect_value2 != y.spec_effect_value2
					|| x.spec_effect_value3 != y.spec_effect_value3 || x.cur_life_span != y.cur_life_span || x.attribute != y.attribute
					|| x.equipped != y.equipped || (!x.in_bank && (x.pos_x != y.pos_x || x.pos_y != y.pos_y)))
					return false;
			}
			return true;
		}
	}

	void player_mapping_tests(runner& t)
	{
		const character_record original = sample_record();
		auto client = std::make_unique<CClient>(static_cast<HWND>(nullptr));
		apply_character(*client, original);
		std::string error;
		t.check(apply_items(*client, original, make_item, error), std::format("items placed ({})", error));
		t.check(client->m_cMapIndex == -1 && std::strcmp(client->m_cMapName, "aresden") == 0, "map name set, index left to the game");
		t.check(client->m_iAccountID == 5 && client->m_iCharacterDBID == 77, "account and character ids kept");
		t.check(client->m_pItemList[1] != nullptr && client->m_bIsItemEquipped[1] == TRUE && client->m_ItemPosList[1].x == 42,
			"bag item 1 placed, flagged worn, at its position");
		t.check(client->m_pItemInBankList[0] != nullptr && client->m_pItemInBankList[0]->m_dwCount == 12345, "bank gold placed");
		t.check(client->m_cSkillMastery[2] == 45 && client->m_iSkillSSN[9] == 7 && client->m_cMagicMastery[31] == 1, "mastery applied");

		character_record captured;
		capture_character(*client, captured);
		character expected = original.header;
		character got = captured.header;
		expected.created_at = got.created_at = {};
		expected.saved_at = got.saved_at = {};
		const auto same_header = [](const character& a, const character& b) {
			return a.id == b.id && a.account_id == b.account_id && a.name == b.name && a.name_key == b.name_key && a.profile == b.profile
				&& a.home_location == b.home_location && a.guild_name == b.guild_name && a.guild_guid == b.guild_guid
				&& a.guild_rank == b.guild_rank && a.map_name == b.map_name && a.x == b.x && a.y == b.y && a.hp == b.hp && a.mp == b.mp
				&& a.sp == b.sp && a.level == b.level && a.rating == b.rating && a.strength == b.strength
				&& a.intelligence == b.intelligence && a.vitality == b.vitality && a.dexterity == b.dexterity && a.magic == b.magic
				&& a.charisma == b.charisma && a.luck == b.luck && a.exp == b.exp && a.lu_strength == b.lu_strength
				&& a.lu_vitality == b.lu_vitality && a.lu_dexterity == b.lu_dexterity && a.lu_intelligence == b.lu_intelligence
				&& a.lu_magic == b.lu_magic && a.lu_charisma == b.lu_charisma && a.enemy_kill_count == b.enemy_kill_count
				&& a.pk_count == b.pk_count && a.reward_gold == b.reward_gold && a.down_skill_index == b.down_skill_index
				&& a.id_num1 == b.id_num1 && a.id_num2 == b.id_num2 && a.id_num3 == b.id_num3 && a.sex == b.sex && a.skin == b.skin
				&& a.hair_style == b.hair_style && a.hair_color == b.hair_color && a.underwear == b.underwear && a.appr1 == b.appr1
				&& a.appr2 == b.appr2 && a.appr3 == b.appr3 && a.appr4 == b.appr4 && a.appr_color == b.appr_color
				&& a.hunger_status == b.hunger_status && a.time_left_shut_up == b.time_left_shut_up
				&& a.time_left_rating == b.time_left_rating && a.time_left_force_recall == b.time_left_force_recall
				&& a.time_left_firm_staminar == b.time_left_firm_staminar && a.admin_user_level == b.admin_user_level
				&& a.penalty_block_year == b.penalty_block_year && a.penalty_block_month == b.penalty_block_month
				&& a.penalty_block_day == b.penalty_block_day && a.quest == b.quest && a.quest_id == b.quest_id
				&& a.current_quest_count == b.current_quest_count && a.quest_reward_type == b.quest_reward_type
				&& a.quest_reward_amount == b.quest_reward_amount && a.quest_completed == b.quest_completed
				&& a.contribution == b.contribution && a.war_contribution == b.war_contribution && a.special_event_id == b.special_event_id
				&& a.super_attack_left == b.super_attack_left && a.fightzone_number == b.fightzone_number
				&& a.reserve_time == b.reserve_time && a.fightzone_ticket_number == b.fightzone_ticket_number
				&& a.special_ability_time == b.special_ability_time && a.locked_map_name == b.locked_map_name
				&& a.locked_map_time == b.locked_map_time && a.crusade_duty == b.crusade_duty && a.crusade_guid == b.crusade_guid
				&& a.construction_point == b.construction_point && a.dead_penalty_time == b.dead_penalty_time
				&& a.party_id == b.party_id && a.gizon_item_upgrade_left == b.gizon_item_upgrade_left;
		};
		t.check(same_header(expected, got), "header round-trips record -> CClient -> record");
		t.check(same_items(original, captured), "bag and bank items round-trip");
		t.check(captured.skills.size() == 2 && captured.skills[0].skill_id == 2 && captured.skills[0].mastery == 45
			&& captured.skills[0].ssn == 120 && captured.skills[1].skill_id == 9 && captured.skills[1].ssn == 7, "skills round-trip");
		t.check(captured.magic.size() == 2 && captured.magic[0].magic_id == 0 && captured.magic[1].magic_id == 31, "magic round-trips");

		// The 2.20 writer's normalisations.
		client->m_iGuildRank = -1;
		client->m_iSP = -5;
		client->m_iContribution = -3;
		capture_character(*client, captured);
		t.check(captured.header.guild_name == "NONE" && captured.header.guild_guid == -1, "no guild saves as NONE / -1");
		t.check(captured.header.sp == 0 && captured.header.contribution == 0, "negative SP and contribution save as 0");

		// Unknown item: the load stops and names it.
		character_record unknown = original;
		auto other = std::make_unique<CClient>(static_cast<HWND>(nullptr));
		apply_character(*other, unknown);
		t.check(!apply_items(*other, unknown, [](const character_item&) -> CItem * { return nullptr; }, error) && error.find("Item0") != std::string::npos,
			"an unknown item stops the load");
	}
}
