#pragma once

#include "Util.h"
#include "PackItem.h"

namespace Adversity
{
	class Rule : public PackItem
	{
	public:
		inline std::string GetHint() { return _hint; }
		inline RE::BGSKeyword* GetKwd() { return _kwd; }
		inline bool IsValid() { return !_name.empty() && _global; }
	private:
		RE::BGSKeyword* _kwd = nullptr;
		std::string _hint;

		friend struct YAML::convert<Rule>;
	};
}

namespace YAML
{
	template <>
	struct convert<Rule>
	{
		static bool decode(const Node& node, Rule& rhs)
		{
			rhs._name = node["name"].as<std::string>();
			rhs._desc = node["desc"].as<std::string>("");
			rhs._severity = node["severity"].as<int>(0);
			const auto tags = node["tags"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._tags = std::unordered_set<std::string>{ tags.begin(), tags.end() };

			const auto global = node["global"].as<std::string>();
			rhs._global = RE::TESForm::LookupByEditorID<RE::TESGlobal>(global);
			rhs._reqs = node["requirements"].as<std::vector<std::string>>(std::vector<std::string>{});

			rhs._hint = node["hint"].as<std::string>("");
			const auto keyword = node["global"].as<std::string>();
			rhs._kwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(keyword);

			const auto excludes = node["excludes"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._excludes = std::unordered_set<std::string>{ excludes.begin(), excludes.end() };

			const auto compatible = node["compatible"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._compatible = std::unordered_set<std::string>{ compatible.begin(), compatible.end() };

			return true;
		}
	};
}