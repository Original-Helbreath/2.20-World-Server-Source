// Character request tests (WORLD-006): the list, create and delete answers the World gives the Login server, the
// 2.20 first-entry defaults of a new character, and the select-screen summary. Also the SHARED-003 envelope ids.
#include "character_service.h"
#include "character_store.h"
#include "database.h"
#include "field_checks.h"
#include "message_codec.h"
#include "message_id.h"
#include "test_runner.h"

#include <cstddef>
#include <filesystem>
#include <format>
#include <span>
#include <string>
#include <system_error>
#include <vector>

namespace tests
{
	namespace
	{
		using namespace hb::server::world;
		using hb::net::login::character_result;

		hb::net::link::create_character_request create_request(std::uint64_t account, std::string_view name, std::uint8_t sex = 1)
		{
			hb::net::link::create_character_request r{};
			r.request_id = 7;
			r.account_id = account;
			(void)r.name.assign(name);
			r.sex = sex;
			r.skin = 2;
			r.hair_style = 3;
			r.hair_colour = 5;
			r.underwear_colour = 6;
			r.stats = { 14, 12, 10, 12, 12, 10 };  // str vit dex int mag chr, 70 in all
			return r;
		}

		hb::net::link::delete_character_request delete_request(std::uint64_t account, std::string_view name)
		{
			hb::net::link::delete_character_request r{};
			r.request_id = 9;
			r.account_id = account;
			(void)r.name.assign(name);
			return r;
		}

		void envelope_tests(runner& t)
		{
			using namespace hb::net;
			t.check(to_wire(message_id::world_game_message) == 0x0503, "world_game_message is 0x0503");
			t.check(to_wire(message_id::client_game_message) == 0x0504, "client_game_message is 0x0504");
			t.check(direction_of(0x0503) == message_direction::client_to_world, "0x0503 is client -> World");
			t.check(direction_of(0x0504) == message_direction::world_to_client, "0x0504 is World -> client");
			t.check(direction_of(0x0501) == message_direction::client_to_world, "0x0501 still client -> World");
			t.check(direction_of(0x0502) == message_direction::world_to_client, "0x0502 still World -> client");
			t.check(!is_known(0x0505), "0x0505 is not defined");
		}

		void rule_tests(runner& t)
		{
			t.check(check_new_character(create_request(1, "Hero1")) == character_result::ok, "a well-formed request passes");
			t.check(check_new_character(create_request(1, "")) == character_result::invalid_name, "empty name refused");
			t.check(check_new_character(create_request(1, "Hero_1")) == character_result::invalid_name, "underscore refused");
			t.check(check_new_character(create_request(1, "ElevenChars")) == character_result::invalid_name, "11 characters refused");
			auto stats = create_request(1, "Hero1");
			stats.stats.strength = 15;
			stats.stats.charisma = 9;
			t.check(check_new_character(stats) == character_result::invalid_stats, "stat outside 10..14 refused");
			stats.stats = { 14, 14, 14, 14, 14, 10 };
			t.check(check_new_character(stats) == character_result::invalid_stats, "stats not adding up to 70 refused");
			auto sex = create_request(1, "Hero1", 3);
			t.check(check_new_character(sex) == character_result::failed, "sex 3 refused");
			auto hair = create_request(1, "Hero1");
			hair.hair_colour = 16;
			t.check(check_new_character(hair) == character_result::failed, "hair colour over 15 refused");
		}

		void default_tests(runner& t)
		{
			const character_record male = make_new_character(create_request(5, "Hero1", 1));
			const character& c = male.header;
			t.check(c.account_id == 5 && c.name == "Hero1" && c.name_key == "hero1", "new character: account, name and key");
			t.check(c.map_name == "default" && c.x == -1 && c.y == -1, "new character starts on default at -1,-1");
			t.check(c.level == 1 && c.luck == 10 && c.exp == 0, "new character: level 1, luck 10, no experience");
			t.check(c.guild_name == "NONE" && c.guild_rank == -1 && c.home_location == "NONE", "new character: no guild, no home");
			t.check(c.strength == 14 && c.vitality == 12 && c.dexterity == 10 && c.intelligence == 12 && c.magic == 12 && c.charisma == 10,
				"new character keeps the requested stats");
			t.check(c.appr1 == ((3 << 8) | (5 << 4) | 6), "appr1 packs hair style, hair colour and underwear");
			t.check(c.hp == 12 * 3 + 2 + 14 / 2 && c.mp == 12 * 2 + 2 + 12 / 2 && c.sp == 14 * 2 + 2, "HP, MP and SP at the game's maximum");
			t.check(male.items.size() == 7 && male.skills.empty() && male.magic.empty(), "starting kit of seven items, no skills or magic");
			t.check(male.items[0].name == "Dagger" && male.items[5].name == "KneeTrousers(M)" && male.items[6].name == "Shirt(M)",
				"male kit names");
			t.check(male.items[5].equipped && male.items[6].equipped && !male.items[0].equipped, "male wears trousers and shirt");
			t.check(male.items[0].cur_life_span == 300 && male.items[0].count == 1, "kit items: count 1, life span 300");

			const character_record female = make_new_character(create_request(5, "Hera1", 2));
			t.check(female.items[6].name == "Chemise(W)" && !female.items[6].equipped && female.items[5].equipped,
				"female kit: chemise carried, trousers worn");
		}

