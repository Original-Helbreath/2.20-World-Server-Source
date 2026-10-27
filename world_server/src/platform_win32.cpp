#include "platform_win32.h"

#include <mmsystem.h>

#include <atomic>

namespace hb::server::world::platform
{
	namespace
	{
		constexpr const char* window_class_name = "HBWorldServerMessages";
		// The OS ends the process about five seconds after a console close event returns or stalls.
		constexpr DWORD close_event_grace_ms = 4500;

		message_handler g_handler = nullptr;
		HWND g_window = nullptr;
		bool g_class_registered = false;
		HANDLE g_stop_event = nullptr;
		HANDLE g_done_event = nullptr;
		std::atomic<bool> g_stop{ false };
		std::atomic<const char*> g_reason{ "" };
		UINT g_timer_period = 0;

		LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM first, LPARAM second)
		{
			if (g_handler != nullptr)
			{
				bool handled = true;
				const LRESULT result = g_handler(message, first, second, handled);
				if (handled)
					return result;
			}
			return DefWindowProcA(window, message, first, second);
		}

		BOOL WINAPI console_event(DWORD type)
		{
			switch (type)
			{
			case CTRL_C_EVENT:
				request_stop("Ctrl+C");
				return TRUE;
			case CTRL_BREAK_EVENT:
				request_stop("Ctrl+Break");
				return TRUE;
			case CTRL_CLOSE_EVENT:
			case CTRL_LOGOFF_EVENT:
			case CTRL_SHUTDOWN_EVENT:
				request_stop(type == CTRL_CLOSE_EVENT ? "console closed" : "logoff or shutdown");
				// Hold the OS here until the main thread has finished its teardown.
				if (g_done_event != nullptr)
					WaitForSingleObject(g_done_event, close_event_grace_ms);
				return TRUE;
			default:
				return FALSE;
			}
		}
	}

	HWND create_message_window(message_handler handler)
	{
		g_handler = handler;
		const HINSTANCE instance = GetModuleHandleA(nullptr);
		WNDCLASSA window_class = {};
		window_class.lpfnWndProc = window_procedure;
		window_class.hInstance = instance;
		window_class.lpszClassName = window_class_name;
		if (RegisterClassA(&window_class) == 0)
			return nullptr;
		g_class_registered = true;
		g_window = CreateWindowExA(0, window_class_name, "world_server", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, instance, nullptr);
		return g_window;
	}

	void destroy_message_window()
	{
		if (g_window != nullptr)
		{
			DestroyWindow(g_window);
			g_window = nullptr;
		}
		if (g_class_registered)
		{
			UnregisterClassA(window_class_name, GetModuleHandleA(nullptr));
			g_class_registered = false;
		}
		g_handler = nullptr;
	}

	bool install_stop_handler()
	{
		g_stop_event = CreateEventA(nullptr, TRUE, FALSE, nullptr);
		g_done_event = CreateEventA(nullptr, TRUE, FALSE, nullptr);
		if (g_stop_event == nullptr || g_done_event == nullptr)
			return false;
		return SetConsoleCtrlHandler(console_event, TRUE) != 0;
	}

	void remove_stop_handler()
	{
		SetConsoleCtrlHandler(console_event, FALSE);
	}

	bool stop_requested()
	{
		return g_stop.load();
	}

	const char* stop_reason()
	{
		return g_reason.load();
	}

	void request_stop(const char* reason)
	{
		if (!g_stop.exchange(true))
			g_reason.store(reason);
		if (g_stop_event != nullptr)
			SetEvent(g_stop_event);
	}

	void notify_shutdown_complete()
	{
		if (g_done_event != nullptr)
			SetEvent(g_done_event);
	}

	void pump(unsigned timeout_ms)
	{
		const DWORD count = g_stop_event != nullptr ? 1 : 0;
		MsgWaitForMultipleObjectsEx(count, count != 0 ? &g_stop_event : nullptr, timeout_ms, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
		MSG message;
		while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}
	}

	void begin_timer_resolution()
	{
		TIMECAPS caps = {};
		if (timeGetDevCaps(&caps, sizeof(caps)) == TIMERR_NOERROR && timeBeginPeriod(caps.wPeriodMin) == TIMERR_NOERROR)
			g_timer_period = caps.wPeriodMin;
	}

	void end_timer_resolution()
	{
		if (g_timer_period != 0)
		{
			timeEndPeriod(g_timer_period);
			g_timer_period = 0;
		}
	}
}
