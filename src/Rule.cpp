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

bool Conflict::With(Conflict a_other)
{
	if (type == a_other.type)
		return true;

	if (type == Type::Naked && a_other.type == Type::Wear)
		return true;

	return false;
}

bool Rule::Conflicts(Rule* a_rule)
{
	if (_excludes.contains(a_rule->GetId()) || a_rule->_excludes.contains(a_rule->GetId()))
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