		void summary_tests(runner& t)
		{
			character c;
			c.name = "Hero1";
			c.sex = 2;
			c.skin = 3;
			c.level = 42;
			c.exp = 123456;
			c.strength = 20;
			c.appr1 = 0x0356;
			c.appr2 = 0x0AB7;
			c.appr3 = 0x1234;
			c.appr4 = 0x5680;
			c.appr_color = static_cast<std::int32_t>(0x12345678);
			c.map_name = "aresden";
			c.saved_at = schemata::timestamp(std::chrono::sys_days(std::chrono::year(2026) / 10 / 27) + std::chrono::hours(13) + std::chrono::minutes(5));
			const hb::net::character_summary s = to_summary(c);
			t.check(s.name.view() == "Hero1" && s.map_name.view() == "aresden" && s.is_valid == 1, "summary name, map and valid flag");
			t.check(s.sex == 2 && s.skin == 3 && s.level == 42 && s.experience == 123456 && s.stats.strength == 20, "summary basics");
			const hb::net::character_appearance& a = s.appearance;
			t.check(a.hair_style == 3 && a.hair_colour == 5 && a.underwear_colour == 6, "appr1 unpacked");
			t.check(a.weapon == 0xAB && a.shield == 7, "appr2 unpacked");
			t.check(a.body_armour == 1 && a.pants == 2 && a.helm == 3 && a.arm_armour == 4, "appr3 unpacked");
			t.check(a.boots == 5 && a.mantle == 6 && a.hides_body_armour == 1, "appr4 unpacked");
			t.check(a.weapon_colour == 1 && a.shield_colour == 2 && a.helm_colour == 8, "appr_color unpacked");
			t.check(s.last_saved.year == 2026 && s.last_saved.month == 10 && s.last_saved.day == 27 && s.last_saved.hour == 13
				&& s.last_saved.minute == 5, "last saved stamp");
			t.check(hb::net::is_well_formed(s), "summary is well formed");
		}

		void service_tests(runner& t, const std::filesystem::path& folder)
		{
			const std::filesystem::path dir = folder / "character_service_tests";
			std::error_code ignored;
			std::filesystem::remove_all(dir, ignored);
			std::filesystem::create_directories(dir);
			std::string error;
			database db;
			t.check(db.open((dir / "players.db").string(), 2, error), std::format("players.db opens ({})", error));
			character_store store(db);
			character_service service(store);

			t.check(service.list(11).summaries.empty() && service.list(11).max_slots == max_character_slots, "empty list, four slots");
			t.check(service.create(create_request(11, "Alpha")) == character_result::ok, "create Alpha");
			t.check(service.create(create_request(12, "alpha")) == character_result::name_taken, "name taken in any case");
			t.check(service.create(create_request(11, "Bad Name")) == character_result::invalid_name, "invalid name from the World");
			for (const std::string_view name : { "Beta", "Gamma", "Delta" })
				t.check(service.create(create_request(11, name)) == character_result::ok, std::format("create {}", name));
			t.check(service.create(create_request(11, "Epsilon")) == character_result::slots_full, "fifth character refused");

			const character_list_answer listed = service.list(11);
			t.check(listed.summaries.size() == 4 && listed.summaries[0].name.view() == "Alpha" && listed.summaries[3].name.view() == "Delta",
				"list holds the four, oldest first");
			t.check(service.list(12).summaries.empty(), "another account sees none of them");

			hb::net::link::character_list_response head{};
			head.request_id = 3;
			head.max_slots = listed.max_slots;
			head.count = static_cast<std::uint8_t>(listed.summaries.size());
			std::vector<std::byte> bytes;
			t.check(hb::net::encode_character_list(head, std::span<const hb::net::character_summary>(listed.summaries), 65536, bytes),
				"the list encodes as a character_list_response");

			character_record loaded;
			t.check(store.load("alpha", loaded, error) == load_result::loaded && loaded.items.size() == 7, "Alpha stored with its kit");

			t.check(service.remove(delete_request(12, "Alpha")) == character_result::no_such_character, "another account cannot delete it");
			t.check(service.remove(delete_request(11, "Nobody")) == character_result::no_such_character, "unknown name");
			character_record veteran;
			t.check(store.load("Beta", veteran, error) == load_result::loaded, "load Beta");
			veteran.header.level = 50;
			t.check(store.save(veteran, error), "Beta raised to level 50");
			t.check(service.remove(delete_request(11, "Beta")) == character_result::level_too_high, "level 50 cannot be deleted");
			t.check(service.remove(delete_request(11, "ALPHA")) == character_result::ok, "owner deletes Alpha, any case");
			t.check(service.list(11).summaries.size() == 3, "three left");
			t.check(service.create(create_request(11, "Epsilon")) == character_result::ok, "a freed slot takes a new character");
		}
	}

	void character_service_tests(runner& t, const std::filesystem::path& folder)
	{
		envelope_tests(t);
		rule_tests(t);
		default_tests(t);
		summary_tests(t);
		service_tests(t, folder);
	}
}
