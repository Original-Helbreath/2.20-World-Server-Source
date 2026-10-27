#pragma once

#include <schemata/schemata.h>

#include <cstdint>
#include <optional>
#include <string>

// Test stand-in for the Login server's `accounts` table, identical to login_server/src/entities.h in the Login
// repository. The World server never declares this table; the tests declare it in a separate model, the way the
// Login server does in its own process, to prove the World's schema sync leaves it and its rows alone.
namespace tests
{
	struct login_account
	{
		std::int64_t id = 0;
		std::string name;
		std::string name_key;
		std::string password_hash;
		std::string password_salt;
		std::int32_t hash_iterations = 0;
		std::int32_t hash_memory_kib = 0;
		std::int32_t hash_lanes = 0;
		std::int32_t pepper_id = 0;
		bool locked = false;
		schemata::timestamp created_at{};
		std::optional<schemata::timestamp> last_login_at;
	};
}

template <>
struct schemata::Entity<tests::login_account>
{
	static void describe(schemata::EntityBuilder<tests::login_account>& e)
	{
		using tests::login_account;
		e.table("accounts");
		SCH_KEY(e, id).identity();
		SCH_PROP(e, name).max_length(10);
		SCH_PROP(e, name_key).max_length(10).unique();
		SCH_PROP(e, password_hash).max_length(64);
		SCH_PROP(e, password_salt).max_length(32);
		SCH_PROP(e, hash_iterations);
		SCH_PROP(e, hash_memory_kib);
		SCH_PROP(e, hash_lanes);
		SCH_PROP(e, pepper_id);
		SCH_PROP(e, locked).default_sql("0");
		SCH_PROP(e, created_at);
		SCH_PROP(e, last_login_at);
	}
};
