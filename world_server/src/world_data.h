#pragma once

#include "world_templates.h"

#include <string>
#include <vector>

// Owns world.db for the console host (WORLD-008), so the legacy game translation units read their templates without
// including Schemata. Opened before the game initialises; the game copies the templates into its own lists.
namespace hb::server::world
{
	class template_references;

	// Opens world.db, creates or heals its tables and reads the templates. False (with the reason logged) on a fault
	// or when a template table the game needs is empty.
	bool open_world_data(const std::string& world_database);

	// Releases what open_world_data made. Safe to call when nothing is open.
	void close_world_data();

	// The skills read at open, by id. Empty when world.db is not open.
	const std::vector<skill>& world_skills();

	// The cross-file reference check over the open world.db; null when it is not open.
	const template_references* world_references();
}
