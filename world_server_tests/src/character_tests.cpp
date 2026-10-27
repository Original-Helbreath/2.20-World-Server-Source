// Character table tests (WORLD-004): the World's tables sync into a players.db that already holds the Login server's
// accounts without touching them, and a character with bag and bank items, skills and magic saves and loads whole.
#include "character_store.h"
#include "database.h"
#include "entities.h"
#include "log.h"
#include "login_tables.h"
#include "test_runner.h"

#include <schemata/sqlite.h>

#include <chrono>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>

namespace tests
{
	namespace
	{
		using namespace hb::server::world;

		// Every stored value of a row except the key columns and timestamps, as a tuple of references, so one list
		// serves both filling a row with distinct values and comparing two rows.
		auto fields(character& c)
		{
			return std::tie(c.account_id, c.name, c.name_key, c.profile, c.home_location, c.guild_name, c.guild_guid, c.guild_rank,
				c.map_name, c.x, c.y, c.hp, c.mp, c.sp, c.level, c.rating, c.strength, c.intelligence, c.vitality, c.dexterity, c.magic,
				c.charisma, c.luck, c.exp, c.lu_strength, c.lu_vitality, c.lu_dexterity, c.lu_intelligence, c.lu_magic, c.lu_charisma,
				c.enemy_kill_count, c.pk_count, c.reward_gold, c.down_skill_index, c.id_num1, c.id_num2, c.id_num3, c.sex, c.skin,
				c.hair_style, c.hair_color, c.underwear, c.appr1, c.appr2, c.appr3, c.appr4, c.appr_color, c.hunger_status,
				c.time_left_shut_up, c.time_left_rating, c.time_left_force_recall, c.time_left_firm_staminar, c.admin_user_level,
				c.penalty_block_year, c.penalty_block_month, c.penalty_block_day, c.quest, c.quest_id, c.current_quest_count,
				c.quest_reward_type, c.quest_reward_amount, c.quest_completed, c.contribution, c.war_contribution, c.special_event_id,
				c.super_attack_left, c.fightzone_number, c.reserve_time, c.fightzone_ticket_number, c.special_ability_time,
				c.locked_map_name, c.locked_map_time, c.crusade_duty, c.crusade_guid, c.construction_point, c.dead_penalty_time,
				c.party_id, c.gizon_item_upgrade_left);
		}

		auto fields(character_item& i)
		{
			return std::tie(i.in_bank, i.slot, i.name, i.count, i.touch_effect_type, i.touch_effect_value1, i.touch_effect_value2,
				i.touch_effect_value3, i.color, i.spec_effect_value1, i.spec_effect_value2, i.spec_effect_value3, i.cur_life_span,
				i.attribute, i.equipped, i.pos_x, i.pos_y);
		}

		auto fields(character_skill& s)
		{
			return std::tie(s.skill_id, s.mastery, s.ssn);
		}

		auto fields(character_magic& m)
		{
			return std::tie(m.magic_id, m.mastery);
		}

		// Gives every field a different value; `seed` separates one row from the next.
		template <typename Row>
		void fill(Row& row, int seed)
		{
			int next = seed * 1000;
			std::apply([&next](auto&... value) {
				([&next](auto& field) {
					using type = std::remove_cvref_t<decltype(field)>;
					++next;
					if constexpr (std::is_same_v<type, bool>)
						field = (next % 2) == 0;
					else if constexpr (std::is_same_v<type, std::string>)
						field = std::format("v{}", next);
					else
						field = static_cast<type>(next);
				}(value), ...);
			}, fields(row));
		}

		template <typename Row>
		bool same(Row a, Row b)
		{
			return fields(a) == fields(b);
		}

