## Enter the World with a Login ticket

### World Server

**Added**
- Game clients enter the World with a Login ticket on the shared network layer, replacing the old XSocket listener; after the ticket, legacy game messages travel as SHARED-003 envelopes.
- The World holds the Login server's session grants until they are used or expire, and refuses a bad, reused or expired ticket.
- Characters load from and save to players.db with the old character-file load rules; players are saved on logout, auto-save and shutdown.
- The Login server is told when each session starts and ends; the World acts on `kick_account`, and a re-entering account replaces its old session before the World-full check.
- Connections that have not entered yet are capped at 32 in total and 2 per address; a game message too short for its id and type, or longer than the legacy buffer, closes the connection.
- The `default` map, where new characters start, is loaded.

**Fixed**
- The game never started because it waited for the retired World Log server.

Summary: Players enter the World with a Login ticket, and their characters load from and save to players.db (2026-10-27)
