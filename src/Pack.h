#pragma once

#include "Rule.h"
#include "Util.h"

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
		inline void Init(std::string a_context) {
			if (_context.empty()) {
				_context = a_context;
				_id = std::format("{}/{}", _context, Util::Lower(_name)); 
			}
		}
		inline RE::TESQuest* GetQuest() { return _quest; }
		inline std::string GetId() { return _id; }
		std::vector<Rule> rules;
	private:
		std::string _id;
		std::string _name;
		RE::TESQuest* _quest;
		std::string _context;

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
			
			rhs.rules = node["rules"].as<std::vector<Rule>>();

			return !rhs._name.empty() && rhs._quest && !rhs.rules.empty();
		}
	};
}