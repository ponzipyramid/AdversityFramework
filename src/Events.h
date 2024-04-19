#pragma once

#include "Event.h"

namespace Adversity
{
	class Events
	{
	public:
		static void Load(std::string a_context, std::string a_pack, std::vector<Event>& a_events);
		static Event* GetById(std::string a_id);
		static std::vector<Event*> GetByIds(std::vector<std::string> a_ids);
		static std::vector<Event*> GetInContext(std::string a_context);
		static std::vector<Event*> GetInPack(std::string a_pack);
		static std::vector<std::string> GetIds(std::vector<Event*> a_Events);
		static std::vector<Event*> Filter(std::function<bool(Event* a_Event)> a_check);
	private:
		static inline std::unordered_map<std::string, Event> _events;
		static inline std::unordered_map<std::string, std::vector<Event*>> _contexts;
		static inline std::unordered_map<std::string, std::vector<Event*>> _packs;
	};
}