		bool same_record(character_record a, character_record b)
		{
			if (!same(a.header, b.header) || a.items.size() != b.items.size() || a.skills.size() != b.skills.size() || a.magic.size() != b.magic.size())
				return false;
			for (std::size_t i = 0; i < a.items.size(); ++i)
				if (!same(a.items[i], b.items[i]))
					return false;
			for (std::size_t i = 0; i < a.skills.size(); ++i)
				if (!same(a.skills[i], b.skills[i]))
					return false;
			for (std::size_t i = 0; i < a.magic.size(); ++i)
				if (!same(a.magic[i], b.magic[i]))
					return false;
			return true;
		}

		// A full character: a name that is not lowercase, 50 bag items (every fifth worn) and 20 bank items, 9 skills and
		// 12 spells, every value distinct.
		character_record make_character(std::string_view name, std::int64_t account_id)
		{
			character_record record;
			fill(record.header, 1);
			record.header.name = std::string(name);
			record.header.account_id = account_id;
			record.header.guild_guid = 77;
			record.header.guild_rank = 2;
			for (int slot = 0; slot < 50; ++slot)
			{
				character_item item;
				fill(item, 2 + slot);
				item.in_bank = false;
				item.slot = slot;
				item.name = std::format("Item{}", slot);
				item.equipped = slot % 5 == 0;
				record.items.push_back(item);
			}
			for (int slot = 0; slot < 20; ++slot)
			{
				character_item item;
				fill(item, 100 + slot);
				item.in_bank = true;
				item.slot = slot;
				item.name = std::format("Bank{}", slot);
				item.equipped = false;
				record.items.push_back(item);
			}
			for (int skill = 0; skill < 9; ++skill)
			{
				character_skill row;
				fill(row, 200 + skill);
				row.skill_id = skill * 6;
				record.skills.push_back(row);
			}
			for (int spell = 0; spell < 12; ++spell)
			{
				character_magic row;
				fill(row, 300 + spell);
				row.magic_id = spell * 3;
				record.magic.push_back(row);
			}
			return record;
		}

		std::int64_t count_of(database& db, const char* table)
		{
			schemata::ConnectionPool::Lease lease = db.pool().acquire();
			schemata::Context ctx(std::move(lease), db.model());
			return ctx.count_rows(table);
		}
	}

	namespace
	{
		// True when the live characters table holds the key to accounts(id) with ON DELETE CASCADE.
		bool has_account_key(const std::string& path)
		{
			std::unique_ptr<schemata::driver::Connection> connection = schemata::sqlite(path);
			const schemata::SchemaSnapshot live = schemata::read_schema(*connection);
			for (const schemata::SnapshotTable& table : live.tables)
			{
				if (table.name != "characters")
					continue;
				for (const schemata::SnapshotForeignKey& key : table.foreign_keys)
					if (key.referenced_table == "accounts" && key.columns == std::vector<std::string>{ "account_id" }
						&& key.referenced_columns == std::vector<std::string>{ "id" } && key.on_delete == schemata::ReferentialAction::cascade)
						return true;
			}
			return false;
		}

		std::int64_t add_account(schemata::driver::Connection& connection, schemata::Model& model, std::string_view name)
		{
			schemata::Context login(connection, model);
			login_account account;
			account.name = std::string(name);
			account.name_key = std::string(name);
			account.password_hash = "h";
			account.password_salt = "s";
			account.created_at = std::chrono::floor<std::chrono::microseconds>(std::chrono::system_clock::now());
			login.add(account);
			login.save_changes();
			return account.id;
		}

		bool save_new(character_store& store, character_record record, std::string& error)
		{
			return store.save(record, error);
		}

		bool save_refused(character_store& store, character_record record)
		{
			std::string error;
			return !store.save(record, error) && !error.empty();
		}

