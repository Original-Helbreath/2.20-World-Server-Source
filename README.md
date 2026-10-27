# 2.20-World-Server-Source

Source for the World server: every map in one process, all world duties, game data and player saving..

## Contents

Part of the two-binary recreation of the 2.20 servers (Login + World), written new in C++20 to the org
[Standards](https://github.com/Original-Helbreath/Standards). It replaces `2.20-Game-Server-*`, `2.20-Gate-Server-*` and the World Log server's game duties, which are kept as found in
[2.20-Reference](https://github.com/Original-Helbreath/2.20-Reference) (`as-found/servers/`). Empty until the
first ticket lands; the plan and its tickets are on the project board.

## Import (WORLD-001)

`world_server/src/` is the 2.20 game server copied from 2.20-Reference `as-found/servers/2.20-Game-Server-Source`.
Left out of the copy:

- `HGserver.ncb`, `HGserver.opt`, `HGserver.plg` - Visual C++ 6 IDE state and build log.
- `mssccprj.scc`, `vssver.scc` - Visual SourceSafe bindings.
- `README.md` - the archive repo's own readme.
- `mapdata/middleland.amd.old` - an old map backup; maps come from 2.20-Map-Files.

`HGserver.dsp` links `GameServerStrTok.lib` and `GameServerXsocket.lib`, which are not in the archive; their sources
`StrTok.cpp` and `XSocket.cpp` are compiled into the project instead.

## Branches

| Branch | Purpose |
|---|---|
| `development` | Integration branch. Work happens in feature branches named for their ticket, which merge into `development`. |
| `stable` | Default branch. `development` is merged here once it is stable. |

This repo has no `original` branch: it holds new code, not files transferred from elsewhere.

## Project

Part of [Original Helbreath](https://github.com/Original-Helbreath): keeping the original 1999-2005 Siementech Helbreath alive.
Repos are named `<version>-<Part>`; other versions follow the same pattern.
Issues are tracked on the [project board](https://github.com/orgs/Original-Helbreath/projects/1). The analysis behind everything is in
[2.20-Documentation](https://github.com/Original-Helbreath/2.20-Documentation).
