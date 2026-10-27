## Build the game server as the World server base

### World Server

**Added**
- `world_server.sln` and `world_server\world_server.vcxproj` (VS 2026, v145, x64 Debug and Release, /W3), building the 2.20 game server source imported unchanged into `world_server\src`.

**Changed**
- Translated every Korean comment, log string, item name and chat command in the World source to English; the source is pure ASCII and the CP949 execution charset is dropped.
- Replaced the Korean chat-command aliases with English ones (`/find`, `/whisper`, `/setprofile`, `/profile`, `/goodrep`, `/badrep`, `/target`, `/kick`), with their match lengths corrected.

**Fixed**
- Two breaks on x64: an ambiguous `abs` on a `DWORD` difference in `Game.cpp`, and the `timeSetEvent` callback signature (`DWORD_PTR`) in `Wmain.cpp` / `WINMAIN.H`.

Summary: World Server gains an x64 build of the 2.20 game logic with all Korean text translated to English (2026-10-27)