		// WORLD-007: characters.account_id references the Login server's accounts(id) and cascades on delete, in a players.db
		// both models share, with the World starting after the Login server and before it.
		void account_key_tests(runner& t, const std::filesystem::path& folder)
		{
			std::string error;

			// Login first: the key is added by the World's first start.
			{
				const std::filesystem::path dir = folder / "account_key_tests" / "login_first";
				std::error_code ignored;
				std::filesystem::remove_all(dir, ignored);
				std::filesystem::create_directories(dir);
				const std::string path = (dir / "players.db").string();
				schemata::Model login_model;
				login_model.add<login_account>();
				std::unique_ptr<schemata::driver::Connection> login = schemata::sqlite(path);
				t.check(schemata::ensure_schema(*login, login_model).applied > 0, "login first: accounts created");
				const std::int64_t keep = add_account(*login, login_model, "keep");
				const std::int64_t gone = add_account(*login, login_model, "gone");

				database db;
				t.check(db.open(path, 2, error) && db.deferred_foreign_keys() == 0, std::format("login first: world opens, nothing deferred ({})", error));
				t.check(has_account_key(path), "login first: characters has the key to accounts(id), cascade on delete");
				character_store store(db);
				t.check(save_refused(store, make_character("Ghost", gone + 1000)), "character for a missing account refused");
				t.check(!store.exists("Ghost", error), "refused character left no row");
				t.check(save_new(store, make_character("Kept", keep), error), std::format("character for an existing account saves ({})", error));
				t.check(save_new(store, make_character("Gone1", gone), error) && save_new(store, make_character("Gone2", gone), error), "two characters for the doomed account");
				t.check(count_of(db, "characters") == 3 && count_of(db, "character_items") == 210, "three characters, 210 items");

				schemata::from<login_account>(*login, login_model).where(schemata::col(&login_account::id) == gone).remove();
				t.check(count_of(db, "characters") == 1 && count_of(db, "character_items") == 70 && count_of(db, "character_skills") == 9 && count_of(db, "character_magic") == 12,
					"deleting the account cascades to its characters and their items, skills and magic");
				t.check(store.exists("Kept", error) && !store.exists("Gone1", error) && !store.exists("Gone2", error), "only the other account's character is left");

				database next;
				t.check(next.open(path, 1, error) && next.deferred_foreign_keys() == 0 && has_account_key(path), "a later start keeps the key");
				t.check(schemata::ensure_schema(*login, login_model).applied == 0, "the login model still has nothing to apply");
			}

			// World first: the key is deferred, then added by the next start, with a row already waiting on it.
			{
				const std::filesystem::path dir = folder / "account_key_tests" / "world_first";
				std::error_code ignored;
				std::filesystem::remove_all(dir, ignored);
				std::filesystem::create_directories(dir);
				const std::string path = (dir / "players.db").string();

				database first;
				t.check(first.open(path, 2, error), std::format("world first: opens with no accounts table ({})", error));
				t.check(first.deferred_foreign_keys() == 1, std::format("world first: the key is deferred ({} deferred)", first.deferred_foreign_keys()));
				t.check(!has_account_key(path), "world first: no key yet");
				character_store store(first);
				t.check(save_new(store, make_character("Early", 1), error), std::format("world first: a character saves while the key waits ({})", error));

				schemata::Model login_model;
				login_model.add<login_account>();
				std::unique_ptr<schemata::driver::Connection> login = schemata::sqlite(path);
				t.check(schemata::ensure_schema(*login, login_model).applied > 0, "world first: the Login server then creates accounts");
				t.check(add_account(*login, login_model, "early") == 1, "world first: account 1 exists");

				database second;
				t.check(second.open(path, 2, error), std::format("world first: second start ({})", error));
				t.check(second.deferred_foreign_keys() == 0 && has_account_key(path), "world first: the second start adds the key");
				character_store later(second);
				t.check(later.exists("Early", error) && count_of(second, "character_items") == 70, "world first: the waiting character survived the rebuild");
				t.check(save_refused(later, make_character("Ghost", 99)), "world first: a missing account is now refused");

				database third;
				t.check(third.open(path, 1, error) && third.deferred_foreign_keys() == 0 && has_account_key(path), "world first: a third start changes nothing");
				schemata::from<login_account>(*login, login_model).where(schemata::col(&login_account::id) == 1).remove();
				t.check(count_of(second, "characters") == 0 && count_of(second, "character_items") == 0, "world first: deleting the account removes the character");
			}
		}
	}

