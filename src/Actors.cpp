#include "Util.h"
#include "Actors.h"

using namespace Adversity;

void Actors::Load(std::string a_context)
{
	const std::string base{ std::format("Data/SKSE/AdversityFramework/Contexts/{}/Actors", a_context) };
	const std::string actors{ base + "/data.yaml" };

	const auto actorFile = YAML::LoadFile(actors);
	const auto actorData = actorFile.as<std::vector<Actor>>();
	for (const auto& actor : actorData) {
		_actors[a_context].insert({ actor.GetId(), actor });
	}

	const std::string dir{ base + "/Traits" };
	for (const auto& a : fs::directory_iterator(dir)) {
		if (fs::is_directory(a)) {
			continue;
		}

		const auto id{ Util::Lower(a.path().filename().replace_extension().string()) };

		try {
			const auto path{ a.path().string() };
			auto config = YAML::LoadFile(path);

			_traits[a_context].insert({ id, config.as<Trait>() });
		} catch (std::exception& e) {
			logger::info("failed to load pack {} due to {}", id, e.what());
		} catch (...) {
			logger::info("failed to load pack {}", id);
		}
	}
}