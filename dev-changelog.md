## Character tables in players.db

### SQL

**Added**
- The World's Schemata model for players.db: `characters` with `character_items`, `character_skills` and `character_magic`, linked by real FKs with cascade; `account_id` is a plain indexed column until Schemata SCH-0054 adds the FK to `accounts.id`.
- `database` syncs the tables and `character_store` saves and loads a character with its children in one transaction.
- Schemata `v0.1.0-rc.24` vendored in `third_party/schemata`.
- `world_server_tests` project, 39 checks.

Summary: World has character tables in the shared player database, not yet used by world_server.exe (2026-10-27)
