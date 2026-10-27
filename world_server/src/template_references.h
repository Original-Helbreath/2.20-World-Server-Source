#pragma once

#include <cstdint>
#include <span>
#include <string>

// The one place the World checks references from players.db into world.db (WORLD-008; user rulings 2026-10-27).
// SQLite has no foreign keys across database files, so a key that points into the other file is resolved there
// first: only when every key resolves is the data pulled (load) or the write allowed (insert, update); otherwise the
// action is refused with the unresolved keys named. Keys within one file stay real foreign keys.
//
// Slice 1 covers character_skills.skill_id -> world.db skill.id. character_items and character_magic join when their
// template tables land (design: .claude/design/plans/world-db.md, sect. 9).
namespace hb::server::world
{
	class world_database;

	class template_references
	{
	public:
		explicit template_references(world_database& templates) : m_templates(templates) {}

		// True when every id is a skill in world.db. False with the reason (the unresolved ids, or the fault).
		bool resolve_skills(std::span<const std::int64_t> skill_ids, std::string& error) const;

	private:
		world_database& m_templates;
	};
}
