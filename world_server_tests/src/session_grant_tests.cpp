// Session grant tests (WORLD-005): a ticket enters once, for its character, before it expires.
#include "pending_limits.h"
#include "session_grants.h"
#include "test_runner.h"

#include <chrono>

namespace tests
{
	void session_grant_tests(runner& t)
	{
		using namespace hb::server::world;
		using namespace std::chrono_literals;
		const auto now = session_grants::clock::now();
		hb::net::session_ticket ticket{};
		ticket.fill(7);
		hb::net::session_ticket other{};
		other.fill(8);

		session_grants grants;
		grants.add(5, "Hero1", ticket, 30s, now);
		std::uint64_t account = 0;
		t.check(grants.take(other, "Hero1", now, account) == grant_take::unknown, "an unknown ticket is refused");
		t.check(grants.take(ticket, "hero1", now + 1s, account) == grant_take::ok && account == 5, "the ticket enters, any case");
		t.check(grants.take(ticket, "Hero1", now + 2s, account) == grant_take::unknown, "a used ticket is refused");

		grants.add(5, "Hero1", ticket, 30s, now);
		t.check(grants.take(ticket, "Other", now, account) == grant_take::unknown, "a ticket for another character is refused");
		t.check(grants.take(ticket, "Hero1", now, account) == grant_take::unknown, "and is used up by that try");

		grants.add(5, "Hero1", ticket, 30s, now);
		t.check(grants.take(ticket, "Hero1", now + 31s, account) == grant_take::expired, "an expired ticket is refused as expired");

		grants.add(5, "Hero1", ticket, 30s, now);
		grants.add(5, "Hero2", other, 30s, now);
		t.check(grants.size() == 1, "a newer grant replaces the account's older one");
		t.check(grants.take(ticket, "Hero1", now, account) == grant_take::unknown, "the replaced ticket no longer enters");
		t.check(grants.prune(now + 31s) == 1 && grants.size() == 0, "expired grants are pruned");

		// Pending-connection caps (review finding 1).
		pending_limits pending(3, 2);
		t.check(pending.try_add("10.0.0.1") && pending.try_add("10.0.0.1"), "two pending from one address");
		t.check(!pending.try_add("10.0.0.1"), "a third from that address is refused");
		t.check(pending.try_add("10.0.0.2"), "another address still gets in");
		t.check(!pending.try_add("10.0.0.3") && pending.total() == 3, "the total cap holds");
		pending.remove("10.0.0.1");
		t.check(pending.try_add("10.0.0.1") && pending.total() == 3, "a slot freed by entry or close is reused");
		pending.remove("10.0.0.9");
		t.check(pending.total() == 3, "removing an address that holds none changes nothing");
		t.check(max_pending_connections == 32 && max_pending_per_address == 2, "World caps: 32 pending, 2 per address");

		// Game message sizes (review finding 2): id and type are read unchecked by the legacy handlers.
		t.check(!is_acceptable_game_message(0) && !is_acceptable_game_message(5), "bodies under 6 bytes are refused");
		t.check(is_acceptable_game_message(6) && is_acceptable_game_message(30000), "6 to 30000 bytes are accepted");
		t.check(!is_acceptable_game_message(30001), "over the legacy buffer is refused");
	}
}
