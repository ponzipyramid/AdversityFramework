#pragma once

#include "Util.h"
#include "PackItem.h"

namespace Adversity
{
	struct Conflict
	{
		enum Type
		{
			Unknown,
			Wear,
			Naked,
			Filth,
			Outfit,
			Clean
		};

		Type type;
		std::unordered_set<int> slots;
		bool exclusive;
		bool With(Conflict a_other);
	};

	class Rule : public PackItem
	{
	public:
		inline std::string GetHint() { return _hint; }
		inline RE::BGSKeyword* GetKwd() { return _kwd; }
		bool Conflicts(Rule* a_rule);
	private:
		RE::BGSKeyword* _kwd = nullptr;
		std::string _hint;
		int _severity;
		std::vector<Conflict> _conflicts;
		std::unordered_set<std::string> _excludes;
		std::unordered_set<std::string> _compatible;

		friend struct YAML::convert<Rule>;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<Conflict>
	{
		static bool decode(const Node& node, Conflict& rhs)
		{
			const auto type = node["type"].as<std::string>();
			rhs.type = magic_enum::enum_cast<Conflict::Type>(type, magic_enum::case_insensitive).value_or(Conflict::Type::Unknown);
			const auto slots = node["slots"].as<std::vector<int>>();
			rhs.slots = std::unordered_set<int>{ slots.begin(), slots.end() };
			rhs.exclusive = node["exclusive"].as<std::string>("") == "true";
			return rhs.type != Conflict::Type::Unknown;
		}
	};

	template <>
	struct convert<Rule>
	{
		static bool decode(const Node& node, Rule& rhs)
		{
			rhs._name = node["name"].as<std::string>();
			rhs._desc = node["desc"].as<std::string>("");
			rhs._hint = node["hint"].as<std::string>("");
			rhs._severity = node["severity"].as<int>();
			const auto tags = node["tags"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._tags = std::unordered_set<std::string>{ tags.begin(), tags.end() };

			const auto global = node["global"].as<std::string>();
			rhs._global = RE::TESForm::LookupByEditorID<RE::TESGlobal>(global);

			const auto keyword = node["global"].as<std::string>();
			rhs._kwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(keyword);

			const auto excludes = node["excludes"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._excludes = std::unordered_set<std::string>{ excludes.begin(), excludes.end() };

			const auto compatible = node["compatible"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._compatible = std::unordered_set<std::string>{ compatible.begin(), compatible.end() };

			rhs._reqs = node["requirements"].as<std::vector<std::string>>(std::vector<std::string>{});

			return !rhs._name.empty() && rhs._severity >= 0 && rhs._global;
		}
	};
}