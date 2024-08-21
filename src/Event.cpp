#include "Event.h"

using namespace Adversity;

bool Event::HasTags(std::vector<std::string> a_tags, bool a_all)
{
	if (a_all) {
		for (const auto& tag : a_tags) {
			if (!_tags.contains(tag))
				return false;
		}

		return true;
	} else {
		for (const auto& tag : a_tags) {
			if (_tags.contains(tag))
				return true;
		}

		return false;
	}
}

bool Event::HasTag(std::string a_tag)
{
	return _tags.contains(a_tag);
}

bool Event::ReqsMet()
{
	const auto handler = RE::TESDataHandler::GetSingleton();

	for (const auto& req : _reqs) {
		if (!handler->LookupModByName(req)) {
			return false;
		}
	}

	return true;
}

bool Event::Conflicts(Event* a_other)
{
	if (_excludes.contains(a_other->GetId()) || a_other->_excludes.contains(a_other->GetId()))
		return true;

	for (auto thisConflict : _conflicts) {
		for (auto otherConflict : a_other->_conflicts) {
			if (thisConflict.With(otherConflict) || otherConflict.With(thisConflict))
				return true;
		}
	}

	return false;
}

bool Conflict::With(Conflict a_other)
{
	if (type == Type::Filth && a_other.type == Type::Clean) {
		return true;
	}

	if (type == Type::Wear && a_other.type == Type::Wear) {
		for (const auto slot : slots) {
			if (a_other.slots.contains(slot))
				return true;
		}
	}

	if (type == Type::Outfit && a_other.type == Type::Naked) {
		return true;
	}

	if (type == Type::HeavyBondage && a_other.type == Type::HeavyBondage) {
		return true;
	}

	if (type == Type::Outfit && a_other.type == Type::Outfit) {
		return true;
	}

	return false;
}