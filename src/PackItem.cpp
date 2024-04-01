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
