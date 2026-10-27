#include "character_store.h"

#include "log.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <exception>
#include <format>
#include <optional>
#include <stdexcept>

namespace hb::server::world
{
	std::string character_store::name_key(std::string_view name)
	{
		std::string key(name);
		std::ranges::transform(key, key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return key;
	}

	bool character_store::exists(std::string_view name, std::string& error)
	{
		error.clear();
		try
		{
			schemata::ConnectionPool::Lease lease = m_database.pool().acquire();
			return schemata::from<character>(*lease, m_database.model()).where(schemata::col(&character::name_key) == name_key(name)).count() > 0;
		}
		catch (const std::exception& failure)
		{
			error = failure.what();
			return false;
		}
	}

	load_result character_store::load(std::string_view name, character_record& record, std::string& error)
	{
		error.clear();
		try
		{
			schemata::ConnectionPool::Lease lease = m_database.pool().acquire();
			const schemata::Model& model = m_database.model();
			std::optional<character> found = schemata::from<character>(*lease, model).where(schemata::col(&character::name_key) == name_key(name)).first();
			if (!found)
				return load_result::not_found;
			const std::int64_t id = found->id;
			record = character_record{};
			record.header = std::move(*found);
			record.items = schemata::from<character_item>(*lease, model).where(schemata::col(&character_item::character_id) == id).to_vector();
			record.skills = schemata::from<character_skill>(*lease, model).where(schemata::col(&character_skill::character_id) == id).to_vector();
			record.magic = schemata::from<character_magic>(*lease, model).where(schemata::col(&character_magic::character_id) == id).to_vector();
			std::ranges::sort(record.items, [](const character_item& a, const character_item& b) {
				return a.in_bank != b.in_bank ? !a.in_bank : a.slot < b.slot;
			});
			std::ranges::sort(record.skills, [](const character_skill& a, const character_skill& b) { return a.skill_id < b.skill_id; });
			std::ranges::sort(record.magic, [](const character_magic& a, const character_magic& b) { return a.magic_id < b.magic_id; });
			return load_result::loaded;
		}
		catch (const std::exception& failure)
		{
			error = failure.what();
			return load_result::failed;
		}
	}

	bool character_store::save(character_record& record, std::string& error)
	{
		error.clear();
		try
		{
			const schemata::timestamp now = std::chrono::floor<std::chrono::microseconds>(std::chrono::system_clock::now());
			character header = record.header;
			header.name_key = name_key(header.name);
			header.saved_at = now;
			schemata::Context ctx(m_database.pool().acquire(), m_database.model());
			ctx.transaction([&] {
				if (header.id == 0)
				{
					header.created_at = now;
					ctx.add(header);
					ctx.save_changes();
				}
				else
				{
					std::optional<character> stored = ctx.query<character>().where(schemata::col(&character::id) == header.id).first();
					if (!stored)
						throw std::runtime_error(std::format("character {} is not stored", header.id));
					header.created_at = stored->created_at;
					ctx.track(*stored);
					*stored = header;
					ctx.save_changes();
				}
				const std::int64_t id = header.id;
				ctx.query<character_item>().where(schemata::col(&character_item::character_id) == id).remove();
				ctx.query<character_skill>().where(schemata::col(&character_skill::character_id) == id).remove();
				ctx.query<character_magic>().where(schemata::col(&character_magic::character_id) == id).remove();
				std::vector<character_item> items = record.items;
				std::vector<character_skill> skills = record.skills;
				std::vector<character_magic> magic = record.magic;
				for (character_item& row : items)
				{
					row.id = 0;
					row.character_id = id;
				}
				for (character_skill& row : skills)
				{
					row.id = 0;
					row.character_id = id;
				}
				for (character_magic& row : magic)
				{
					row.id = 0;
					row.character_id = id;
				}
				if (!items.empty())
					ctx.insert_many(items);
				if (!skills.empty())
					ctx.insert_many(skills);
				if (!magic.empty())
					ctx.insert_many(magic);
			});
			record.header = header;
			return true;
		}
		catch (const std::exception& failure)
		{
			error = failure.what();
			return false;
		}
	}

	bool character_store::remove(std::string_view name, std::string& error)
	{
		error.clear();
		try
		{
			schemata::ConnectionPool::Lease lease = m_database.pool().acquire();
			return schemata::from<character>(*lease, m_database.model()).where(schemata::col(&character::name_key) == name_key(name)).remove() > 0;
		}
		catch (const std::exception& failure)
		{
			error = failure.what();
			return false;
		}
	}
}
