#include "world_host.h"

#include "character_store.h"
#include "client_gate.h"
#include "game_access.h"
#include "link_messages.h"
#include "log.h"
#include "session_grants.h"
#include "world_bridge.h"
#include "world_messages.h"

#include <chrono>
#include <memory>
#include <optional>

namespace hb::server::world
{
	namespace
	{
		namespace link = hb::net::link;
		using hb::net::world::world_enter_result;

		struct host_state
		{
			character_store* store = nullptr;
			login_link* link = nullptr;
			session_grants grants;
			std::unique_ptr<client_gate> gate;
		};

		host_state g_host;

		std::uint8_t end_reason(link::session_end_reason reason)
		{
			return static_cast<std::uint8_t>(reason);
		}

		void on_enter(const std::shared_ptr<client_session>& session, const hb::net::world::world_enter_request& request)
		{
			const std::string character(request.character.view());
			std::uint64_t account_id = 0;
			switch (g_host.grants.take(request.ticket, character, session_grants::clock::now(), account_id))
			{
			case grant_take::unknown:
				hb::log::warning("world_enter refused for {}: unknown or used ticket", character);
				client_gate::refuse(*session, world_enter_result::bad_ticket);
				return;
			case grant_take::expired:
				hb::log::warning("world_enter refused for {}: ticket expired", character);
				client_gate::refuse(*session, world_enter_result::expired);
				return;
			case grant_take::ok:
				break;
			}

			// The grant is used up from here on: every refusal also tells the Login server the account is not coming,
			// so it does not hold it as waiting for this entry.
			const auto refuse_taken = [&](world_enter_result result, std::string_view why) {
				hb::log::warning("world_enter refused for {}: {}", character, why);
				client_gate::refuse(*session, result);
				if (g_host.link != nullptr)
					g_host.link->send_session_ended(account_id, end_reason(link::session_end_reason::disconnected));
			};

			character_record record;
			std::string error;
			const load_result loaded = g_host.store->load(character, record, error);
			if (loaded == load_result::failed)
			{
				hb::log::error("loading character {}: {}", character, error);
				refuse_taken(world_enter_result::bad_ticket, "the character could not be loaded");
				return;
			}
			if (loaded == load_result::not_found || record.header.account_id != static_cast<std::int64_t>(account_id))
			{
				refuse_taken(world_enter_result::bad_ticket, "the character is not the account's");
				return;
			}

			// An older session of this account goes first, so it never counts against the World being full.
			game_replace_account(static_cast<std::int64_t>(account_id), record.header.name);

			switch (game_entry_state())
			{
			case game_state::accepting:
				break;
			case game_state::full:
				refuse_taken(world_enter_result::world_full, "the World is full");
				return;
			case game_state::shutting_down:
				refuse_taken(world_enter_result::shutting_down, "the World is shutting down");
				return;
			case game_state::not_ready:
				refuse_taken(world_enter_result::shutting_down, "the World is not ready");
				return;
			}

			const int client_index = game_prepare_player(client_gate::make_socket(session), static_cast<std::int64_t>(account_id), record, error);
			if (client_index < 0)
			{
				refuse_taken(world_enter_result::bad_ticket, error);
				return;
			}
			client_gate::admit(*session, client_index);
			game_finish_player(client_index);
			hb::log::info("<{}> {} entered the World (account {})", client_index, character, account_id);
		}
	}

	bool host_start(const std::string& bind_address, std::uint16_t port, std::size_t max_players, character_store& store)
	{
		g_host.store = &store;
		gate_events events;
		events.on_enter = on_enter;
		events.on_message = [](int client_index, const std::vector<std::byte>& message) { game_client_message(client_index, message); };
		events.on_closed = [](int client_index) { game_client_closed(client_index); };
		g_host.gate = std::make_unique<client_gate>(std::move(events));
		std::string error;
		if (!g_host.gate->listen(bind_address, port, max_players, error))
		{
			hb::log::error("cannot listen for game clients on {}:{}: {}", bind_address, port, error);
			g_host.gate.reset();
			return false;
		}
		hb::log::info("game clients on {}:{}", bind_address, port);
		return true;
	}

	void host_attach_link(login_link* link)
	{
		g_host.link = link;
	}

	void host_poll()
	{
		if (!g_host.gate)
			return;
		g_host.gate->poll();
		const std::size_t dropped = g_host.grants.prune(session_grants::clock::now());
		if (dropped > 0)
			hb::log::debug("{} session grant(s) expired", dropped);
	}

	void host_stop()
	{
		game_remove_all(end_reason(link::session_end_reason::world_shutdown));
		if (g_host.gate)
		{
			g_host.gate->stop();
			g_host.gate.reset();
		}
	}

	std::uint8_t host_session_grant(const session_grant_request& grant)
	{
		switch (game_entry_state())
		{
		case game_state::accepting:
			break;
		case game_state::full:
			return static_cast<std::uint8_t>(link::grant_result::world_full);
		case game_state::shutting_down:
		case game_state::not_ready:
			return static_cast<std::uint8_t>(link::grant_result::shutting_down);
		}
		if (g_host.store == nullptr)
			return static_cast<std::uint8_t>(link::grant_result::shutting_down);
		character_record record;
		std::string error;
		const load_result loaded = g_host.store->load(grant.character, record, error);
		if (loaded == load_result::failed)
		{
			hb::log::error("session grant for {}: {}", grant.character, error);
			return static_cast<std::uint8_t>(link::grant_result::shutting_down);
		}
		if (loaded == load_result::not_found || record.header.account_id != static_cast<std::int64_t>(grant.account_id))
			return static_cast<std::uint8_t>(link::grant_result::no_such_character);
		g_host.grants.add(grant.account_id, grant.character, grant.ticket, std::chrono::seconds(grant.expires_in_seconds),
			session_grants::clock::now());
		hb::log::debug("session grant held for {} (account {}, {} s)", grant.character, grant.account_id, grant.expires_in_seconds);
		return static_cast<std::uint8_t>(link::grant_result::ok);
	}

	void host_kick(std::uint64_t account_id, std::uint8_t reason)
	{
		hb::log::info("kick for account {} (reason {})", account_id, reason);
		game_remove_account(static_cast<std::int64_t>(account_id), end_reason(link::session_end_reason::kicked));
	}

	void host_announce_players()
	{
		if (g_host.link == nullptr)
			return;
		game_for_each_player([](std::int64_t account_id, std::string_view character) {
			g_host.link->send_session_started(static_cast<std::uint64_t>(account_id), character);
		});
	}
}

// world_bridge.h: the calls the legacy game logic makes.

int world_save_player(int iClientH)
{
	using namespace hb::server::world;
	if (g_host.store == nullptr)
		return 0;
	character_record record;
	if (!game_capture_player(iClientH, record))
		return 0;
	std::string error;
	if (!g_host.store->save(record, error))
	{
		hb::log::error("saving {} failed: {}", record.header.name, error);
		return 0;
	}
	hb::log::debug("saved {}", record.header.name);
	return 1;
}

void world_session_started(std::int64_t account_id, const char * character_name)
{
	using namespace hb::server::world;
	if (g_host.link != nullptr && account_id != 0)
		g_host.link->send_session_started(static_cast<std::uint64_t>(account_id), character_name);
}

void world_session_ended(std::int64_t account_id, char reason)
{
	using namespace hb::server::world;
	if (g_host.link != nullptr && account_id != 0)
		g_host.link->send_session_ended(static_cast<std::uint64_t>(account_id), static_cast<std::uint8_t>(reason));
}
