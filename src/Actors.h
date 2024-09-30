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
			logger::info("looking for {} on {}", a_key, a_actor->GetActorBase()->GetName());
			if (const auto data = GetData(a_context, a_actor, a_key)) {
				logger::info("found data {} on {}", a_key, a_actor->GetActorBase()->GetName());
				if (std::holds_alternative<T>(*data)) {
					return std::get<T>(*data);
				} else {
					return a_default;
				}				
			}

			return a_default;
		}
		template <typename T>
		static bool SetValue(std::string a_context, RE::Actor* a_actor, std::string a_key, T a_val)
		{
			return SetData(a_context, a_actor, a_key, GenericData{ a_val });
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
		static inline GenericData* GetData(std::string a_context, RE::Actor* a_actor, std::string a_key)
		{
			if (const auto base = a_actor->GetActorBase()) {
				const auto id = base->GetName();
				if (_actors[a_context].count(id)) {
					return _actors[a_context][id].GetValue(a_key);
				} 
			}

			return nullptr;
		}
		static inline bool SetData(std::string a_context, RE::Actor* a_actor, std::string a_key, GenericData a_data)
		{
			if (const auto base = a_actor->GetActorBase()) {
				const auto id = base->GetName();
				_actors[a_context][id].SetValue(a_key, a_data);

				_dirty[a_context] = true;

				return true;
			}

			return false;
		}

		static inline std::unordered_map<std::string, std::unordered_map<std::string, Actor>> _actors;
		static inline std::unordered_map<std::string, std::unordered_map<std::string, Trait>> _traits;
		static inline std::unordered_map<std::string, bool> _dirty;
		static inline std::unordered_map<std::string, std::mutex> _locks;
	};
}