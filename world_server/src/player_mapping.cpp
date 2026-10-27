// players.db <-> CClient field mapping (WORLD-005). Built with the game's legacy settings because it includes Client.h
// (windows.h); NOMINMAX is set for this file so Schemata's headers keep std::min and std::max.
#include "player_mapping.h"

#include "Client.h"
#include "Item.h"

#include <algorithm>
#include <cstring>
#include <format>
#include <string>

namespace hb::server::world
{
	namespace
	{
		template <std::size_t N>
		void copy_text(char (&target)[N], const std::string& value, const char* when_empty = nullptr)
		{
			std::memset(target, 0, N);
			const std::string& text = (value.empty() && when_empty != nullptr) ? std::string(when_empty) : value;
			std::memcpy(target, text.data(), (std::min)(text.size(), N - 1));
		}

		template <std::size_t N>
		std::string text_of(const char (&source)[N])
		{
			return std::string(source, strnlen(source, N));
		}

		character_item capture_item(const CItem& item, bool in_bank, std::int32_t slot)
		{
			character_item row;
			row.in_bank = in_bank;
			row.slot = slot;
			row.name = text_of(item.m_cName);
			row.count = static_cast<std::int64_t>(item.m_dwCount);
			row.touch_effect_type = item.m_sTouchEffectType;
			row.touch_effect_value1 = item.m_sTouchEffectValue1;
			row.touch_effect_value2 = item.m_sTouchEffectValue2;
			row.touch_effect_value3 = item.m_sTouchEffectValue3;
			row.color = item.m_cItemColor;
			row.spec_effect_value1 = item.m_sItemSpecEffectValue1;
			row.spec_effect_value2 = item.m_sItemSpecEffectValue2;
			row.spec_effect_value3 = item.m_sItemSpecEffectValue3;
			row.cur_life_span = item.m_wCurLifeSpan;
			row.attribute = static_cast<std::int64_t>(item.m_dwAttribute);
			return row;
		}
	}

