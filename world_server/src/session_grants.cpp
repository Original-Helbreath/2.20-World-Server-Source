#include "session_grants.h"

#include <algorithm>
#include <cctype>

namespace hb::server::world
{
	namespace
	{
		std::string key_of(std::string_view name)
		{
			std::string key(name);
			std::ranges::transform(key, key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return key;
		}

		// Every byte is compared whatever the earlier ones were, so the time taken says nothing about the ticket.
		bool same_ticket(const hb::net::session_ticket& a, const hb::net::session_ticket& b)
		{
			unsigned difference = 0;
			for (std::size_t i = 0; i < a.size(); ++i)
				difference |= static_cast<unsigned>(a[i] ^ b[i]);
			return difference == 0;
		}
	}

	void session_grants::add(std::uint64_t account_id, std::string_view character, const hb::net::session_ticket& ticket,
		std::chrono::seconds lifetime, clock::time_point now)
	{
		std::erase_if(m_grants, [account_id](const grant& g) { return g.account_id == account_id; });
		grant g;
		g.account_id = account_id;
		g.character_key = key_of(character);
		g.ticket = ticket;
		g.expires = now + lifetime;
		m_grants.push_back(std::move(g));
	}

	grant_take session_grants::take(const hb::net::session_ticket& ticket, std::string_view character, clock::time_point now,
		std::uint64_t& account_id)
	{
		account_id = 0;
		auto found = m_grants.end();
		for (auto it = m_grants.begin(); it != m_grants.end(); ++it)
		{
			if (same_ticket(it->ticket, ticket))
				found = it;
		}
		if (found == m_grants.end())
			return grant_take::unknown;
		const grant taken = *found;
		m_grants.erase(found);
		if (taken.expires <= now)
			return grant_take::expired;
		if (taken.character_key != key_of(character))
			return grant_take::unknown;
		account_id = taken.account_id;
		return grant_take::ok;
	}

	std::size_t session_grants::prune(clock::time_point now)
	{
		return std::erase_if(m_grants, [now](const grant& g) { return g.expires <= now; });
	}
}
