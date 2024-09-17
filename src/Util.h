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
		static inline void ProcessEntities(std::string a_context, std::string a_pack, std::string a_type, std::function<void(std::string, T)> a_func)
		{

			const std::string dir{ std::format("data/skse/adversityframework/contexts/{}/packs/{}/{}", a_context, a_pack, a_type) };

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

		static inline std::string Join(const std::vector<std::string>& a_vec, std::string_view a_delimiter)
		{
			return std::accumulate(a_vec.begin(), a_vec.end(), std::string{},
				[a_delimiter](const auto& str1, const auto& str2) {
					return str1.empty() ? str2 : str1 + a_delimiter.data() + str2;
				});
		}

		static inline float GetGameTime()
		{
			return RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x39, "Skyrim.esm")->value;
		}

		static inline void AddKwd(RE::TESObjectARMO* a_form, std::string a_kwd) {
			if (const auto kwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(a_kwd)) {
				a_form->AddKeyword(kwd);
			}
		}

		static inline std::optional<std::pair<RE::FormID, std::string>> ParseFormId(std::string a_str)
		{
			const auto splits = Split(a_str, "|");

			if (splits.size() != 2) {
				return std::nullopt;
			}

			char* p;
			const auto formId = std::strtol(splits[0].c_str(), &p, 16);

			if (*p != 0) {
				return std::nullopt;
			}

			const auto espName = std::regex_replace(splits[1], std::regex("^ +| +$|( ) +"), "$1");

			return std::make_pair(formId, espName);
		}


		template <typename T = RE::TESForm>
		static inline T* GetFormFromString(const std::string& s)
		{
			if (auto form = RE::TESForm::LookupByEditorID(s)) {
				return form->As<T>();
			}

			if (const auto parsed = ParseFormId(s)) {
				const auto [formId, espName] = parsed.value();
				return RE::TESDataHandler::GetSingleton()->LookupForm<T>(formId, espName);
			} else {
				return nullptr;
			}
		}

		static inline bool IsNumeric(std::string a_str)
		{
			static const std::regex pattern(R"(^[+-]?(?:\d+|\d*\.\d+)$)");
			return std::regex_match(a_str, pattern);
		}

	private:
		static inline const std::unordered_set<std::string> _exts{ ".yaml", ".yml" };
	};
}