	void apply_character(CClient& c, const character_record& record)
	{
		const character& h = record.header;
		c.m_iCharacterDBID = h.id;
		c.m_iAccountID = h.account_id;
		copy_text(c.m_cCharName, h.name);
		copy_text(c.m_cProfile, h.profile, "__________");
		copy_text(c.m_cLocation, h.home_location, "NONE");
		copy_text(c.m_cGuildName, h.guild_name, "NONE");
		c.m_iGuildGUID = h.guild_guid;
		c.m_iGuildRank = h.guild_rank;
		copy_text(c.m_cMapName, h.map_name);
		c.m_cMapIndex = -1;
		c.m_sX = static_cast<short>(h.x);
		c.m_sY = static_cast<short>(h.y);
		c.m_iHP = h.hp;
		c.m_iMP = h.mp;
		c.m_iSP = h.sp;
		c.m_iLevel = h.level;
		c.m_iRating = h.rating;
		c.m_iStr = h.strength;
		c.m_iInt = h.intelligence;
		c.m_iVit = h.vitality;
		c.m_iDex = h.dexterity;
		c.m_iMag = h.magic;
		c.m_iCharisma = h.charisma;
		c.m_iLuck = h.luck;
		c.m_iExp = h.exp;
		c.m_cLU_Str = static_cast<char>(h.lu_strength);
		c.m_cLU_Vit = static_cast<char>(h.lu_vitality);
		c.m_cLU_Dex = static_cast<char>(h.lu_dexterity);
		c.m_cLU_Int = static_cast<char>(h.lu_intelligence);
		c.m_cLU_Mag = static_cast<char>(h.lu_magic);
		c.m_cLU_Char = static_cast<char>(h.lu_charisma);
		c.m_iEnemyKillCount = h.enemy_kill_count;
		c.m_iPKCount = h.pk_count;
		c.m_iRewardGold = h.reward_gold;
		c.m_iDownSkillIndex = h.down_skill_index;
		c.m_sCharIDnum1 = static_cast<short>(h.id_num1);
		c.m_sCharIDnum2 = static_cast<short>(h.id_num2);
		c.m_sCharIDnum3 = static_cast<short>(h.id_num3);
		c.m_cSex = static_cast<char>(h.sex);
		c.m_cSkin = static_cast<char>(h.skin);
		c.m_cHairStyle = static_cast<char>(h.hair_style);
		c.m_cHairColor = static_cast<char>(h.hair_color);
		c.m_cUnderwear = static_cast<char>(h.underwear);
		c.m_sAppr1 = static_cast<short>(h.appr1);
		c.m_sAppr2 = static_cast<short>(h.appr2);
		c.m_sAppr3 = static_cast<short>(h.appr3);
		c.m_sAppr4 = static_cast<short>(h.appr4);
		c.m_iApprColor = h.appr_color;
		c.m_iHungerStatus = h.hunger_status;
		c.m_iTimeLeft_ShutUp = h.time_left_shut_up;
		c.m_iTimeLeft_Rating = h.time_left_rating;
		c.m_iTimeLeft_ForceRecall = h.time_left_force_recall;
		c.m_iTimeLeft_FirmStaminar = h.time_left_firm_staminar;
		c.m_iAdminUserLevel = h.admin_user_level;
		c.m_iPenaltyBlockYear = h.penalty_block_year;
		c.m_iPenaltyBlockMonth = h.penalty_block_month;
		c.m_iPenaltyBlockDay = h.penalty_block_day;
		c.m_iQuest = h.quest;
		c.m_iQuestID = h.quest_id;
		c.m_iCurQuestCount = h.current_quest_count;
		c.m_iQuestRewardType = h.quest_reward_type;
		c.m_iQuestRewardAmount = h.quest_reward_amount;
		c.m_bIsQuestCompleted = h.quest_completed ? TRUE : FALSE;
		c.m_iContribution = h.contribution;
		c.m_iWarContribution = h.war_contribution;
		c.m_iSpecialEventID = h.special_event_id;
		c.m_iSuperAttackLeft = h.super_attack_left;
		c.m_iFightzoneNumber = h.fightzone_number;
		c.m_iReserveTime = h.reserve_time;
		c.m_iFightZoneTicketNumber = h.fightzone_ticket_number;
		c.m_iSpecialAbilityTime = h.special_ability_time;
		copy_text(c.m_cLockedMapName, h.locked_map_name, "NONE");
		c.m_iLockedMapTime = h.locked_map_time;
		c.m_iCrusadeDuty = h.crusade_duty;
		c.m_dwCrusadeGUID = static_cast<DWORD>(h.crusade_guid);
		c.m_iConstructionPoint = h.construction_point;
		c.m_iDeadPenaltyTime = h.dead_penalty_time;
		c.m_iPartyID = h.party_id;
		c.m_iGizonItemUpgradeLeft = h.gizon_item_upgrade_left;

		for (int i = 0; i < DEF_MAXMAGICTYPE; i++)
			c.m_cMagicMastery[i] = 0;
		for (int i = 0; i < DEF_MAXSKILLTYPE; i++)
		{
			c.m_cSkillMastery[i] = 0;
			c.m_iSkillSSN[i] = 0;
		}
		for (const character_magic& m : record.magic)
		{
			if (m.magic_id >= 0 && m.magic_id < DEF_MAXMAGICTYPE)
				c.m_cMagicMastery[m.magic_id] = static_cast<char>(m.mastery);
		}
		for (const character_skill& s : record.skills)
		{
			if (s.skill_id >= 0 && s.skill_id < DEF_MAXSKILLTYPE)
			{
				c.m_cSkillMastery[s.skill_id] = static_cast<unsigned char>(s.mastery);
				c.m_iSkillSSN[s.skill_id] = s.ssn;
			}
		}
	}

	void apply_item_values(CItem& item, const character_item& stored)
	{
		// A negative count is set to 1, as the file loader did.
		item.m_dwCount = static_cast<DWORD>(stored.count < 0 ? 1 : stored.count);
		item.m_sTouchEffectType = static_cast<short>(stored.touch_effect_type);
		item.m_sTouchEffectValue1 = static_cast<short>(stored.touch_effect_value1);
		item.m_sTouchEffectValue2 = static_cast<short>(stored.touch_effect_value2);
		item.m_sTouchEffectValue3 = static_cast<short>(stored.touch_effect_value3);
		item.m_cItemColor = static_cast<char>(stored.color);
		item.m_sItemSpecEffectValue1 = static_cast<short>(stored.spec_effect_value1);
		item.m_sItemSpecEffectValue2 = static_cast<short>(stored.spec_effect_value2);
		item.m_sItemSpecEffectValue3 = static_cast<short>(stored.spec_effect_value3);
		item.m_wCurLifeSpan = stored.cur_life_span;
		item.m_dwAttribute = static_cast<DWORD>(stored.attribute);
	}

