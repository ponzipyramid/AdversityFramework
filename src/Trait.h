#pragma once

#include "Event.h"

namespace Adversity
{
	struct PreferenceList
	{
		std::vector<std::string> likes;
		std::vector<std::string> dislikes;
	};

	struct PreferenceSet
	{
		std::unordered_set<std::string> likes;
		std::unordered_set<std::string> dislikes;
	};

	class Trait
	{
	public:
		inline std::string GetID() const { return _id; }
		inline std::string const GetName() const { return _name; }
		inline RE::TESFaction* const GetFaction() const { return _faction; }
		inline bool Likes(const Event* a_event) const
		{
			if (_eventNames.likes.contains(a_event->GetId()))
				return true;

			for (const auto& tag : _eventTags.likes) {
				if (a_event->HasTag(tag)) {
					return true;
				}
			}
		}

	private:
		std::string _id;
		std::string _name;
		RE::TESFaction* _faction;
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
			rhs.likes = node["likes"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs.dislikes = node["dislikes"].as<std::vector<std::string>>(std::vector<std::string>{});

			return true;
		}
	};

	void FilterPreferences(std::vector<std::string> a_prefs, std::vector<std::string>& a_tags, std::unordered_set<std::string>& a_names)
	{
		for (const auto& pref : a_prefs) {
			if (pref.starts_with("tag:")) {
				a_tags.push_back(pref);
			} else {
				a_names.insert(pref);
			}
		}
	}

	template <>
	struct convert<Trait>
	{
		static bool decode(const Node& node, Trait& rhs)
		{
			rhs._name = node["name"].as<std::string>();

			const auto faction = node["faction"].as<std::string>("");
			rhs._faction = RE::TESForm::LookupByEditorID<RE::TESFaction>(faction);

			const auto eventPrefs = node["events"].as<PreferenceList>(PreferenceList{});
			FilterPreferences(eventPrefs.likes, rhs._eventTags.likes, rhs._eventNames.likes);
			FilterPreferences(eventPrefs.dislikes, rhs._eventTags.dislikes, rhs._eventNames.dislikes);

			return true;
		}
	};
}