#include "Rule.h"

using namespace Adversity;

bool Rule::HasTags(std::vector<std::string> a_tags, bool a_all)
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

bool Rule::HasTag(std::string a_tag)
{
	return _tags.contains(a_tag);
}

bool Rule::Conflicts(Rule* a_rule)
{
	if (_compatible.contains(a_rule->GetId()) || a_rule->_compatible.contains(GetId()))
		return false;

	if (_excludes.contains(a_rule->GetId()) || a_rule->_excludes.contains(GetId()))
		return true;

	for (auto thisConflict : _conflicts) {
		for (auto otherConflict : a_rule->_conflicts) {
			if (thisConflict.With(otherConflict) || otherConflict.With(thisConflict))
				return true;
		}
	}

	return false;
}

bool Rule::ReqsMet()
{
	const auto handler = RE::TESDataHandler::GetSingleton();
	
	for (const auto& req : _reqs) {
		if (!handler->LookupModByName(req)) {
			return false;
		}
	}

	return true;
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

	if (type == Type::Outfit && a_other.type == Type::Outfit) {
		return exclusive;
	}

	return false;
}
