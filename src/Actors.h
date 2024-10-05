#pragma once

#include "Actor.h"

namespace Adversity
{
	class Actors
	{
	public:
		static void Load(std::string a_id);

		static std::vector<Trait*> GetTraits(std::string a_context, RE::Actor* a_actor)
		{
			const auto ids = GetTraitIds(a_context, a_actor);

			std::vector<Trait*> traits;

			for (const auto& id : ids) {
				traits.push_back(&_traits[a_context][id]);
			}

			return traits;
		}
		static std::vector<std::string> GetTraitIds(std::string a_context, RE::Actor* a_actor)
		{
			std::vector<std::string> traits;

			if (!a_actor)
				return traits;

			if (const auto base = a_actor->GetActorBase()) {
				if (_actors[a_context].count(base->GetName())) {
					for (auto traitId : _actors[a_context][base->GetName()].GetTraits()) {
						if (_traits[a_context].count(traitId)) {
							traits.push_back(traitId);
						}
					}
				}
			}

			return traits;
		}

		template <typename T>
		static T GetValue(std::string a_context, RE::Actor* a_actor, std::string a_key, T a_default)
		{
			if (const auto& actor = GetActor(a_context, a_actor)) {
				return actor->GetValue<T>(a_key, a_default);
			}

			return a_default;
		}
		template <typename T>
		static bool SetValue(std::string a_context, RE::Actor* a_actor, std::string a_key, T a_val)
		{
			if (const auto& actor = GetActor(a_context, a_actor)) {
				_dirty[a_context] = true;
				actor->SetValue<T>(a_key, a_val);
				return true;
			}
			return false;
		}

		/*template <typename T>
		static T GetTraitValue(std::string a_context, RE::Actor* a_actor, std::string a_key, T a_default)
		{
			if (const auto& actor = GetActor(a_context, a_actor)) {
				const auto& traits = actor->GetTraits();
				for (const auto& traitId : traits) {
					if (const auto& trait = GetTrait(traitId)) {
						if (const auto& value = trait->GetValue<T>(a_key)) {
							return value;
						}
					}
				}
			}

			return a_default;
		}*/

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
			if (const auto base = a_actor->GetActorBase()) {
				const auto id = base->GetName();
				if (_actors[a_context].count(id)) {
					return &_actors[a_context][id];
				}
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

		static inline std::unordered_map<std::string, std::unordered_map<std::string, Actor>> _actors;
		static inline std::unordered_map<std::string, std::unordered_map<std::string, Trait>> _traits;
		static inline std::unordered_map<std::string, bool> _dirty;
		static inline std::unordered_map<std::string, std::mutex> _locks;
	};
}