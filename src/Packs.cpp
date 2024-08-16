#include "Packs.h"
#include "Rules.h"
#include "Events.h"
#include "Outfits.h"
#include "Tattoos.h"

using namespace Adversity;


void Packs::Load(std::string a_context)
{
	const std::string dir{ std::format("Data/SKSE/AdversityFramework/Contexts/{}/Packs", a_context) };
	for (const auto& a : fs::directory_iterator(dir)) {
		if (!fs::is_directory(a)) {
			continue;
		}

		const auto id{ a_context + '/' + Util::Lower(a.path().filename().replace_extension().string()) };
		
		try {
			const auto path{ a.path().string() + "/manifest.yaml" };

			auto config = YAML::LoadFile(path);

			_packs.insert({ id, config.as<Pack>() });
			auto& pack = _packs[id];

			_contexts[a_context].push_back(&pack);

			logger::info("Processing {} {} {} - {} {}", a_context, id, path, pack.rules.size(), pack.events.size());

			Rules::Load(a_context, id, pack.rules);
			Events::Load(a_context, id, pack.events);
			Outfits::Load(a_context, id);
			Tattoos::Load(a_context, id);
		} catch (std::exception& e) {
			logger::info("failed to load pack {} due to {}", id, e.what());
		} catch (...) {
			logger::info("failed to load pack {}", id);
		}
	}
	
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