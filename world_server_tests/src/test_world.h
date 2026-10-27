#pragma once

#include "world_database.h"

#include <cstdint>
#include <filesystem>
#include <format>
#include <initializer_list>
#include <string>
#include <vector>

// Throwaway world.db templates for tests that need keys to check against: skills named "skill<id>".
namespace tests
{
	// Replaces the skill table of an open world.db with the given ids.
	inline bool fill_world_skills(std::initializer_list<std::int64_t> skill_ids, hb::server::world::world_database& db, std::string& error)
	{
		std::vector<hb::server::world::skill> skills;
		for (const std::int64_t id : skill_ids)
		{
			hb::server::world::skill row;
			row.id = id;
			row.name = std::format("skill{}", id);
			skills.push_back(row);
		}
		return db.write_skills(skills, true, error);
	}

	// Opens (creating) a world.db at path and fills its skills.
	inline bool make_world_db(const std::filesystem::path& path, std::initializer_list<std::int64_t> skill_ids,
		hb::server::world::world_database& db, std::string& error)
	{
		return db.open(path.string(), 1, error) && fill_world_skills(skill_ids, db, error);
	}
}
