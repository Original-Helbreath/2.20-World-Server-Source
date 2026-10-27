#include "skill_legacy.h"

#include "Skill.h"
#include "StrTok.h"
#include "text_codec.h"

#include <cstdlib>
#include <cstring>
#include <format>
#include <string_view>

namespace hb::server::world::skill_legacy
{
	namespace
	{
		// CGame::_bGetIsStringIsNumber: digits and '-' only (an empty token passes, as it did).
		bool is_number(const char* token)
		{
			for (const char* p = token; *p != '\0'; ++p)
				if (*p != '-' && (*p < '0' || *p > '9'))
					return false;
			return true;
		}

		short* value_slot(CSkill& skill, int field)
		{
			switch (field)
			{
			case 3: return &skill.m_sType;
			case 4: return &skill.m_sValue1;
			case 5: return &skill.m_sValue2;
			case 6: return &skill.m_sValue3;
			case 7: return &skill.m_sValue4;
			case 8: return &skill.m_sValue5;
			case 9: return &skill.m_sValue6;
			default: return nullptr;
			}
		}
	}

	bool parse_skill_file(const std::vector<char>& bytes, skill_table& skills, std::string& error)
	{
		error.clear();
		for (auto& slot : skills)
			slot.reset();

		// The decoder worked on a zero-terminated copy.
		std::vector<char> contents(bytes);
		contents.push_back('\0');
		char separators[] = "= \t\n";
		CStrTok tokens(contents.data(), separators);

		int field = 0;  // cReadModeB: 0 outside a line, 1 = number, 2 = name, 3..9 = type and values
		int index = 0;
		for (char* token = tokens.pGet(); token != nullptr; token = tokens.pGet())
		{
			if (field == 0)
			{
				if (std::memcmp(token, "skill", 5) == 0)
					field = 1;
				continue;
			}
			if (field == 1)
			{
				if (!is_number(token))
				{
					error = std::format("skill number '{}' is not a number", token);
					return false;
				}
				index = std::atoi(token);
				if (index < 0 || index >= static_cast<int>(max_skills))
				{
					error = std::format("skill number {} is outside 0..{}", index, max_skills - 1);
					return false;
				}
				if (skills[static_cast<std::size_t>(index)])
				{
					error = std::format("skill number {} is defined twice", index);
					return false;
				}
				skills[static_cast<std::size_t>(index)] = std::make_unique<CSkill>();
				field = 2;
				continue;
			}
			CSkill& skill = *skills[static_cast<std::size_t>(index)];
			if (field == 2)
			{
				const std::size_t length = std::strlen(token);
				if (length > max_skill_name_bytes)
				{
					error = std::format("skill {}: name is {} bytes, the game holds {}", index, length, max_skill_name_bytes);
					return false;
				}
				std::memset(skill.m_cName, 0, sizeof(skill.m_cName));
				std::memcpy(skill.m_cName, token, length);
				field = 3;
				continue;
			}
			if (!is_number(token))
			{
				error = std::format("skill {}: field {} '{}' is not a number", index, field, token);
				return false;
			}
			*value_slot(skill, field) = static_cast<short>(std::atoi(token));
			field = (field == 9) ? 0 : field + 1;
		}
		if (field != 0)
		{
			error = "the last skill line is incomplete";
			return false;
		}
		return true;
	}

	bool to_legacy(const skill& row, CSkill& out, std::string& error)
	{
		error.clear();
		std::string name;
		if (!text::utf8_to_cp949(row.name, name))
		{
			error = std::format("skill {}: name is not convertible to CP949", row.id);
			return false;
		}
		if (name.size() > max_skill_name_bytes)
		{
			error = std::format("skill {}: name is {} bytes, the game holds {}", row.id, name.size(), max_skill_name_bytes);
			return false;
		}
		std::memset(out.m_cName, 0, sizeof(out.m_cName));
		std::memcpy(out.m_cName, name.data(), name.size());
		out.m_sType = static_cast<short>(row.type);
		out.m_sValue1 = static_cast<short>(row.value1);
		out.m_sValue2 = static_cast<short>(row.value2);
		out.m_sValue3 = static_cast<short>(row.value3);
		out.m_sValue4 = static_cast<short>(row.value4);
		out.m_sValue5 = static_cast<short>(row.value5);
		out.m_sValue6 = static_cast<short>(row.value6);
		return true;
	}

	bool from_legacy(std::int64_t id, const CSkill& in, skill& out, std::string& error)
	{
		error.clear();
		out = skill{};
		out.id = id;
		if (!text::cp949_to_utf8(std::string_view(in.m_cName, ::strnlen(in.m_cName, sizeof(in.m_cName))), out.name))
		{
			error = std::format("skill {}: name is not valid CP949", id);
			return false;
		}
		out.type = in.m_sType;
		out.value1 = in.m_sValue1;
		out.value2 = in.m_sValue2;
		out.value3 = in.m_sValue3;
		out.value4 = in.m_sValue4;
		out.value5 = in.m_sValue5;
		out.value6 = in.m_sValue6;
		return true;
	}

	bool same(const CSkill& a, const CSkill& b)
	{
		return std::memcmp(a.m_cName, b.m_cName, sizeof(a.m_cName)) == 0 && a.m_sType == b.m_sType && a.m_sValue1 == b.m_sValue1
			&& a.m_sValue2 == b.m_sValue2 && a.m_sValue3 == b.m_sValue3 && a.m_sValue4 == b.m_sValue4 && a.m_sValue5 == b.m_sValue5
			&& a.m_sValue6 == b.m_sValue6;
	}
}
