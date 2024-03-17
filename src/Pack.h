#pragma once

#include "Rule.h"

namespace Adversity
{
	struct ContextSpec
	{
		std::string name;
		bool all;

	};

	class Pack
	{
	public:
		inline void SetId(std::string a_id) { _id = _id.empty() ? a_id : _id; }
	private:
		std::string _id;
		std::string _name;
		RE::TESQuest* _quest;
		std::unordered_map<std::string, ContextSpec> _contexts;
		std::vector<Rule> _rules;

		friend struct YAML::convert<Pack>;
	};
}

namespace YAML
{
	using namespace Adversity;
	
	template <>
	struct convert<ContextSpec>
	{
		static bool decode(const Node& node, ContextSpec& rhs)
		{
			rhs.name = node["name"].as<std::string>();
			rhs.all = node["all"].as<std::string>() == "true";

			return true;
		}
	};

	template <>
	struct convert<Pack>
	{
		static bool decode(const Node& node, Pack& rhs)
		{
			rhs._name = node["name"].as<std::string>();

			auto questEdid = node["quest"].as<std::string>();
			rhs._quest = RE::TESForm::LookupByEditorID<RE::TESQuest>(questEdid);

			auto contexts = node["contexts"].as<std::vector<ContextSpec>>();
			for (const auto context : contexts) {
				rhs._contexts[context.name] = context;
			}

			rhs._rules = node["rules"].as<std::vector<Rule>>();

			return !rhs._name.empty() && rhs._quest && !rhs._contexts.empty() && !rhs._rules.empty();
		}
	};
}