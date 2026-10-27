#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

// The world.db template rows as plain structs (WORLD-008). No Schemata here, so the legacy game translation units
// can include it; world_entities.h maps them. Design: .claude/design/plans/world-db.md.
namespace hb::server::world
{
	// The game holds a skill name in char[21]: at most 20 legacy (CP949) bytes, checked by the importer and on load.
	constexpr std::size_t max_skill_name_bytes = 20;

	// One skill (legacy GameData\Skill.cfg line: skill = Num Name Type V1..V6). id is the legacy number: character
	// saves, packets and the client use it. Each value slot has a fixed meaning per type in the game logic.
	struct skill
	{
		std::int64_t id = 0;
		std::string name;  // UTF-8
		std::int32_t type = 0;
		std::int32_t value1 = 0;
		std::int32_t value2 = 0;
		std::int32_t value3 = 0;
		std::int32_t value4 = 0;
		std::int32_t value5 = 0;
		std::int32_t value6 = 0;
	};
}
