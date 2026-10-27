## Foreign key from characters to accounts

### SQL

**Added**
- `characters.account_id` references the Login server's `accounts.id` with cascade on delete; `accounts` stays unmapped and `drop_extra_tables` stays off.
- If the World starts before `accounts` exists, the key is deferred with a warning and added on the next start.

**Changed**
- Schemata v0.1.0-rc.25 (SCH-0054) vendored.

Summary: Characters are tied to their account by a real foreign key (2026-10-27)
