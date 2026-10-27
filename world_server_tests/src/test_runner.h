#pragma once

#include "log.h"

#include <filesystem>
#include <string_view>

namespace tests
{
	class runner
	{
	public:
		void check(bool condition, std::string_view name)
		{
			++m_total;
			if (condition)
				return;
			++m_failed;
			hb::log::error("FAIL: {}", name);
		}

		bool passed() const
		{
			return m_failed == 0;
		}

		int total() const
		{
			return m_total;
		}

		int failed() const
		{
			return m_failed;
		}

	private:
		int m_total = 0;
		int m_failed = 0;
	};

	// character_tests.cpp: the character tables against a throwaway players.db that already holds the Login's accounts
	void character_tests(runner& t, const std::filesystem::path& folder);

	// character_service_tests.cpp: list, create and delete answers, new-character defaults, summaries, envelope ids
	void character_service_tests(runner& t, const std::filesystem::path& folder);
}
