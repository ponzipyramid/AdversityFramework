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
				for (auto traitId : _actors[a_context][base->GetFormEditorID()].GetTraits()) {
					if (_traits[a_context].count(traitId)) {
						traits.push_back(traitId);
					}
				}
			}

			return traits;
		}

		template <typename T>
		static T GetValue(std::string a_context, RE::Actor* a_actor, std::string a_key, T a_default)
		{
			if (const auto data = Actors::GetData(a_context, a_actor, a_key)) {
				if constexpr (std::same_as<T, decltype(data)>) {
					// do stuff with value as an int since it's a int here
					return std::get<T>(*data);
				} else {
					return a_default;
				}				
			}

			return a_default;
		}
		template <typename T>
		static void SetValue(std::string a_context, RE::Actor* a_actor, std::string a_key, T a_val, bool a_persist)
		{
			Actors::SetData(a_context, a_actor, a_key, GenericData{ a_val }, a_persist);
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
				const auto id = base->GetFormEditorID();
				_actors[a_context][id].GetValue(a_key);
			}

			return nullptr;
		}
		static inline void SetData(std::string a_context, RE::Actor* a_actor, std::string a_key, GenericData a_data, bool a_persist)
		{
			if (const auto base = a_actor->GetActorBase()) {
				const auto id = base->GetFormEditorID();
				_actors[a_context][id].SetValue(a_key, a_data);
			}

			_dirty[a_context] = true;

			if (a_persist) {
				Persist(a_context);
			}
		}

		static inline std::unordered_map<std::string, std::unordered_map<std::string, Actor>> _actors;
		static inline std::unordered_map<std::string, std::unordered_map<std::string, Trait>> _traits;
		static inline std::unordered_map<std::string, bool> _dirty;
		static inline std::unordered_map<std::string, std::mutex> _locks;
	};
}