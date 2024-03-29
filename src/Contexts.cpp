#include "Contexts.h"
#include "Packs.h"
#include "Devices.h"

using namespace Adversity;

namespace
{
	constexpr std::string_view dir = "Data/SKSE/AdversityFramework/Contexts";
}

void Contexts::Init()
{
	if (!fs::is_directory(dir)) {
		logger::error("no context directory exists");
		return;
	}

	for (const auto& a : fs::directory_iterator(dir)) {
		if (!fs::is_directory(a)) {
			continue;
		}

		const auto path{ a.path().string() };
		const auto configPath{ std::format("{}/{}", path, "config.yaml") };

		if (!std::filesystem::exists(configPath)) {
			continue;
		}

		try {
			auto config = YAML::LoadFile(configPath);
			auto& context = _contexts.emplace_back(config.as<Context>());

			Packs::Load(path, context.GetId());
			Devices::Load(path, context.GetId());

			logger::info("loaded context {} successfully", context.GetName());

		} catch (const std::exception& e) {
			logger::error("failed to load context {}: {}", path, e.what());
		} catch (...) {
			logger::error("failed to load context {}", path);
		}
	}
}

void Contexts::Pause(std::string a_context)
{

}

void Contexts::Reset(std::string a_context)
{

}