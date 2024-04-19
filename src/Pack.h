#pragma once

#include "Event.h"
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
		inline void Init(std::string a_context, std::string a_id) {
			if (_context.empty()) {
				_context = a_context;
				_id = a_id; 
			}
		}
		inline RE::TESQuest* GetQuest() { return _quest; }
		inline std::string GetId() { return _id; }
		inline std::string GetName() { return _name; }
		std::vector<Rule> rules;
		std::vector<Event> events;
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
			
			rhs.rules = node["rules"].as<std::vector<Rule>>(std::vector<Rule>{});
			rhs.events = node["events"].as<std::vector<Event>>(std::vector<Event>{});

			return !rhs._name.empty() && rhs._quest && (!rhs.rules.empty() || !rhs.events.empty());
		}
	};
}