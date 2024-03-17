#include "Packs.h"

using namespace Adversity;

namespace
{
	constexpr std::string_view ext1 = ".yaml";
	constexpr std::string_view ext2 = ".yml";
	constexpr std::string_view dir = "Data\\SKSE\\AdversityFramework\\Packs";
}

void Packs::Init()
{
	for (const auto& a : std::filesystem::directory_iterator(dir)) {
		if (std::filesystem::is_directory(a)) {
			continue;
		}

		const auto ext = a.path().extension();
		if (ext != ext1 && ext != ext2)
			continue;

		const auto path{ a.path().string() };
		const auto id{ a.path().filename().replace_extension().string() };

		try {
			logger::info("file: {}", path);

			auto packFile = YAML::LoadFile(path);
			auto pack = packFile.as<Pack>();
			pack.SetId(id);
			_packs.push_back(pack);

			logger::info("loaded pack {} successfully", id);

		} catch (const std::exception& e) {
			logger::error("failed to load pack {}: {}", path, e.what());
		} catch (...) {
			logger::error("failed to load pack {}", path);
		}
	}
}

void Packs::SwitchDialogueContexts(std::string a_context)
{
	for (const auto& [name, globals] : _contexts) {
		if (name == a_context) {
			for (const auto& global : globals) {
				global->value = 1.0f;
			}
		} else {
			for (const auto& global : globals) {
				global->value = 0.0f;
			}
		}
	}
}
