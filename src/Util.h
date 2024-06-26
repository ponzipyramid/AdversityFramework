#pragma once
#include "UI.h"

namespace Adversity
{
	class Util
	{
	public:
		template <typename T>
		static inline T* GetFormById(RE::FormID a_id)
		{
			return RE::TESDataHandler::GetSingleton()->LookupForm<T>(a_id, "Adversity Framework.esm");
		}
		static inline int GetWeightedIndex(std::vector<int> a_weights)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::discrete_distribution<int> d{ a_weights.begin(), a_weights.end() };

			return d(gen);
		}
		static inline std::string Lower(std::string a_str)
		{
			std::string data{ a_str };
			std::transform(data.begin(), data.end(), data.begin(),
				[](unsigned char c) { return (char)std::tolower(c); });

			return data;
		}
		static inline bool IsYAML(fs::path a_path)
		{
			return _exts.contains(a_path.extension().string());
		}

		static inline std::vector<std::string> FilterByPrefix(std::vector<std::string> a_strs, std::string a_prefix)
		{
			std::vector<std::string> filtered;

			std::copy_if(a_strs.begin(), a_strs.end(), std::back_inserter(filtered), [&a_prefix](std::string a_str) { return a_str.starts_with(a_prefix); });

			return filtered;
		}

		static inline std::string RemovePrefix(std::string a_str, std::string a_prefix)
		{
			return a_str.starts_with(a_prefix) ? a_str.substr(a_prefix.size()) : a_str;
		}

		template <typename T>
		static inline void ProcessEntities(std::string a_context, std::string a_type, std::function<void(std::string, T)> a_func)
		{
			const std::string dir{ std::format("data/skse/adversityframework/contexts/{}/{}", a_context, a_type) };

			if (!fs::is_directory(dir)) {
				logger::warn("{} has no {} directory", a_context, a_type);
				return;
			}

			for (const auto& a : fs::directory_iterator(dir)) {
				if (fs::is_directory(a)) {
					continue;
				}

				if (!IsYAML(a.path()))
					continue;

				const auto path{ a.path().string() };
				const auto filename{ a.path().filename().replace_extension().string() };

				try {
					const std::string id{ std::format("{}/{}", a_context, Util::Lower(filename)) };
					auto config = YAML::LoadFile(path);
					a_func(id, config.as<T>());
					logger::info("loaded {} {} in {} successfully", a_type, filename, a_context);
				} catch (const std::exception& e) {
					logger::error("failed to load {} {} in {}: {}", a_type, filename, a_context, e.what());
				} catch (...) {
					logger::error("failed to load {} {} in {}", a_type, filename, a_context);
				}
			}
		}

		static inline std::vector<std::string> Split(std::string a_str, std::string a_delim)
		{
			std::vector<std::string> v;
			if (!a_str.empty()) {
				std::size_t start = 0;
				do {
					auto idx = a_str.find(a_delim, start);
					if (idx == std::string::npos) {
						break;
					}
					std::size_t length = idx - start;
					v.push_back(a_str.substr(start, length));
					start += (length + a_delim.size());
				} while (true);
				v.push_back(a_str.substr(start));
			}

			return v;
		}

		static inline float GetGameTime()
		{
			return RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x39, "Skyrim.esm")->value;
		}
	private:
		static inline const std::unordered_set<std::string> _exts{ ".yaml", ".yml" };
	};
}