	void character_tests(runner& t, const std::filesystem::path& folder)
	{
		const std::filesystem::path dir = folder / "character_tests";
		std::error_code ignored;
		std::filesystem::remove_all(dir, ignored);
		std::filesystem::create_directories(dir);
		const std::string path = (dir / "players.db").string();
		std::string error;

		// The Login server went first: its accounts table exists with rows in it.
		schemata::Model login_model;
		login_model.add<login_account>();
		auto login_pool = [&] { return schemata::sqlite(path); };
		std::int64_t bob_id = 0;
		{
			std::unique_ptr<schemata::driver::Connection> login_connection_owner = login_pool();
			schemata::driver::Connection& login_connection = *login_connection_owner;
			t.check(schemata::ensure_schema(login_connection, login_model).applied > 0, "login model creates accounts");
			schemata::Context login(login_connection, login_model);
			login_account bob;
			bob.name = "Bob";
			bob.name_key = "bob";
			bob.password_hash = "h";
			bob.password_salt = "s";
			bob.created_at = std::chrono::floor<std::chrono::microseconds>(std::chrono::system_clock::now());
			login.add(bob);
			login.save_changes();
			t.check(bob.id > 0, "account row stored");
			bob_id = bob.id;
		}

		database db;
		t.check(db.open(path, 2, error), std::format("world model syncs into the shared players.db ({})", error));
		t.check(count_of(db, "characters") == 0 && count_of(db, "character_items") == 0 && count_of(db, "character_skills") == 0 && count_of(db, "character_magic") == 0,
			"four empty character tables");
		{
			std::unique_ptr<schemata::driver::Connection> login_connection_owner = login_pool();
			schemata::driver::Connection& login_connection = *login_connection_owner;
			schemata::Context login(login_connection, login_model);
			const schemata::EnsureResult after = schemata::ensure_schema(login_connection, login_model);
			t.check(after.applied == 0, std::format("accounts untouched by the world sync ({} statements applied to the login model)", after.applied));
			t.check(login.count_rows("accounts") == 1, "account row survived the world sync");
		}
		database again;
		t.check(again.open(path, 1, error), std::format("world model reopens ({})", error));

		character_store store(db);
		t.check(!store.exists("Aria", error) && error.empty(), "no character yet");
		character_record missing;
		t.check(store.load("Aria", missing, error) == load_result::not_found, "unknown character is not_found");

		t.check(db.deferred_foreign_keys() == 0, "accounts existed: no foreign key deferred");

		// Round trip: the account id must name an existing account (foreign key to the Login's accounts, WORLD-007).
		character_record aria = make_character("Aria", bob_id);
		t.check(store.save(aria, error), std::format("insert saves ({})", error));
		t.check(aria.header.id > 0, "insert sets the id");
		t.check(aria.header.name_key == "aria", "name key is lowercase");
		t.check(count_of(db, "characters") == 1 && count_of(db, "character_items") == 70 && count_of(db, "character_skills") == 9 && count_of(db, "character_magic") == 12,
			"1 character, 70 items, 9 skills and 12 spells stored");
		t.check(store.exists("ARIA", error), "exists finds any case");

		character_record loaded;
		t.check(store.load("aRiA", loaded, error) == load_result::loaded, std::format("load any case ({})", error));
		t.check(loaded.header.id == aria.header.id && loaded.header.created_at != schemata::timestamp{} && loaded.header.saved_at != schemata::timestamp{},
			"load keeps the id and stamps");
		t.check(same(loaded.header, aria.header), "header round-trips (every column)");
		t.check(loaded.items.size() == 70 && loaded.skills.size() == 9 && loaded.magic.size() == 12, "child rows loaded");
		t.check(same_record(loaded, aria), "character, items, skills and magic round-trip");
		t.check(!loaded.items.front().in_bank && loaded.items[49].slot == 49 && loaded.items[50].in_bank && loaded.items.back().slot == 19, "items come back bag first, by slot");

		// Update: change values, drop an item, add an item and a skill, forget a spell.
		const schemata::timestamp created = loaded.header.created_at;
		loaded.header.level = 99;
		loaded.header.map_name = "aresden";
		loaded.header.quest_completed = !loaded.header.quest_completed;
		loaded.items.erase(loaded.items.begin() + 3);
		loaded.items[0].count = 5000000000LL;
		character_item extra;
		fill(extra, 900);
		extra.in_bank = true;
		extra.slot = 20;
		extra.name = "Extra";
		loaded.items.push_back(extra);
		character_skill newest;
		fill(newest, 901);
		newest.skill_id = 59;
		loaded.skills.push_back(newest);
		loaded.magic.pop_back();
		t.check(store.save(loaded, error), std::format("update saves ({})", error));
		character_record updated;
		t.check(store.load("Aria", updated, error) == load_result::loaded, "reload after update");
		t.check(count_of(db, "characters") == 1, "update keeps one character row");
		t.check(updated.header.id == aria.header.id && updated.header.created_at == created, "update keeps id and creation time");
		t.check(updated.items.size() == 70 && updated.skills.size() == 10 && updated.magic.size() == 11, "children replaced (70 items, 10 skills, 11 spells)");
		t.check(same_record(updated, loaded), "updated character round-trips, with a 64-bit item count");
		t.check(count_of(db, "character_items") == 70 && count_of(db, "character_skills") == 10 && count_of(db, "character_magic") == 11, "no orphaned old child rows");

		// A second character is independent.
		character_record bard = make_character("Bard", bob_id);
		t.check(store.save(bard, error), "second character saves");
		t.check(count_of(db, "characters") == 2 && count_of(db, "character_items") == 140, "two characters, 140 items");
		character_record bard_loaded;
		t.check(store.load("bard", bard_loaded, error) == load_result::loaded && same_record(bard_loaded, bard), "second character round-trips");

		// The name is the unique key.
		character_record twin = make_character("ARIA", bob_id);
		t.check(!store.save(twin, error) && !error.empty(), "duplicate name (any case) refused");
		t.check(count_of(db, "characters") == 2 && count_of(db, "character_items") == 140, "refused insert left nothing behind");

		// A failing child row rolls the whole save back.
		character_record broken = updated;
		broken.header.level = 1;
		broken.skills.push_back(broken.skills.front());
		t.check(!store.save(broken, error), "duplicate skill row refused");
		character_record after_rollback;
		t.check(store.load("Aria", after_rollback, error) == load_result::loaded && after_rollback.header.level == 99 && same_record(after_rollback, updated),
			"failed save rolled back, nothing half written");

		// Real foreign keys between the World's own tables.
		{
			schemata::ConnectionPool::Lease lease = db.pool().acquire();
			schemata::Context ctx(std::move(lease), db.model());
			character_magic orphan;
			orphan.character_id = 987654;
			orphan.magic_id = 1;
			bool refused = false;
			try
			{
				ctx.add(orphan);
				ctx.save_changes();
			}
			catch (const std::exception&)
			{
				refused = true;
			}
			t.check(refused, "child row for a missing character refused by its foreign key");
		}
		t.check(store.remove("ARIA", error), "character removed");
		t.check(count_of(db, "characters") == 1 && count_of(db, "character_items") == 70 && count_of(db, "character_skills") == 9 && count_of(db, "character_magic") == 12,
			"removing a character cascades to its items, skills and magic only");
		t.check(store.load("aria", missing, error) == load_result::not_found, "removed character is gone");

		// Still alongside the Login's table.
		{
			std::unique_ptr<schemata::driver::Connection> login_connection_owner = login_pool();
			schemata::driver::Connection& login_connection = *login_connection_owner;
			schemata::Context login(login_connection, login_model);
			t.check(schemata::ensure_schema(login_connection, login_model).applied == 0 && login.count_rows("accounts") == 1, "accounts still untouched after all World writes");
		}

		account_key_tests(t, folder);
	}
}
