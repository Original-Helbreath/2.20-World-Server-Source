// world_import (WORLD-008): builds world.db from the legacy World files, and compares the two.
//
//   world_import --game-data <GameData folder> --database <world.db> [--replace]
//       Reads the legacy files and writes their tables, one transaction per table. A table that already holds rows
//       is refused unless --replace is given.
//   world_import --compare --game-data <GameData folder> --database <world.db>
//       Loads every template both ways - the legacy file through the legacy decoding rules, and world.db through the
//       World's own row -> game-object mapping - and compares the game objects field by field. Exit 1 on a difference.
//
// Slice 1 covers skills (GameData\Skill.cfg). Design: .claude/design/plans/world-db.md.

#include "Skill.h"
#include "log.h"
#include "skill_legacy.h"
#include "world_database.h"

#include <cstddef>
#include <cstring>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace
{
	namespace world = hb::server::world;

	struct options
	{
		std::filesystem::path game_data;
		std::string database;
		bool replace = false;
		bool compare = false;
	};

	bool parse_arguments(int argc, char** argv, options& parsed)
	{
		for (int i = 1; i < argc; ++i)
		{
			const std::string_view argument = argv[i];
			if (argument == "--replace")
				parsed.replace = true;
			else if (argument == "--compare")
				parsed.compare = true;
			else if (argument == "--game-data" && i + 1 < argc)
				parsed.game_data = argv[++i];
			else if (argument == "--database" && i + 1 < argc)
				parsed.database = argv[++i];
			else
			{
				hb::log::error("unknown argument '{}'", argument);
				return false;
			}
		}
		if (parsed.game_data.empty() || parsed.database.empty())
		{
			hb::log::error("usage: world_import [--compare] [--replace] --game-data <folder> --database <world.db>");
			return false;
		}
		if (parsed.compare && parsed.replace)
		{
			hb::log::error("--compare reads only; it does not take --replace");
			return false;
		}
		return true;
	}

	bool read_file(const std::filesystem::path& path, std::vector<char>& bytes)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			hb::log::error("cannot read {}", path.string());
			return false;
		}
		bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		return true;
	}

	bool read_legacy_skills(const options& parsed, world::skill_legacy::skill_table& table)
	{
		const std::filesystem::path path = parsed.game_data / "Skill.cfg";
		std::vector<char> bytes;
		if (!read_file(path, bytes))
			return false;
		std::string error;
		if (!world::skill_legacy::parse_skill_file(bytes, table, error))
		{
			hb::log::error("{}: {}", path.string(), error);
			return false;
		}
		return true;
	}

	bool import_skills(const options& parsed, world::world_database& db)
	{
		world::skill_legacy::skill_table table;
		if (!read_legacy_skills(parsed, table))
			return false;
		std::vector<world::skill> rows;
		std::string error;
		for (std::size_t id = 0; id < table.size(); ++id)
		{
			if (!table[id])
				continue;
			world::skill row;
			if (!world::skill_legacy::from_legacy(static_cast<std::int64_t>(id), *table[id], row, error))
			{
				hb::log::error("{}", error);
				return false;
			}
			rows.push_back(std::move(row));
		}
		if (!db.write_skills(rows, parsed.replace, error))
		{
			hb::log::error("skill: {}", error);
			return false;
		}
		hb::log::info("skill: {} row(s) written", rows.size());
		return true;
	}

	bool compare_skills(const options& parsed, world::world_database& db)
	{
		world::skill_legacy::skill_table legacy;
		if (!read_legacy_skills(parsed, legacy))
			return false;
		std::vector<world::skill> rows;
		std::string error;
		if (!db.load_skills(rows, error))
		{
			hb::log::error("skill: {}", error);
			return false;
		}
		// The World's path: rows -> CSkill, exactly as CGame::bLoadSkillsFromWorldData fills m_pSkillConfigList.
		world::skill_legacy::skill_table loaded;
		for (const world::skill& row : rows)
		{
			if (row.id < 0 || row.id >= static_cast<std::int64_t>(loaded.size()))
			{
				hb::log::error("skill {}: id outside 0..{}", row.id, loaded.size() - 1);
				return false;
			}
			auto object = std::make_unique<CSkill>();
			if (!world::skill_legacy::to_legacy(row, *object, error))
			{
				hb::log::error("{}", error);
				return false;
			}
			loaded[static_cast<std::size_t>(row.id)] = std::move(object);
		}

		std::size_t compared = 0;
		std::size_t differ = 0;
		for (std::size_t id = 0; id < legacy.size(); ++id)
		{
			if (!legacy[id] && !loaded[id])
				continue;
			++compared;
			if (!legacy[id] || !loaded[id])
			{
				++differ;
				hb::log::error("skill {}: only in {}", id, legacy[id] ? "Skill.cfg" : "world.db");
				continue;
			}
			if (!world::skill_legacy::same(*legacy[id], *loaded[id]))
			{
				++differ;
				const CSkill& a = *legacy[id];
				const CSkill& b = *loaded[id];
				hb::log::error("skill {}: Skill.cfg type {} values {} {} {} {} {} {} | world.db type {} values {} {} {} {} {} {} (names {})", id,
					a.m_sType, a.m_sValue1, a.m_sValue2, a.m_sValue3, a.m_sValue4, a.m_sValue5, a.m_sValue6,
					b.m_sType, b.m_sValue1, b.m_sValue2, b.m_sValue3, b.m_sValue4, b.m_sValue5, b.m_sValue6,
					std::memcmp(a.m_cName, b.m_cName, sizeof(a.m_cName)) == 0 ? "match" : "differ");
			}
		}
		if (differ == 0)
			hb::log::info("skill: {} compared, all identical (name bytes, type, value1..6)", compared);
		else
			hb::log::error("skill: {} compared, {} differ", compared, differ);
		return differ == 0;
	}
}

int main(int argc, char** argv)
{
	hb::log::initialize("world_import", "import");
	options parsed;
	if (!parse_arguments(argc, argv, parsed))
	{
		hb::log::shutdown();
		return 2;
	}
	world::world_database db;
	std::string error;
	bool ok = db.open(parsed.database, 1, error);
	if (!ok)
		hb::log::error("cannot open {}", error);
	else if (parsed.compare)
		ok = compare_skills(parsed, db);
	else
		ok = import_skills(parsed, db);
	hb::log::shutdown();
	return ok ? 0 : 1;
}
