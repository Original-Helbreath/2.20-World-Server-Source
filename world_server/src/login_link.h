#pragma once

#include "world_config.h"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

// The World's link to the Login server (WORLD-003): the World is an hb::net::client of the Login's World link.
// It sends world_register first, then link_heartbeat every heartbeat_seconds and world_status on change and
// every 10 s; it treats 3 missed link_heartbeats from Login as a loss and reconnects. It replaces the legacy
// main-log, sub-log and gate sockets.
//
// Asio stays out of this header. The link has no thread of its own: the host loop calls poll(), so every
// handler below runs on that one thread, next to the legacy WSAAsyncSelect client sockets.
namespace hb::server::world
{
	// What world_register tells the Login server about this World.
	struct link_registration
	{
		std::string client_host;      // where game clients connect to this World
		std::uint16_t client_port = 0;
		std::uint16_t max_players = 0;
	};

	// What world_status reports; the host fills it in from the game.
	struct link_report
	{
		bool is_accepting_players = false;
		bool is_shutting_down = false;
		std::uint16_t player_count = 0;
		std::uint16_t max_players = 0;

		bool operator==(const link_report&) const = default;
	};

	class character_service;

	// The two Login requests the session handling answers (link_messages.h); declared here so this header stays plain.
	struct session_grant_request
	{
		std::uint64_t account_id = 0;
		std::string character;
		std::array<std::uint8_t, 32> ticket{};
		std::uint16_t expires_in_seconds = 0;
	};

	struct link_events
	{
		std::function<void()> on_registered;                   // the Login accepted this World
		std::function<void()> on_lost;                         // a registered link went down; reconnecting
		std::function<void(std::string_view)> on_refused;      // refused for good (wrong key, unknown world); no retry
		std::function<link_report()> report;                   // current status
		character_service* characters = nullptr;               // answers list, create and delete (WORLD-006); null: refused
		// WORLD-005: a session_grant; returns the hb::net::link::grant_result to answer with. Unset: shutting_down.
		std::function<std::uint8_t(const session_grant_request&)> on_session_grant;
		// WORLD-005: kick_account; reason is an hb::net::link::kick_reason value.
		std::function<void(std::uint64_t account_id, std::uint8_t reason)> on_kick;
	};

	class login_link
	{
	public:
		login_link(const world_config& config, link_registration registration, link_events events);
		~login_link();

		login_link(const login_link&) = delete;
		login_link& operator=(const login_link&) = delete;

		// Begins connecting; retries every 2 s until the Login server answers.
		void start();

		// Runs whatever is ready (connects, reads, timers) without blocking, then sends world_status if it changed.
		void poll();

		// Ends the connection and retries, and drains what that leaves for a moment.
		void stop();

		bool is_registered() const;

		// WORLD-005: session_started / session_ended to the Login server; dropped (logged at debug) while unregistered.
		// reason is an hb::net::link::session_end_reason value.
		void send_session_started(std::uint64_t account_id, std::string_view character);
		void send_session_ended(std::uint64_t account_id, std::uint8_t reason);

	private:
		struct impl;
		std::unique_ptr<impl> m_impl;
	};
}
