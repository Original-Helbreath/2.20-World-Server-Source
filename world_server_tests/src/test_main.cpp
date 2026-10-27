// Tests for the World server's character tables in players.db, run against a throwaway database.
// Exit code 0 when every check passes.
#include "log.h"
#include "test_runner.h"

#if defined(_MSC_VER)
#include <crtdbg.h>
#include <cstdlib>
#endif

#include <exception>
#include <filesystem>

int main()
{
#if defined(_MSC_VER)
	// A failed assert or abort reports to stderr and exits; a headless test run never waits on a dialog.
	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
	for (const int kind : { _CRT_WARN, _CRT_ERROR, _CRT_ASSERT })
	{
		_CrtSetReportMode(kind, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
		_CrtSetReportFile(kind, _CRTDBG_FILE_STDERR);
		(void)kind;  // the two calls above compile to nothing in Release
	}
#endif
	hb::log::initialize("world_server_tests", "tests");
	tests::runner t;
	const std::filesystem::path folder = "test_data";
	try
	{
		tests::character_tests(t, folder);
		tests::character_service_tests(t, folder);
		tests::session_grant_tests(t);
		tests::player_mapping_tests(t);
	}
	catch (const std::exception& failure)
	{
		hb::log::report_crash(failure.what());
		return 1;
	}
	hb::log::info("{} of {} checks passed", t.total() - t.failed(), t.total());
	hb::log::shutdown();
	return t.passed() ? 0 : 1;
}
