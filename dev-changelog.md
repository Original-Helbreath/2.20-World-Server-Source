## Connect the World to the Login server

### World Server

**Added**
- The World connects to the Login server's World link as an `hb::net::client`: it registers, heartbeats, reports status and reconnects after a loss; it exits when the Login server refuses its registration for good.
- New `world_server.json` (JSON with comments) holds the Login address and port, world name, link key, heartbeat interval and log level.
- The game data the log server used to push is read from `GameData\` at start-up.
- `third_party/nlohmann_json`, the same copy the Login server uses.

**Changed**
- Game activation waits for a registered Login link, and new players are refused while the link is down.
- Player-data requests to the old log server are not sent any more (`bSendMsgToLS` returns FALSE), so players cannot enter the game until the character work lands; saving logouts fall back to the local save.

**Removed**
- The legacy main-log, sub-log and gate sockets and their event handlers.

Summary: World Server registers with the Login server over the shared network layer and activates without the old log and gate servers (2026-10-27)
