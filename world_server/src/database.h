#pragma once

#include <schemata/schemata.h>

#include <cstddef>
#include <memory>
#include <string>

// players.db through Schemata. Every access borrows a connection from the pool for one unit of work.
// The file is shared with the Login server: this model declares only the World's character tables.
namespace hb::server::world
{
	class database
	{
	public:
		// Opens the pool and creates or heals the character tables. False (with the reason) on any database fault.
		bool open(const std::string& path, std::size_t pool_size, std::string& error);

		schemata::ConnectionPool& pool()
		{
			return *m_pool;
		}

		const schemata::Model& model() const
		{
			return m_model;
		}

		// Foreign keys the last open() could not add yet (their table belongs to the Login server and is not created).
		std::size_t deferred_foreign_keys() const
		{
			return m_deferred_foreign_keys;
		}

	private:
		schemata::Model m_model;
		std::unique_ptr<schemata::ConnectionPool> m_pool;
		std::size_t m_deferred_foreign_keys = 0;
	};
}
