#include "world_database.h"

#include "log.h"

#include <schemata/sqlite.h>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <format>
#include <set>

namespace hb::server::world
{
	bool world_database::open(const std::string& path, std::size_t pool_size, std::string& error)
	{
		error.clear();
		try
		{
			const std::filesystem::path file(path);
			if (file.has_parent_path())
				std::filesystem::create_directories(file.parent_path());
			m_model.add<skill>();
			schemata::PoolOptions options;
			options.max_size = pool_size;
			m_pool = std::make_unique<schemata::ConnectionPool>([path] { return schemata::sqlite(path); }, options);

			schemata::ConnectionPool::Lease lease = m_pool->acquire();
			const schemata::EnsureResult result = schemata::ensure_schema(*lease, m_model);
			if (result.applied > 0)
				log::info("schema updated: {} statement(s) applied to {}", result.applied, path);
			else
				log::info("schema verified ({})", path);
			return true;
		}
		catch (const std::exception& failure)
		{
			// A database-layer fault is reported, never worked around here (og-helbreath rule: Schemata owns it).
			error = std::format("{}: {}", path, failure.what());
			return false;
		}
	}

	bool world_database::load_skills(std::vector<skill>& skills, std::string& error)
	{
		error.clear();
		try
		{
			schemata::ConnectionPool::Lease lease = m_pool->acquire();
			skills = schemata::from<skill>(*lease, m_model).order_by(schemata::asc(&skill::id)).to_vector();
			return true;
		}
		catch (const std::exception& failure)
		{
			error = failure.what();
			return false;
		}
	}

	bool world_database::missing_skills(std::span<const std::int64_t> ids, std::vector<std::int64_t>& missing, std::string& error)
	{
		error.clear();
		missing.clear();
		if (ids.empty())
			return true;
		try
		{
			const std::set<std::int64_t> wanted(ids.begin(), ids.end());
			const std::vector<std::int64_t> list(wanted.begin(), wanted.end());
			schemata::ConnectionPool::Lease lease = m_pool->acquire();
			const std::vector<skill> found = schemata::from<skill>(*lease, m_model).where(schemata::in(schemata::col(&skill::id), list)).to_vector();
			std::set<std::int64_t> present;
			for (const skill& row : found)
				present.insert(row.id);
			std::ranges::set_difference(wanted, present, std::back_inserter(missing));
			return true;
		}
		catch (const std::exception& failure)
		{
			error = failure.what();
			return false;
		}
	}

	bool world_database::write_skills(const std::vector<skill>& skills, bool replace, std::string& error)
	{
		error.clear();
		try
		{
			schemata::Context ctx(m_pool->acquire(), m_model);
			ctx.transaction([&] {
				const std::int64_t existing = ctx.query<skill>().count();
				if (existing > 0)
				{
					if (!replace)
						throw std::runtime_error(std::format("the skill table already holds {} row(s); pass --replace to rewrite it", existing));
					// No filter: the whole table goes, in the same transaction as the new rows.
					ctx.query<skill>().remove();
				}
				if (!skills.empty())
					ctx.insert_many(skills);
			});
			return true;
		}
		catch (const std::exception& failure)
		{
			error = failure.what();
			return false;
		}
	}
}