	bool apply_items(CClient& c, const character_record& record, const item_factory& make, std::string& error)
	{
		error.clear();
		std::vector<const character_item*> bag;
		std::vector<const character_item*> bank;
		for (const character_item& row : record.items)
			(row.in_bank ? bank : bag).push_back(&row);
		const auto by_slot = [](const character_item* a, const character_item* b) { return a->slot < b->slot; };
		std::ranges::stable_sort(bag, by_slot);
		std::ranges::stable_sort(bank, by_slot);
		if (bag.size() > DEF_MAXITEMS || bank.size() > DEF_MAXBANKITEMS)
		{
			error = std::format("{} bag and {} bank items, over the limits {} and {}", bag.size(), bank.size(), DEF_MAXITEMS, DEF_MAXBANKITEMS);
			return false;
		}
		for (std::size_t i = 0; i < bag.size(); ++i)
		{
			CItem* item = make(*bag[i]);
			if (item == nullptr)
			{
				error = std::format("unknown item {}", bag[i]->name);
				return false;
			}
			c.m_pItemList[i] = item;
			c.m_ItemPosList[i].x = bag[i]->pos_x;
			c.m_ItemPosList[i].y = bag[i]->pos_y;
			c.m_bIsItemEquipped[i] = bag[i]->equipped ? TRUE : FALSE;
		}
		for (std::size_t i = 0; i < bank.size(); ++i)
		{
			CItem* item = make(*bank[i]);
			if (item == nullptr)
			{
				error = std::format("unknown bank item {}", bank[i]->name);
				return false;
			}
			c.m_pItemInBankList[i] = item;
		}
		return true;
	}

