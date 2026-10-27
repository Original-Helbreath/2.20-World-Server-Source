#include "world_data.h"

#include "log.h"
#include "template_references.h"
#include "world_database.h"

#include <memory>

namespace hb::server::world
{
	namespace
	{
		// The host loop and the character store's reference checks are the only users.
		constexpr std::size_t pool_size = 2;

		std::unique_ptr<world_database> g_database;
		std::unique_ptr<template_references> g_references;
		std::vector<skill> g_skills;
	}

	bool open_world_data(const std::string& world_database_path)
	{
		close_world_data();
		auto opened = std::make_unique<world_database>();
		std::string error;
		if (!opened->open(world_database_path, pool_size, error))
		{
			hb::log::error("cannot open world database: {}", error);
			return false;
		}
		std::vector<skill> skills;
		if (!opened->load_skills(skills, error))
		{
			hb::log::error("cannot read skills from {}: {}", world_database_path, error);
			return false;
		}
		if (skills.empty())
		{
			hb::log::error("{} holds no skills; build it with world_import", world_database_path);
			return false;
		}
		hb::log::info("{} skill(s) read from {}", skills.size(), world_database_path);
		g_database = std::move(opened);
		g_references = std::make_unique<template_references>(*g_database);
		g_skills = std::move(skills);
		return true;
	}

	void close_world_data()
	{
		g_skills.clear();
		g_references.reset();
		g_database.reset();
	}

	const std::vector<skill>& world_skills()
	{
		return g_skills;
	}

	const template_references* world_references()
	{
		return g_references.get();
	}
}
