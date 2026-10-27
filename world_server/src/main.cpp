// World server console host (WORLD-002): replaces the legacy Win32 window shell (Wmain.cpp).
// Game logic is untouched; this file starts it, drives its 300 ms timer from the console thread, forwards the
// legacy socket-event messages (the game clients' WSAAsyncSelect sockets) and shuts down on Ctrl+C or a console
// close. WORLD-003 adds the Login link, polled from the same loop, in place of the legacy log and gate sockets.

#include "log.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <exception>
#include <memory>
#include <string_view>

#include "Game.h"
#include "UserMessages.h"
#include "character_host.h"
#include "login_link.h"
#include "platform_win32.h"
#include "world_config.h"
#include "world_data.h"

char			G_cTxt[512];
char			G_cData50000[50000];

class XSocket * G_pListenSock = NULL;
class CGame *   G_pGame       = NULL;

BOOL			G_bIsThread = TRUE;
BOOL            G_bShutdown = FALSE;

extern FILE * pLogFile;				// legacy_log_files.cpp

namespace
{
	constexpr std::string_view application_name = "world_server";
	constexpr const char * config_file = "world_server.json";
	// The game's own tick, as the old multimedia timer ran it.
	constexpr std::chrono::milliseconds timer_interval{ 300 };
	// The loop never sleeps longer than this, so the Login link's reads, heartbeats and timers stay prompt.
	constexpr std::chrono::milliseconds link_poll_interval{ 20 };

	std::unique_ptr<hb::server::world::login_link> G_pLoginLink;

	// The legacy message log. Messages that flag a failure are raised to error, the rest are info.
	hb::log::level classify(std::string_view message)
	{
		if (message.starts_with("(!!!)") || message.starts_with("@@@"))
			return hb::log::level::error;
		return hb::log::level::info;
	}

	// What WndProc used to do: the game clients' socket events arrive here as window messages (WSAAsyncSelect).
	LRESULT dispatch_message(UINT message, WPARAM first, LPARAM second, bool& handled)
	{
		if (G_pGame == NULL || G_bShutdown == TRUE) {
			handled = false;
			return 0;
		}

		switch (message) {
		case WM_USER_STARTGAMESIGNAL:
			G_pGame->OnStartGameSignal();
			return 0;

		case WM_USER_ACCEPT:
			G_pGame->bAccept(G_pListenSock);
			return 0;

		default:
			if ((message >= WM_ONCLIENTSOCKETEVENT) && (message < WM_ONCLIENTSOCKETEVENT + DEF_MAXCLIENTS)) {
				G_pGame->OnClientSocketEvent(message, first, second);
				return 0;
			}
			break;
		}

		// WM_DESTROY is how the game asks for its final shutdown (CGame::OnTimer, SendMessage(m_hWnd, WM_DESTROY)).
		// The teardown itself runs from the loop, never from inside the game's own call.
		if (message == WM_DESTROY) {
			G_bShutdown = TRUE;
			hb::server::world::platform::request_stop("game shutdown");
			return 0;
		}

		handled = false;
		return 0;
	}

	// The Login link: registers this World, keeps the heartbeats going and reconnects. The game activates once it
	// is registered and refuses new players while it is down.
	void start_login_link(const hb::server::world::world_config& config, hb::server::world::character_service * characters)
	{
		namespace world = hb::server::world;

		world::link_registration registration;
		registration.client_host = G_pGame->m_cGameServerAddr;
		registration.client_port = static_cast<std::uint16_t>(G_pGame->m_iGameServerPort);
		registration.max_players = static_cast<std::uint16_t>(DEF_MAXCLIENTS - 1);

		world::link_events events;
		events.on_registered = [] {
			if (G_pGame != NULL) G_pGame->m_bIsLoginLinkAvailable = TRUE;
		};
		events.on_lost = [] {
			if (G_pGame != NULL) G_pGame->m_bIsLoginLinkAvailable = FALSE;
			hb::log::warning("refusing new players until the Login link is back");
		};
		events.on_refused = [](std::string_view) {
			hb::server::world::platform::request_stop("the Login server refused this World");
		};
		events.report = [] {
			world::link_report report;
			if (G_pGame == NULL) return report;
			report.is_accepting_players = (G_pGame->m_bIsGameStarted == TRUE) && (G_pGame->m_bOnExitProcess == FALSE)
				&& (G_pGame->m_bIsLoginLinkAvailable == TRUE);
			report.is_shutting_down = (G_pGame->m_bOnExitProcess == TRUE);
			report.player_count = static_cast<std::uint16_t>(std::clamp(G_pGame->m_iTotalClients, 0, 65535));
			report.max_players = static_cast<std::uint16_t>(DEF_MAXCLIENTS - 1);
			return report;
		};
		events.characters = characters;

		G_pLoginLink = std::make_unique<world::login_link>(config, std::move(registration), std::move(events));
		G_pLoginLink->start();
	}

