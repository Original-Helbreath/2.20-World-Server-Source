#pragma once

#include "world_templates.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

class CSkill;

// Skill templates between world.db rows and the game's CSkill (WORLD-008 slice 1). The World loads CSkill from rows
// with to_legacy; world_import reads GameData\Skill.cfg with parse_skill_file and stores rows from from_legacy.
namespace hb::server::world::skill_legacy
{
	// m_pSkillConfigList[DEF_MAXSKILLTYPE] (Client.h).
	constexpr std::size_t max_skills = 60;

	using skill_table = std::array<std::unique_ptr<CSkill>, max_skills>;

	// The legacy decoder (CGame::_bDecodeSkillConfigFileContents) restated: the same tokeniser (CStrTok, "= \t\n"),
	// the same `skill` keyword match and field order, the same number check, atoi into shorts. It also refuses what
	// the decoder would have overrun: an id outside 0..59 or a name over 20 bytes. False with the reason.
	bool parse_skill_file(const std::vector<char>& bytes, skill_table& skills, std::string& error);

	// A row to the game's object: the name back to CP949. False when it does not convert or does not fit.
	bool to_legacy(const skill& row, CSkill& out, std::string& error);

	// The game's object to a row (name to UTF-8). False when the name is not valid CP949.
	bool from_legacy(std::int64_t id, const CSkill& in, skill& out, std::string& error);

	// Every field the game reads, the name compared over the whole buffer.
	bool same(const CSkill& a, const CSkill& b);
}
