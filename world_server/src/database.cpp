#include "database.h"

#include "entities.h"
#include "log.h"

#include <schemata/sqlite.h>

#include <exception>
#include <filesystem>
#include <format>

namespace hb::server::world
{
	bool database::open(const std::string& path, std::size_t pool_size, std::string& error)
	{
		error.clear();
		try
		{
			const std::filesystem::path file(path);
			if (file.has_parent_path())
				std::filesystem::create_directories(file.parent_path());
			m_model.add<character>();
			m_model.add<character_item>();
			m_model.add<character_skill>();
			m_model.add<character_magic>();
			schemata::PoolOptions options;
			options.max_size = pool_size;
			m_pool = std::make_unique<schemata::ConnectionPool>([path] { return schemata::sqlite(path); }, options);

			schemata::ConnectionPool::Lease lease = m_pool->acquire();
			const schemata::EnsureResult result = schemata::ensure_schema(*lease, m_model);
			if (result.applied > 0)
				log::info("schema updated: {} statement(s) applied to {}", result.applied, path);
			else
				log::info("schema verified ({})", path);
			// A key into a table the Login server has not created yet waits; the next start adds it.
			m_deferred_foreign_keys = result.deferred_foreign_keys.size();
			for (const schemata::DeferredForeignKey& key : result.deferred_foreign_keys)
				log::warning("foreign key {} deferred: waiting for table {}", key.name, key.referenced_table);
			return true;
		}
		catch (const std::exception& failure)
		{
			// A database-layer fault is reported, never worked around here (og-helbreath rule: Schemata owns it).
			error = std::format("{}: {}", path, failure.what());
			return false;
		}
	}
}
