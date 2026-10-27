// WORLD-003: the game data the log server used to push to the game server when it registered (Npc, Item, Magic,
// Skill, Quest, BuildItem, DupItemID, Potion and WLserver) is read from GameData\ here, until world.db holds it.
// Every file goes through the decoder the push used, so the game logic is unchanged.
// WORLD-008: world.db replaces these files one kind at a time; skills already come from it (slice 1).

#include "Game.h"

#include "log.h"
#include "Skill.h"
#include "skill_legacy.h"
#include "world_data.h"

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace
{
	// False when the file is missing or empty.
	bool read_file(const char * name, std::vector<char>& bytes)
	{
		std::ifstream file(name, std::ios::binary);
		if (!file) return false;
		bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		return !bytes.empty();
	}
}

BOOL CGame::bLoadGameDataFiles()
{
	struct entry
	{
		const char * pFile;
		BOOL (CGame::*pDecode)(char *, DWORD);
		BOOL * pAvailable;  // the flag the decoded data raises; null when the game does not wait for it
		const char * pLabel;
	};

	// The order the log server pushed them in. Item data comes in three files; the flag stays up only if all load.
	const entry list[] = {
		{ "GameData\\Npc.cfg",       &CGame::_bDecodeNpcConfigFileContents,       &m_bIsNpcAvailable,       "Npc" },
		{ "GameData\\Item.cfg",      &CGame::_bDecodeItemConfigFileContents,      &m_bIsItemAvailable,      "Item" },
		{ "GameData\\Item2.cfg",     &CGame::_bDecodeItemConfigFileContents,      &m_bIsItemAvailable,      "Item" },
		{ "GameData\\Magic.cfg",     &CGame::_bDecodeMagicConfigFileContents,     &m_bIsMagicAvailable,     "Magic" },
		{ "GameData\\Item3.cfg",     &CGame::_bDecodeItemConfigFileContents,      &m_bIsItemAvailable,      "Item" },
		{ "GameData\\Quest.cfg",     &CGame::_bDecodeQuestConfigFileContents,     &m_bIsQuestAvailable,     "Quest" },
		{ "GameData\\BuildItem.cfg", &CGame::_bDecodeBuildItemConfigFileContents, &m_bIsBuildItemAvailable, "BuildItem" },
		{ "GameData\\DupItemID.cfg", &CGame::_bDecodeDupItemIDFileContents,       nullptr,                  "DupItemID" },
		{ "GameData\\Potion.cfg",    &CGame::_bDecodePortionConfigFileContents,   &m_bIsPortionAvailable,   "Potion" },
#ifdef DEF_DEFENCEHACKING
		{ "GameData\\WLserver.cfg",  &CGame::_bDecodeWLServerConfigFileContents,  &m_bIsWLServerAvailable,  "WLserver" },
#endif
	};

	for (const entry & item : list)
		if (item.pAvailable != nullptr) *item.pAvailable = TRUE;

	BOOL bAllLoaded = TRUE;
	for (const entry & item : list) {
		std::vector<char> bytes;
		BOOL bLoaded = FALSE;
		if (read_file(item.pFile, bytes)) {
			bLoaded = (this->*item.pDecode)(bytes.data(), static_cast<DWORD>(bytes.size()));
		}
		else hb::log::error("cannot read {}", item.pFile);

		if (bLoaded == FALSE) {
			if (item.pAvailable != nullptr) *item.pAvailable = FALSE;
			bAllLoaded = FALSE;
		}
	}

	// The old "STOPPED! ... configuration error" lines, once per kind of data.
	for (const entry & item : list) {
		if ((item.pAvailable == nullptr) || (*item.pAvailable == TRUE)) continue;
		if (&item != &list[0] && (&item - 1)->pAvailable == item.pAvailable) continue;
		hb::log::error("{} configuration failed to load; the game will not activate", item.pLabel);
	}

	// WORLD-008 slice 1: skills come from world.db (opened by the host before bInit).
	if (bLoadSkillsFromWorldData() == FALSE) bAllLoaded = FALSE;

	return bAllLoaded;
}

BOOL CGame::bLoadSkillsFromWorldData()
{
	m_bIsSkillAvailable = FALSE;
	for (int i = 0; i < DEF_MAXSKILLTYPE; i++) {
		if (m_pSkillConfigList[i] != NULL) delete m_pSkillConfigList[i];
		m_pSkillConfigList[i] = NULL;
	}

	const std::vector<hb::server::world::skill>& skills = hb::server::world::world_skills();
	for (const hb::server::world::skill& row : skills) {
		std::string error;
		if ((row.id < 0) || (row.id >= DEF_MAXSKILLTYPE) || (m_pSkillConfigList[row.id] != NULL)) {
			hb::log::error("skill {} is outside 0..{} or repeated", row.id, DEF_MAXSKILLTYPE - 1);
			return FALSE;
		}
		CSkill * pSkill = new CSkill;
		if (!hb::server::world::skill_legacy::to_legacy(row, *pSkill, error)) {
			delete pSkill;
			hb::log::error("{}", error);
			return FALSE;
		}
		m_pSkillConfigList[row.id] = pSkill;
	}
	if (skills.empty()) {
		hb::log::error("Skill configuration failed to load; the game will not activate");
		return FALSE;
	}
	hb::log::info("{} skill(s) loaded", skills.size());
	m_bIsSkillAvailable = TRUE;
	return TRUE;
}
