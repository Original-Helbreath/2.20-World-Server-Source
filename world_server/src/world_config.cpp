#include "world_config.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <sstream>
#include <string_view>

namespace hb::server::world
{
	namespace
	{
		constexpr std::size_t link_key_hex_digits = 64;
		constexpr std::uint64_t min_port = 1;
		constexpr std::uint64_t min_heartbeat_seconds = 1;
		constexpr std::uint64_t max_heartbeat_seconds = 60;

		int hex_value(char digit)
		{
			if (digit >= '0' && digit <= '9')
				return digit - '0';
			if (digit >= 'a' && digit <= 'f')
				return digit - 'a' + 10;
			if (digit >= 'A' && digit <= 'F')
				return digit - 'A' + 10;
			return -1;
		}

		bool parse_link_key(std::string_view text, std::array<std::uint8_t, link_key_bytes>& key)
		{
			if (text.size() != link_key_hex_digits)
				return false;
			for (std::size_t i = 0; i < key.size(); ++i)
			{
				const int high = hex_value(text[i * 2]);
				const int low = hex_value(text[i * 2 + 1]);
				if (high < 0 || low < 0)
					return false;
				key[i] = static_cast<std::uint8_t>(high * 16 + low);
			}
			return true;
		}

		// Private to this file: it drags <nlohmann/json.hpp> into every includer otherwise.
		class config_reader
		{
		public:
			explicit config_reader(world_config& config)
				: m_config(config)
			{
			}

			bool read(const nlohmann::json& root)
			{
				allow_keys(root, { "login_address", "login_port", "world_name", "link_key", "heartbeat_seconds", "log_level", "players_database", "world_database" });
				read_level(root);
				read_string(root, "players_database", m_config.players_database);
				read_string(root, "world_database", m_config.world_database);
				read_string(root, "login_address", m_config.login_address);
				read_number(root, "login_port", m_config.login_port, min_port, std::numeric_limits<std::uint16_t>::max());
				read_number(root, "heartbeat_seconds", m_config.heartbeat_seconds, min_heartbeat_seconds, max_heartbeat_seconds);
				read_name(root);
				read_key(root);
				return m_error.empty();
			}

			const std::string& error() const
			{
				return m_error;
			}

		private:
			void fail(std::string message)
			{
				if (m_error.empty())
					m_error = std::move(message);
			}

			// A misspelt key would otherwise be silently ignored and its default used.
			void allow_keys(const nlohmann::json& object, std::initializer_list<std::string_view> keys)
			{
				for (const auto& [key, value] : object.items())
				{
					if (std::find(keys.begin(), keys.end(), key) == keys.end())
						fail(std::format("unknown key '{}'", key));
				}
			}

			void read_string(const nlohmann::json& parent, std::string_view key, std::string& value)
			{
				const auto found = parent.find(key);
				if (found == parent.end())
					return;
				if (!found->is_string() || found->get_ref<const std::string&>().empty())
				{
					fail(std::format("'{}' must be a non-empty string", key));
					return;
				}
				value = found->get<std::string>();
			}

			template <typename T>
			void read_number(const nlohmann::json& parent, std::string_view key, T& value, std::uint64_t minimum, std::uint64_t maximum)
			{
				const auto found = parent.find(key);
				if (found == parent.end())
					return;
				if (!found->is_number_unsigned() || found->get<std::uint64_t>() < minimum || found->get<std::uint64_t>() > maximum)
				{
					fail(std::format("'{}' must be a whole number from {} to {}", key, minimum, maximum));
					return;
				}
				value = static_cast<T>(found->get<std::uint64_t>());
			}

			void read_level(const nlohmann::json& root)
			{
				std::string name = "info";
				read_string(root, "log_level", name);
				if (m_error.empty() && !hb::log::parse_level(name, m_config.log_level))
					fail("'log_level' must be debug, info, warning or error");
			}

			void read_name(const nlohmann::json& root)
			{
				const auto found = root.find("world_name");
				if (found == root.end())
				{
					fail("'world_name' is required");
					return;
				}
				read_string(root, "world_name", m_config.world_name);
				if (m_error.empty() && m_config.world_name.size() > world_name_max_length)
					fail(std::format("'world_name' must be at most {} characters", world_name_max_length));
			}

			void read_key(const nlohmann::json& root)
			{
				const auto found = root.find("link_key");
				if (found == root.end())
				{
					fail("'link_key' is required");
					return;
				}
				if (!found->is_string() || !parse_link_key(found->get_ref<const std::string&>(), m_config.link_key))
					fail(std::format("'link_key' must be {} hex digits", link_key_hex_digits));
			}

			world_config& m_config;
			std::string m_error;
		};
	}

	bool is_sample_key(const world_config& config)
	{
		return std::ranges::all_of(config.link_key, [](std::uint8_t byte) { return byte == 0; });
	}

	bool load_world_config(const std::filesystem::path& path, world_config& config, std::string& error)
	{
		error.clear();
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			error = std::format("cannot open {}", path.string());
			return false;
		}
		std::stringstream text;
		text << file.rdbuf();
		// Comments allowed (// and /* */), as the Login server reads its own config.
		const nlohmann::json root = nlohmann::json::parse(text.str(), nullptr, false, true);
		if (root.is_discarded() || !root.is_object())
		{
			error = std::format("{} is not valid JSON (comments are allowed)", path.string());
			return false;
		}
		world_config loaded;
		config_reader reader(loaded);
		if (!reader.read(root))
		{
			error = std::format("{}: {}", path.string(), reader.error());
			return false;
		}
		config = std::move(loaded);
		return true;
	}
}
