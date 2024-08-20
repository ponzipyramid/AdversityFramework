#pragma once

#include "Event.h"
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

			const auto events{ node["events"].as<std::vector<Event>>(std::vector<Event>{}) };
			std::copy_if(events.begin(), events.end(), std::back_inserter(rhs.events), [](Event a_event) {
				return a_event.IsValid();
			});

			return !rhs._name.empty() && rhs._quest && !rhs.events.empty();
		}
	};
}