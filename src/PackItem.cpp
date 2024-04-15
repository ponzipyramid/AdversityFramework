#include "PackItem.h"

using namespace Adversity;

bool PackItem::HasTags(std::vector<std::string> a_tags, bool a_all)
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

bool PackItem::HasTag(std::string a_tag)
{
	return _tags.contains(a_tag);
}

bool PackItem::ReqsMet()
{
	const auto handler = RE::TESDataHandler::GetSingleton();

	for (const auto& req : _reqs) {
		if (!handler->LookupModByName(req)) {
			return false;
		}
	}

	return true;
}

bool PackItem::Conflicts(PackItem* a_other)
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