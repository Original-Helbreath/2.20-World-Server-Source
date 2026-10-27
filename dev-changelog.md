## Character list, create and delete

### World Server

**Added**
- The World answers the Login server's character list, create and delete requests from players.db.
- A new character gets the 2.20 starting defaults: the `default` map, level 1 and the starting kit (HP/MP/SP from the level-1 maximums and shirt colour 0 until the 2.20 values are recovered).
- Characters are deleted only by their owner and only below level 50; at most 4 per account.
- `world_server.json` gains `players_database`, the players.db path shared with the Login server.

**Changed**
- `world_server.exe` links Schemata and uses the dynamic runtime (`/MD`), like the Login server.
- `shared/` moves to SHARED-003 (game message envelope ids 0x0503 and 0x0504).

Summary: World serves character select from players.db (2026-10-27)
