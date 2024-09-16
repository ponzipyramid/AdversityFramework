#pragma once

#include "Actor.h"

namespace Adversity
{
	class Actors
	{
	public:
		static void Load(std::string a_id);
		static void SetData(std::string a_context, RE::Actor* a_actor, std::string a_key, GenericData a_data, bool a_persist)
		{
			if (const auto base = a_actor->GetActorBase()) {
				const auto id = base->GetFormEditorID();
				if (_actors[a_context].count(id)) {
					_actors[a_context][id].SetValue(a_key, a_data);
				}
			}

			if (a_persist) {
				Persist(a_context);
			}
		}
		static void Persist(std::string a_context)
		{
			std::vector<Actor> actors;
			for (const auto& [id, actor] : _actors[a_context])
			{
				actors.push_back(actor);
			}
		
			YAML::Node node;
			node["root"] = actors;
		}
	private:
		static inline std::unordered_map<std::string, std::unordered_map<std::string, Actor>> _actors;
		static inline std::unordered_map<std::string, std::unordered_map<std::string, Trait>> _traits;
	};
}