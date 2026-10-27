#pragma once

#include "common_types.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// The Login server's session grants, held until the client arrives with the ticket (WORLD-005, plan sect. 4.6).
// A ticket is used once: any presentation of it, right or wrong character, consumes it. Tickets compare in constant
// time. One grant per account: a newer grant replaces an older one.
namespace hb::server::world
{
	enum class grant_take
	{
		ok,
		unknown,   // no held grant has this ticket, or the character differs
		expired,   // the grant's lifetime ran out before the client arrived
	};

	class session_grants
	{
	public:
		using clock = std::chrono::steady_clock;

		void add(std::uint64_t account_id, std::string_view character, const hb::net::session_ticket& ticket,
			std::chrono::seconds lifetime, clock::time_point now);

		// Takes the grant for this ticket; on ok, account_id is the account it was granted to.
		grant_take take(const hb::net::session_ticket& ticket, std::string_view character, clock::time_point now,
			std::uint64_t& account_id);

		// Drops expired grants; returns how many.
		std::size_t prune(clock::time_point now);

		std::size_t size() const { return m_grants.size(); }

	private:
		struct grant
		{
			std::uint64_t account_id = 0;
			std::string character_key;
			hb::net::session_ticket ticket{};
			clock::time_point expires;
		};
		std::vector<grant> m_grants;
	};
}
