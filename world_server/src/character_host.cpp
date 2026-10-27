#include "character_host.h"

#include "character_service.h"
#include "character_store.h"
#include "database.h"
#include "log.h"

#include <memory>

namespace hb::server::world
{
	namespace
	{
		// Four connections: the host loop is the only user today; the pool leaves room for the save work to come.
		constexpr std::size_t pool_size = 4;

		std::unique_ptr<database> g_database;
		std::unique_ptr<character_store> g_store;
		std::unique_ptr<character_service> g_service;
	}

	character_service* open_characters(const std::string& players_database)
	{
		close_characters();
		auto opened = std::make_unique<database>();
		std::string error;
		if (!opened->open(players_database, pool_size, error))
		{
			hb::log::error("cannot open players database: {}", error);
			return nullptr;
		}
		g_database = std::move(opened);
		g_store = std::make_unique<character_store>(*g_database);
		g_service = std::make_unique<character_service>(*g_store);
		return g_service.get();
	}

	character_store* characters_store()
	{
		return g_store.get();
	}

	void close_characters()
	{
		g_service.reset();
		g_store.reset();
		g_database.reset();
	}
}
