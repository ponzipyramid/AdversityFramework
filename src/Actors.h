#pragma once

#include "Actor.h"

namespace Adversity
{
	class Actors
	{
	public:
		static void Load(std::string a_id);

		static inline void InitializeActor(std::string a_context, RE::Actor* a_actor)
		{
			if (!a_actor) {
				return;
			}

			if (const auto& actor = GetActor(a_context, a_actor)) {
				// clean up current traits
				for (const auto& [_, trait] : _traits[a_context]) {
					if (const auto& faction = trait.GetFaction()) {
						if (a_actor->GetFactionRank(faction, false) > -1) {
							logger::info("removing from faction {}", faction->GetFormEditorID());
							a_actor->RemoveFromFaction(faction);
						}
					}
				}

				for (const auto& attached : actor->GetTraits()) {
					if (const auto& trait = GetTrait(a_context, attached.id)) {
						if (const auto& faction = trait->GetFaction()) {
							a_actor->AddToFaction(faction, attached.rank);
							logger::info("adding to faction {}", faction->GetFormEditorID());
						}
					}
				}
			}
		}

		static inline std::vector<Trait*> GetTraits(std::string a_context, RE::Actor* a_actor)
		{
			const auto ids = GetTraitIds(a_context, a_actor);

			std::vector<Trait*> traits;

			for (const auto& id : ids) {
				traits.push_back(&_traits[a_context][id]);
			}

			return traits;
		}
		static inline std::vector<std::string> GetTraitIds(std::string a_context, RE::Actor* a_actor)
		{
			std::vector<std::string> traits;

			if (!a_actor)
				return traits;

			if (const auto& actor = GetActor(a_context, a_actor)) {
				for (auto attached : _actors[a_context][actor->GetId()].GetTraits()) {
					if (_traits[a_context].count(attached.id)) {
						traits.push_back(attached.id);
					}
				}
			}

			return traits;
		}

		template <typename T>
		static inline T GetValue(std::string a_context, RE::Actor* a_actor, std::string a_key, T a_default)
		{
			if (const auto& actor = GetActor(a_context, a_actor)) {
				if (actor->HasValue<T>(a_key)) {
					return actor->GetValue<T>(a_key, a_default);
				}

				const auto& traits = actor->GetTraits();
				for (const auto& attached : traits) {
					if (const auto& trait = GetTrait(a_context, attached.id)) {
						a_actor->AddToFaction(trait->GetFaction(), 0);
					}
				}
			}

			return a_default;
		}
		template <typename T>
		static inline bool SetValue(std::string a_context, RE::Actor* a_actor, std::string a_key, T a_val)
		{
			if (!a_actor)
				return false;

			if (!GetActor(a_context, a_actor)) {
				Actor actor{ a_actor };
				_actors[a_context].insert({ actor.GetId(), actor});
			}

			if (const auto& actor = GetActor(a_context, a_actor)) {
				_dirty[a_context] = true;
				actor->SetValue<T>(a_key, a_val);
				return true;
			}
			return false;
		}

		static inline void PersistAll()
		{
			for (const auto& [id, data] : _actors)
			{
				if (_dirty.count(id) && _dirty[id]) {
					Persist(id);
				}
			}
		}
		static inline void Persist(std::string a_context)
		{
			if (!_actors.count(a_context)) {
				return;
			}

			std::unique_lock lock{ _locks[a_context] };

			std::vector<Actor> actors;
			for (const auto& [id, actor] : _actors[a_context]) {
				actors.push_back(actor);
			}
		
			YAML::Node node{ actors };
			const std::string file{ std::format("Data/SKSE/AdversityFramework/Contexts/{}/Actors/data.yaml", a_context) };
			std::ofstream fout(file);
			fout << node;

			_dirty.erase(a_context);
		}
	private:
		static inline Actor* GetActor(std::string a_context, RE::Actor* a_actor)
		{
			const auto formId = a_actor->GetFormID();

			if (_actors[a_context].count(formId)) {
				return &_actors[a_context][formId];
			}

			return nullptr;
		}

		static inline Trait* GetTrait(std::string a_context, std::string a_id)
		{
			if (_traits[a_context].count(a_id)) {
				return &_traits[a_context][a_id];
			}

			return nullptr;
		}

		static inline std::unordered_map<std::string, std::unordered_map<RE::FormID, Actor>> _actors;
		static inline std::unordered_map<std::string, std::unordered_map<std::string, Trait>> _traits;
		static inline std::unordered_map<std::string, bool> _dirty;
		static inline std::unordered_map<std::string, std::mutex> _locks;
	};
}