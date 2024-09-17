#pragma once

#include "Event.h"

namespace
{
	inline void FilterPreferences(std::string a_context, std::vector<std::string> a_prefs, std::vector<std::string>& a_tags, std::unordered_set<std::string>& a_names)
	{
		for (const auto& pref : a_prefs) {
			if (pref.starts_with("tag:")) {
				a_tags.push_back(pref);
			} else {
				a_names.insert(a_context + "/" + pref);
			}
		}
	}
}

namespace Adversity
{
	struct PreferenceList
	{
		std::vector<std::string> like;
		std::vector<std::string> dislike;
	};

	struct PreferenceSet
	{
		std::unordered_set<std::string> like;
		std::unordered_set<std::string> dislike;
	};

	class Trait
	{
	public:
		inline void Init(std::string a_context, std::string a_id)
		{
			_id = a_id;
			FilterPreferences(a_context, _eventPrefs.like, _eventTags.like, _eventNames.like);
			FilterPreferences(a_context, _eventPrefs.dislike, _eventTags.dislike, _eventNames.dislike);
		}
		inline std::string GetID() const { return _id; }
		inline std::string const GetName() const { return _name; }
		inline RE::TESFaction* const GetFaction() const { return _faction; }
		inline int Prefers(const Event* a_event) const {
			if (Prefers(a_event, _eventTags.like, _eventNames.like))
				return 1;
			if (Prefers(a_event, _eventTags.dislike, _eventNames.dislike))
				return -1;
			return 0;
		}
	private:
		inline bool Prefers(const Event* a_event, const std::vector<std::string>& a_tags, const std::unordered_set<std::string>& a_names) const
		{
			if (a_names.contains(a_event->GetId()))
				return true;

			for (const auto& tag : a_tags) {
				if (a_event->HasTag(tag)) {
					return true;
				}
			}

			return false;
		}

		std::string _id;
		std::string _name;
		std::string _context;
		RE::TESFaction* _faction;
		PreferenceList _eventPrefs;
		PreferenceSet _eventNames;
		PreferenceList _eventTags;

		friend struct YAML::convert<Trait>;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<PreferenceList>
	{
		static bool decode(const Node& node, PreferenceList& rhs)
		{
			rhs.like = node["like"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs.dislike = node["dislike"].as<std::vector<std::string>>(std::vector<std::string>{});

			return true;
		}
	};

	template <>
	struct convert<Trait>
	{
		static bool decode(const Node& node, Trait& rhs)
		{
			rhs._name = node["name"].as<std::string>();

			const auto faction = node["faction"].as<std::string>("");
			rhs._faction = RE::TESForm::LookupByEditorID<RE::TESFaction>(faction);

			rhs._eventPrefs = node["events"].as<PreferenceList>(PreferenceList{});

			return true;
		}
	};
}