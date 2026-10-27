#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <string_view>

// Caps on game-client connections that have not yet entered with a ticket (WORLD-005 review): a separate pool from
// the players' slots, so unauthenticated connections cannot crowd players out. Counted in total and per peer address.
namespace hb::server::world
{
	inline constexpr std::size_t max_pending_connections = 32;
	inline constexpr std::size_t max_pending_per_address = 2;

	// The smallest legacy game message: DWORD message id and WORD message type (DEF_INDEX2_MSGTYPE + 2). Every legacy
	// handler reads those six bytes unchecked, so a shorter world_game_message body is refused.
	inline constexpr std::size_t min_game_message_bytes = 6;
	// The legacy message buffer (DEF_MSGBUFFERSIZE); a longer body is refused.
	inline constexpr std::size_t max_game_message_bytes = 30000;

	constexpr bool is_acceptable_game_message(std::size_t size)
	{
		return size >= min_game_message_bytes && size <= max_game_message_bytes;
	}

	class pending_limits
	{
	public:
		explicit pending_limits(std::size_t total = max_pending_connections, std::size_t per_address = max_pending_per_address)
			: m_total_limit(total), m_per_address_limit(per_address)
		{
		}

		// Counts one more pending connection from this address; false (nothing counted) when either cap is reached.
		bool try_add(std::string_view address)
		{
			if (m_total >= m_total_limit)
				return false;
			std::size_t& from_address = m_by_address[std::string(address)];
			if (from_address >= m_per_address_limit)
			{
				if (from_address == 0)
					m_by_address.erase(std::string(address));
				return false;
			}
			++from_address;
			++m_total;
			return true;
		}

		// The connection entered or closed.
		void remove(std::string_view address)
		{
			const auto found = m_by_address.find(std::string(address));
			if (found == m_by_address.end() || found->second == 0)
				return;
			--found->second;
			--m_total;
			if (found->second == 0)
				m_by_address.erase(found);
		}

		std::size_t total() const { return m_total; }

	private:
		std::size_t m_total_limit;
		std::size_t m_per_address_limit;
		std::size_t m_total = 0;
		std::map<std::string, std::size_t, std::less<>> m_by_address;
	};
}
