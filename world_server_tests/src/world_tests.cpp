// world.db tests (WORLD-008 slice 1): the skill table, the legacy Skill.cfg rules world_import reads with, the row ->
// CSkill mapping the World loads with, and the CP949 <-> UTF-8 names between them.
#include "Skill.h"
#include "log.h"
#include "skill_legacy.h"
#include "test_runner.h"
#include "test_world.h"
#include "text_codec.h"
#include "world_database.h"

#include <cstring>
#include <filesystem>
#include <format>
#include <string>
#include <system_error>
#include <vector>

namespace tests
{
	namespace
	{
		using namespace hb::server::world;

		std::vector<char> bytes_of(const std::string& text)
		{
			return std::vector<char>(text.begin(), text.end());
		}

		void parser_tests(runner& t, const std::string& korean)
		{
			std::string error;
			// The file's own shape: a comment header, CRLF lines, tabs, a Korean name, negative values.
			const std::string file = "//-----Type--v1-v2---\r\nskill = 0  " + korean + "\t1    1  6  0 0 0 0 \r\nskill = 12 Alchemy 1 0 -3 0 0 0 7\r\n";
			skill_legacy::skill_table table;
			t.check(skill_legacy::parse_skill_file(bytes_of(file), table, error), std::format("Skill.cfg lines parse ({})", error));
			t.check(table[0] && table[12] && !table[1], "skills 0 and 12 defined, nothing else");
			if (table[0] && table[12])
			{
				t.check(std::memcmp(table[0]->m_cName, korean.data(), korean.size()) == 0 && table[0]->m_cName[korean.size()] == '\0',
					"Korean name kept as its CP949 bytes");
				t.check(table[0]->m_sType == 1 && table[0]->m_sValue1 == 1 && table[0]->m_sValue2 == 6 && table[0]->m_sValue6 == 0, "skill 0 values");
				t.check(std::strcmp(table[12]->m_cName, "Alchemy") == 0 && table[12]->m_sValue2 == -3 && table[12]->m_sValue6 == 7, "skill 12 values, negative kept");
			}

			t.check(!skill_legacy::parse_skill_file(bytes_of("skill = 3 A 1 1 1 1 1 1 1\nskill = 3 B 1 1 1 1 1 1 1\n"), table, error), "a repeated id is refused");
			t.check(!skill_legacy::parse_skill_file(bytes_of("skill = 60 A 1 1 1 1 1 1 1\n"), table, error), "an id past 59 is refused");
			t.check(!skill_legacy::parse_skill_file(bytes_of("skill = 1 ABCDEFGHIJKLMNOPQRSTU 1 1 1 1 1 1 1\n"), table, error), "a 21-byte name is refused");
			t.check(!skill_legacy::parse_skill_file(bytes_of("skill = 1 A 1 x 1 1 1 1 1\n"), table, error), "a non-number value is refused");
			t.check(!skill_legacy::parse_skill_file(bytes_of("skill = 1 A 1 1 1\n"), table, error), "an incomplete last line is refused");
		}

		void table_tests(runner& t, const std::filesystem::path& dir, const std::string& korean)
		{
			std::string error;
			world_database db;
			t.check(db.open((dir / "world.db").string(), 1, error), std::format("world.db creates its tables ({})", error));
			{
				world_database again;
				t.check(again.open((dir / "world.db").string(), 1, error), std::format("world.db reopens ({})", error));
			}

			// Import path: Skill.cfg -> CSkill -> row; World path: row -> CSkill. The two CSkill must be identical.
			const std::string file = "skill = 0 " + korean + " 1 1 6 0 0 0 0\nskill = 23 Taming 3 -1 2 3 4 5 32767\n";
			skill_legacy::skill_table legacy;
			t.check(skill_legacy::parse_skill_file(bytes_of(file), legacy, error), "legacy lines parse");
			std::vector<skill> rows;
			for (std::size_t id = 0; id < legacy.size(); ++id)
			{
				if (!legacy[id])
					continue;
				skill row;
				t.check(skill_legacy::from_legacy(static_cast<std::int64_t>(id), *legacy[id], row, error), std::format("skill {} to a row ({})", id, error));
				rows.push_back(row);
			}
			t.check(rows.size() == 2 && rows[0].name == std::string(reinterpret_cast<const char*>(u8"광물채취")), "the stored name is UTF-8");
			t.check(db.write_skills(rows, false, error), std::format("skills written ({})", error));
			t.check(!db.write_skills(rows, false, error) && error.find("--replace") != std::string::npos, "a second import without replace is refused");
			t.check(db.write_skills(rows, true, error), "replace rewrites the table");

			std::vector<skill> loaded;
			t.check(db.load_skills(loaded, error) && loaded.size() == 2 && loaded[0].id == 0 && loaded[1].id == 23, "skills load by id");
			bool identical = loaded.size() == 2;
			for (const skill& row : loaded)
			{
				CSkill object;
				identical = identical && skill_legacy::to_legacy(row, object, error) && skill_legacy::same(object, *legacy[static_cast<std::size_t>(row.id)]);
			}
			t.check(identical, "every skill loads into the same CSkill the legacy decoder made");

			std::vector<std::int64_t> missing;
			const std::vector<std::int64_t> asked{ 23, 5, 0, 5 };
			t.check(db.missing_skills(asked, missing, error) && missing == std::vector<std::int64_t>{ 5 }, "missing_skills names only the absent id, once");

			// Out-of-range values are refused by the table itself.
			skill bad;
			bad.id = 60;
			bad.name = "far";
			t.check(!db.write_skills({ bad }, true, error), "id 60 refused by CK_skill_id");
			bad.id = 1;
			bad.value3 = 40000;
			t.check(!db.write_skills({ bad }, true, error), "a value past a short refused");
			t.check(db.load_skills(loaded, error) && loaded.size() == 2, "refused writes left the table as it was");

			// A name that does not fit the game's buffer once back in CP949.
			skill wide;
			wide.id = 2;
			wide.name = reinterpret_cast<const char*>(u8"광광광광광광광광광광광");
			CSkill object;
			t.check(!skill_legacy::to_legacy(wide, object, error), "a name over 20 CP949 bytes is refused on load");
		}
	}

	void world_tests(runner& t, const std::filesystem::path& folder)
	{
		const std::filesystem::path dir = folder / "world_tests";
		std::error_code ignored;
		std::filesystem::remove_all(dir, ignored);
		std::filesystem::create_directories(dir);

		std::string korean;
		t.check(text::utf8_to_cp949(reinterpret_cast<const char*>(u8"광물채취"), korean) && korean.size() == 8, "UTF-8 -> CP949, 2 bytes per syllable");
		std::string back;
		t.check(text::cp949_to_utf8(korean, back) && back == std::string(reinterpret_cast<const char*>(u8"광물채취")), "CP949 -> UTF-8 round trip");
		t.check(!text::utf8_to_cp949("\xC3\x28", back), "invalid UTF-8 refused");

		parser_tests(t, korean);
		table_tests(t, dir, korean);
	}
}
