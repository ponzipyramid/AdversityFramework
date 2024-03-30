#include "Packs.h"
#include "Rules.h"

using namespace Adversity;

void Packs::Load(std::string a_path, std::string a_context)
{
	const std::string dir{ std::format("{}/Packs", a_path, a_context) };

	if (!fs::is_directory(dir)) {
		logger::warn("{} has no packs directory", a_context);
		return;
	}

	for (const auto& a : fs::directory_iterator(dir)) {
		if (fs::is_directory(a)) {
			continue;
		}

		if (!Util::IsYAML(a.path()))
			continue;

		const auto path{ a.path().string() };
		const auto filename{ a.path().filename().replace_extension().string() };

		try {
			auto packFile = YAML::LoadFile(path);
			auto temp = packFile.as<Pack>();
			temp.Init(a_context);

			const auto id = temp.GetId();

			_packs.insert({ id, temp });

			auto pack = _packs[id];

			_contexts[a_context].push_back(&pack);

			Rules::Load(a_context, id, pack.rules);

			logger::info("loaded pack {} successfully", filename);

		} catch (const std::exception& e) {
			logger::error("failed to load pack {}: {}", filename, e.what());
		} catch (...) {
			logger::error("failed to load pack {}", filename);
		}
	}
}

Pack* Packs::GetById(std::string a_pack) {
	return _packs.count(a_pack) ? &_packs[a_pack] : nullptr;
}


std::vector<Pack*> Packs::GetByContext(std::string a_context)
{
	return _contexts[a_context];
}

std::vector<std::string> Packs::GetIds(std::vector<Pack*> a_packs)
{
	std::vector<std::string> ids;
	for (auto pack : a_packs)
		ids.push_back(pack->GetId());
	return ids;
}