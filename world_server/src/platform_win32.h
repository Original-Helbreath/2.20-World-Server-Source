#pragma once

// The World host's only Win32 code (Standards sect. 8). The legacy game logic receives its socket events as window
// messages (WSAAsyncSelect), so the host keeps one hidden message-only window and pumps it from the console
// thread; nothing is ever shown. WORLD-003 retires this when the Login link replaces the legacy sockets.

#include <windows.h>

namespace hb::server::world::platform
{
	// Receives every message of the hidden window; sets handled to false to leave it to the default handler.
	using message_handler = LRESULT (*)(UINT message, WPARAM first, LPARAM second, bool& handled);

	// Creates the hidden message-only window; null on failure. The handle is what CGame and XSocket expect.
	HWND create_message_window(message_handler handler);
	void destroy_message_window();

	// Ctrl+C, Ctrl+Break and the console close/logoff/shutdown events request a stop.
	bool install_stop_handler();
	void remove_stop_handler();

	bool stop_requested();
	const char* stop_reason();
	void request_stop(const char* reason);

	// Called when teardown is done: releases a close/logoff/shutdown event the OS is holding the process for.
	void notify_shutdown_complete();

	// Sleeps up to timeout_ms, waking early for a stop request or a window message; dispatches pending messages.
	void pump(unsigned timeout_ms);

	// Finer system timer resolution for the whole run (the game reads timeGetTime).
	void begin_timer_resolution();
	void end_timer_resolution();
}
