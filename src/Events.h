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

			logger::info("GetValue - {} {} {}", a_id, a_key, a_persist);

			if (const auto data = Contexts::GetEventValue<T>(a_id, a_key, a_persist)) {
				return data.value();
			}

			// TODO: add default persistent metadata to events

			return a_default;
		}
		template <typename T>
		static bool SetValue(std::string a_id, std::string a_key, T a_val, bool a_persist)
		{
			logger::info("SetValue - {} {} {}", a_id, a_key, a_persist);
			return Contexts::SetEventValue<T>(a_id, a_key, a_val, a_persist);
		}

	private:
		static inline std::unordered_map<std::string, Event> _events;
		static inline std::unordered_map<std::string, std::vector<Event*>> _contexts;
		static inline std::unordered_map<std::string, std::vector<Event*>> _packs;
	};
}