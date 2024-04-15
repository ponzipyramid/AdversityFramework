#include "Packs.h"
#include "Rules.h"

using namespace Adversity;

void Packs::Load(std::string a_context)
{
	Util::ProcessEntities<Pack>(a_context, "packs", [a_context](std::string a_id, Pack a_pack) {
		a_pack.Init(a_context, a_id);

		_packs.insert({ a_id, a_pack });

		auto& pack = _packs[a_id];

		_contexts[a_context].push_back(&pack);

		Rules::Load(a_context, a_pack.GetId(), a_pack.rules);
	});
}

Pack* Packs::GetById(std::string a_pack) {
	return _packs.count(a_pack) ? &_packs[a_pack] : nullptr;
}

std::vector<Pack*> Packs::GetByContext(std::string a_context)
{
	return _contexts[Util::Lower(a_context)];
}

std::vector<std::string> Packs::GetIds(std::vector<Pack*> a_packs)
{
	std::vector<std::string> ids;
	for (auto pack : a_packs)
		ids.push_back(pack->GetId());
	return ids;
}