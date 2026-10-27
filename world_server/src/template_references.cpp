#include "template_references.h"

#include "world_database.h"

#include <format>
#include <vector>

namespace hb::server::world
{
	namespace
	{
		std::string list_of(const std::vector<std::int64_t>& ids)
		{
			std::string text;
			for (const std::int64_t id : ids)
				text += text.empty() ? std::format("{}", id) : std::format(", {}", id);
			return text;
		}
	}

	bool template_references::resolve_skills(std::span<const std::int64_t> skill_ids, std::string& error) const
	{
		std::vector<std::int64_t> missing;
		if (!m_templates.missing_skills(skill_ids, missing, error))
		{
			error = std::format("cannot check skills against world.db: {}", error);
			return false;
		}
		if (!missing.empty())
		{
			error = std::format("skill id(s) {} not in world.db", list_of(missing));
			return false;
		}
		return true;
	}
}
