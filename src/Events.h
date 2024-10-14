#pragma once

#include "Event.h"
#include "Contexts.h"

namespace Adversity
{
	class Events
	{
	public:
		static void Load(std::string a_context, std::string a_pack, std::vector<Event>& a_events, ConditionParser::RefMap& a_refs);
		static Event* GetById(std::string a_id);
		static std::vector<Event*> GetByIds(std::vector<std::string> a_ids);
		static std::vector<Event*> GetInContext(std::string a_context);
		static std::vector<Event*> GetInPack(std::string a_pack);
		static std::vector<std::string> GetIds(std::vector<Event*> a_Events);
		static std::vector<Event*> Filter(std::function<bool(Event* a_Event)> a_check);
		static std::vector<Event*> Filter(std::vector<Event*> a_events, std::function<bool(Event* a_Event)> a_check);

		template <typename T>
		static T GetValue(const std::string& a_id, const std::string& a_key, T a_default, bool a_persist)
		{
			if (const auto& event = GetById(a_id)) {
				if (const auto& data = GetContextData(event, a_persist)) {
					return data->GetValue<T>(a_key, a_default);
				}

				return event->GetValue<T>(a_key, a_default);
			}

			return a_default;
		}
		template <typename T>
		static bool SetValue(std::string a_id, std::string a_key, T a_val, bool a_persist)
		{
			if (const auto& event = GetById(a_id)) {
				if (const auto& data = GetContextData(event, a_persist, true)) {
					data->SetValue<T>(a_key, a_val);
					return true;
				}
			}
			
			return false;
		}

	private:
		static inline Meta* GetContextData(Event* a_event, bool a_persist, bool a_create = false)
		{
			if (const auto& context = Contexts::GetContext(a_event->GetContext(), a_persist)) {
				if (const auto& meta = context->GetEventData(a_event->GetPackId(), a_event->GetName(), a_create)) {
					return meta;
				}
			}

			return nullptr;
		}

		static inline std::unordered_map<std::string, Event> _events;
		static inline std::unordered_map<std::string, std::vector<Event*>> _contexts;
		static inline std::unordered_map<std::string, std::vector<Event*>> _packs;
	};
}