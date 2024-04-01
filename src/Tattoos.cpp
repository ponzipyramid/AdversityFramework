#include "Tattoos.h"
#include "Util.h"

using namespace Adversity;

void Tattoos::Load(std::string a_context)
{
	Util::ProcessEntities<std::vector<Group>>(a_context, "tattoos", [](std::string a_id, std::vector<Group> a_group) {
		_groups.insert({ a_id, a_group });
	});
}

std::vector<Group*> Tattoos::GetGroups(std::string a_context, std::string a_name)
{
	std::vector<Group*> groups;
	const auto id{ std::format("{}/{}", a_context, a_name) };
	if (!_groups.count(id))
		return groups;

	auto& found = _groups[id];
	groups.reserve(found.size());
	for (auto& group : found) groups.push_back(&group);

	return groups;
}