## Run the World server as a console program

### World Server

**Changed**
- The World server is a console program: the Win32 window, message loop, list-box log and multimedia timer are replaced by a console host (`main.cpp`) that drives the game's 300 ms tick and shuts down on Ctrl+C, Ctrl+Break or a console close.
- Log output goes through the shared `hb::log`; the legacy `PutLogList` feeds it.
- A hidden message-only window in `platform_win32.cpp` carries the legacy socket events until the Login link replaces the legacy sockets (WORLD-003).
- A failure to listen on the game port now stops start-up instead of being ignored.

**Added**
- The `shared/` submodule (2.20-Shared); `shared/log` is built into the World server.

**Fixed**
- The item log file pointer is cleared after closing, so shutdown no longer closes it twice.

Summary: World Server runs as a console program with hb::log output and a clean Ctrl+C shutdown (2026-10-27)
