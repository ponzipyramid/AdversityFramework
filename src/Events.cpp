#include "Util.h"
#include "Events.h"

using namespace Adversity;

void Events::Load(std::string a_context, std::string a_pack, std::vector<Event>& a_events, ConditionParser::RefMap& a_refs)
{
	for (auto& event : a_events) {
		event.Init(a_context, a_pack, a_refs);
		_events[event.GetId()] = event;

		auto ref = GetById(event.GetId());
		_contexts[a_context].push_back(ref);
		_packs[a_pack].push_back(ref);
	}
}

Event* Events::GetById(std::string a_id)
{
	a_id = Util::Lower(a_id);
	return _events.count(a_id) ? &_events[a_id] : nullptr;
}

std::vector<Event*> Events::GetByIds(std::vector<std::string> a_ids)
{
	std::vector<Event*> events;

	for (auto id : a_ids) {
		if (auto ev = GetById(id))
			events.push_back(ev);
	}

	return events;
}

std::vector<Event*> Events::GetInContext(std::string a_context)
{
	return _contexts[a_context];
}

std::vector<Event*> Events::GetInPack(std::string a_pack)
{
	return _packs[a_pack];
}

std::vector<std::string> Events::GetIds(std::vector<Event*> a_events)
{
	std::vector<std::string> ids;
	for (auto ev : a_events) {
		ids.push_back(ev->GetId());
	}

	return ids;
}

std::vector<Event*> Events::Filter(std::function<bool(Event* a_event)> a_check)
{
	std::vector<Event*> filtered;

	for (auto& [_, event] : _events) {
		if (a_check(&event))
			filtered.push_back(&event);
	}

	return filtered;
}

std::vector<Event*> Events::Filter(std::vector<Event*> a_events, std::function<bool(Event* a_event)> a_check)
{
	std::vector<Event*> filtered;

	for (auto& event : a_events) {
		if (a_check(event))
			filtered.push_back(event);
	}

	return filtered;
}
