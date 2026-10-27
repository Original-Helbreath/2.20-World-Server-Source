#pragma once

#include "world_entities.h"

#include <schemata/schemata.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

// world.db through Schemata (WORLD-008). The World reads its templates here; world_import writes them.
namespace hb::server::world
{
	class world_database
	{
	public:
		// Opens the pool and creates or heals the template tables. False (with the reason) on any database fault.
		bool open(const std::string& path, std::size_t pool_size, std::string& error);

		// Every skill, by id.
		bool load_skills(std::vector<skill>& skills, std::string& error);

		// The ids of `ids` that are not in the skill table (each once, ascending). False (with the reason) on a fault.
		bool missing_skills(std::span<const std::int64_t> ids, std::vector<std::int64_t>& missing, std::string& error);

		// Writes the skill table in one transaction. A table that already holds rows is refused unless replace is set,
		// which deletes them first.
		bool write_skills(const std::vector<skill>& skills, bool replace, std::string& error);

		schemata::ConnectionPool& pool()
		{
			return *m_pool;
		}

		const schemata::Model& model() const
		{
			return m_model;
		}

	private:
		schemata::Model m_model;
		std::unique_ptr<schemata::ConnectionPool> m_pool;
	};
}