	bool initialize()
	{
		namespace platform = hb::server::world::platform;

		hb::log::info("Helbreath World Server V{}.{} build {}", DEF_UPPERVERSION, DEF_LOWERVERSION, DEF_BUILDDATE);

		hb::server::world::world_config config;
		std::string config_error;
		if (!hb::server::world::load_world_config(config_file, config, config_error)) {
			hb::log::error("{}", config_error);
			return false;
		}
		hb::log::set_level(config.log_level);
		if (hb::server::world::is_sample_key(config))
			hb::log::warning("link_key is the all-zero sample key; set a real key before deploying");
		hb::log::info("world {}, Login server at {}:{}", config.world_name, config.login_address, config.login_port);

		if (_InitWinsock() == FALSE) {
			hb::log::error("Winsock 2.2 not available");
			return false;
		}

		// WORLD-008: the templates come from world.db; the game copies them in during bInit.
		if (!hb::server::world::open_world_data(config.world_database)) return false;

		HWND window = platform::create_message_window(dispatch_message);
		if (window == nullptr) {
			hb::log::error("cannot create the socket-event window");
			return false;
		}

		G_pGame = new class CGame(window);
		if (G_pGame->bInit() == FALSE) {
			hb::log::error("game initialisation failed");
			return false;
		}

		G_pListenSock = new class XSocket(window, DEF_SERVERSOCKETBLOCKLIMIT);
		if (G_pListenSock->bListen(G_pGame->m_cGameServerAddr, G_pGame->m_iGameServerPort, WM_USER_ACCEPT) == FALSE) {
			hb::log::error("cannot listen on {}:{}", G_pGame->m_cGameServerAddr, G_pGame->m_iGameServerPort);
			return false;
		}
		hb::log::info("listening on {}:{}", G_pGame->m_cGameServerAddr, G_pGame->m_iGameServerPort);

		// WORLD-006: players.db holds the characters the Login server lists, creates and deletes through this World.
		hb::server::world::character_service * characters = hb::server::world::open_characters(config.players_database, *hb::server::world::world_references());
		if (characters == nullptr) return false;

		start_login_link(config, characters);
		return true;
	}

	// The game's timer and event loop: socket messages are pumped as they arrive, the Login link is polled, and the
	// game ticks every 300 ms.
	void run_loop()
	{
		namespace platform = hb::server::world::platform;
		using clock = std::chrono::steady_clock;

		auto next_tick = clock::now() + timer_interval;
		while (!platform::stop_requested()) {
			if (G_pLoginLink) G_pLoginLink->poll();
			auto now = clock::now();
			if (now >= next_tick) {
				if (G_bShutdown == FALSE) G_pGame->OnTimer(0);
				next_tick += timer_interval;
				if (next_tick <= now) next_tick = now + timer_interval;
				now = clock::now();
			}
			if (platform::stop_requested()) break;
			// (std::min) in parentheses: windows.h defines a min macro.
			const auto wait = (std::min)(std::chrono::duration_cast<std::chrono::milliseconds>(next_tick - now), link_poll_interval);
			platform::pump(wait.count() > 0 ? static_cast<unsigned>(wait.count()) : 0u);
		}
	}

	// What OnDestroy used to do. The shared state is torn down in the same order.
	void shut_down()
	{
		namespace platform = hb::server::world::platform;

		G_bShutdown = TRUE;
		hb::log::info("shutting down ({})", platform::stop_reason());
		if (G_pGame != NULL && G_pGame->m_iTotalClients > 0)
			hb::log::warning("{} player(s) still connected; their data is not saved", G_pGame->m_iTotalClients);

		if (G_pLoginLink) {
			G_pLoginLink->stop();
			G_pLoginLink.reset();
		}
		hb::server::world::close_characters();
		hb::server::world::close_world_data();  // after the characters: their store checks keys against it

		if (G_pListenSock != NULL) delete G_pListenSock;
		G_pListenSock = NULL;

		if (G_pGame != NULL) {
			G_pGame->Quit();
			delete G_pGame;
			G_pGame = NULL;
		}

		platform::destroy_message_window();
		_TermWinsock();
		if (pLogFile != NULL) {
			fclose(pLogFile);
			pLogFile = NULL;
		}
		hb::log::info("stopped");
	}
}

// The legacy game logic's output call; it used to fill the window's list box. Trailing blanks are trimmed and
// a blank spacer line is dropped.
void PutLogList(char * cMsg)
{
	std::string_view message(cMsg);
	while (message.ends_with(' ') || message.ends_with('\n')) message.remove_suffix(1);
	if (message.empty()) return;
	hb::log::write(classify(message), message);
}

int main()
{
	namespace platform = hb::server::world::platform;

	hb::log::initialize(application_name, "world");
	bool started = false;
	try {
		platform::begin_timer_resolution();
		if (!platform::install_stop_handler())
			hb::log::warning("Ctrl+C handler not installed");

		started = initialize();
		if (started) run_loop();
		else platform::request_stop("startup failed");
		shut_down();
	}
	catch (const std::exception& error) {
		hb::log::report_crash(error.what());
		started = false;
	}
	platform::remove_stop_handler();
	platform::end_timer_resolution();
	hb::log::shutdown();
	platform::notify_shutdown_complete();
	return started ? 0 : 1;
}
