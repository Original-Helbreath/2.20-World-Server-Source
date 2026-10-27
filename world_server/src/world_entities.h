#pragma once

#include "world_templates.h"

#include <schemata/schemata.h>

// world.db: the World's static templates (WORLD-008, design in .claude/design/plans/world-db.md). Slice 1 maps the
// skill templates only; the other template and map tables follow slice by slice.
//
// world.db is its own file. Keys between its tables are real foreign keys; references from players.db into these
// tables (character_skills.skill_id, later character_items and character_magic) cannot be foreign keys in SQLite, so
// the World resolves them itself through template_references before it loads or writes such a row.

template <>
struct schemata::Entity<hb::server::world::skill>
{
	static void describe(schemata::EntityBuilder<hb::server::world::skill>& e)
	{
		e.table("skill");
		SCH_KEY(e, id);
		// UTF-8 takes up to 3 bytes per legacy 2-byte character; the 20-byte limit is enforced on the CP949 form.
		SCH_PROP(e, name).max_length(60).unique();
		// The game keeps every value in a short.
		SCH_PROP(e, type).check("type BETWEEN -32768 AND 32767");
		SCH_PROP(e, value1).check("value1 BETWEEN -32768 AND 32767");
		SCH_PROP(e, value2).check("value2 BETWEEN -32768 AND 32767");
		SCH_PROP(e, value3).check("value3 BETWEEN -32768 AND 32767");
		SCH_PROP(e, value4).check("value4 BETWEEN -32768 AND 32767");
		SCH_PROP(e, value5).check("value5 BETWEEN -32768 AND 32767");
		SCH_PROP(e, value6).check("value6 BETWEEN -32768 AND 32767");
		// m_pSkillConfigList[DEF_MAXSKILLTYPE], DEF_MAXSKILLTYPE = 60 (Client.h).
		e.check("CK_skill_id", "id BETWEEN 0 AND 59");
	}
};