	void capture_character(const CClient& c, character_record& record)
	{
		record = character_record{};
		character& h = record.header;
		h.id = c.m_iCharacterDBID;
		h.account_id = c.m_iAccountID;
		h.name = text_of(c.m_cCharName);
		h.name_key = character_store::name_key(h.name);
		h.profile = text_of(c.m_cProfile);
		h.home_location = text_of(c.m_cLocation);
		if (c.m_iGuildRank != -1)
		{
			h.guild_name = text_of(c.m_cGuildName);
			h.guild_guid = c.m_iGuildGUID;
		}
		else
		{
			h.guild_name = "NONE";
			h.guild_guid = -1;
		}
		h.guild_rank = c.m_iGuildRank;
		h.map_name = text_of(c.m_cMapName);
		h.x = c.m_sX;
		h.y = c.m_sY;
		h.hp = c.m_iHP;
		h.mp = c.m_iMP;
		h.sp = (std::max)(c.m_iSP, 0);
		h.level = c.m_iLevel;
		h.rating = c.m_iRating;
		h.strength = c.m_iStr;
		h.intelligence = c.m_iInt;
		h.vitality = c.m_iVit;
		h.dexterity = c.m_iDex;
		h.magic = c.m_iMag;
		h.charisma = c.m_iCharisma;
		h.luck = c.m_iLuck;
		h.exp = c.m_iExp;
		h.lu_strength = c.m_cLU_Str;
		h.lu_vitality = c.m_cLU_Vit;
		h.lu_dexterity = c.m_cLU_Dex;
		h.lu_intelligence = c.m_cLU_Int;
		h.lu_magic = c.m_cLU_Mag;
		h.lu_charisma = c.m_cLU_Char;
		h.enemy_kill_count = c.m_iEnemyKillCount;
		h.pk_count = c.m_iPKCount;
		h.reward_gold = c.m_iRewardGold;
		h.down_skill_index = c.m_iDownSkillIndex;
		h.id_num1 = c.m_sCharIDnum1;
		h.id_num2 = c.m_sCharIDnum2;
		h.id_num3 = c.m_sCharIDnum3;
		h.sex = c.m_cSex;
		h.skin = c.m_cSkin;
		h.hair_style = c.m_cHairStyle;
		h.hair_color = c.m_cHairColor;
		h.underwear = c.m_cUnderwear;
		h.appr1 = c.m_sAppr1;
		h.appr2 = c.m_sAppr2;
		h.appr3 = c.m_sAppr3;
		h.appr4 = c.m_sAppr4;
		h.appr_color = c.m_iApprColor;
		h.hunger_status = c.m_iHungerStatus;
		h.time_left_shut_up = c.m_iTimeLeft_ShutUp;
		h.time_left_rating = c.m_iTimeLeft_Rating;
		h.time_left_force_recall = c.m_iTimeLeft_ForceRecall;
		h.time_left_firm_staminar = c.m_iTimeLeft_FirmStaminar;
		h.admin_user_level = c.m_iAdminUserLevel;
		h.penalty_block_year = c.m_iPenaltyBlockYear;
		h.penalty_block_month = c.m_iPenaltyBlockMonth;
		h.penalty_block_day = c.m_iPenaltyBlockDay;
		h.quest = c.m_iQuest;
		h.quest_id = c.m_iQuestID;
		h.current_quest_count = c.m_iCurQuestCount;
		h.quest_reward_type = c.m_iQuestRewardType;
		h.quest_reward_amount = c.m_iQuestRewardAmount;
		h.quest_completed = c.m_bIsQuestCompleted != FALSE;
		h.contribution = (std::max)(c.m_iContribution, 0);
		h.war_contribution = c.m_iWarContribution;
		h.special_event_id = c.m_iSpecialEventID;
		h.super_attack_left = c.m_iSuperAttackLeft;
		h.fightzone_number = c.m_iFightzoneNumber;
		h.reserve_time = c.m_iReserveTime;
		h.fightzone_ticket_number = c.m_iFightZoneTicketNumber;
		h.special_ability_time = c.m_iSpecialAbilityTime;
		h.locked_map_name = text_of(c.m_cLockedMapName);
		h.locked_map_time = c.m_iLockedMapTime;
		h.crusade_duty = c.m_iCrusadeDuty;
		h.crusade_guid = static_cast<std::int64_t>(c.m_dwCrusadeGUID);
		h.construction_point = c.m_iConstructionPoint;
		h.dead_penalty_time = c.m_iDeadPenaltyTime;
		h.party_id = c.m_iPartyID;
		h.gizon_item_upgrade_left = c.m_iGizonItemUpgradeLeft;

		std::int32_t slot = 0;
		for (int i = 0; i < DEF_MAXITEMS; i++)
		{
			if (c.m_pItemList[i] == nullptr)
				continue;
			character_item row = capture_item(*c.m_pItemList[i], false, slot++);
			row.equipped = (c.m_bIsItemEquipped[i] == TRUE) && (c.m_pItemList[i]->m_cItemType == DEF_ITEMTYPE_EQUIP);
			row.pos_x = c.m_ItemPosList[i].x;
			row.pos_y = c.m_ItemPosList[i].y;
			record.items.push_back(std::move(row));
		}
		slot = 0;
		for (int i = 0; i < DEF_MAXBANKITEMS; i++)
		{
			if (c.m_pItemInBankList[i] != nullptr)
				record.items.push_back(capture_item(*c.m_pItemInBankList[i], true, slot++));
		}
		for (int i = 0; i < DEF_MAXSKILLTYPE; i++)
		{
			if (c.m_cSkillMastery[i] != 0 || c.m_iSkillSSN[i] != 0)
			{
				character_skill s;
				s.skill_id = i;
				s.mastery = c.m_cSkillMastery[i];
				s.ssn = c.m_iSkillSSN[i];
				record.skills.push_back(s);
			}
		}
		for (int i = 0; i < DEF_MAXMAGICTYPE; i++)
		{
			if (c.m_cMagicMastery[i] != 0)
			{
				character_magic m;
				m.magic_id = i;
				m.mastery = c.m_cMagicMastery[i];
				record.magic.push_back(m);
			}
		}
	}
}
