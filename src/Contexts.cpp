#include "Contexts.h"
#include "Packs.h"
#include "Devices.h"
#include "Actors.h"

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
		const auto id{ Util::Lower(a.path().filename().replace_extension().string()) };
		const auto userData{ "Data/SKSE/AdversityFramework/UserData/" + id + ".yaml" };

		try {
			if (fs::exists(userData)) {
				logger::info("loading user data for {}", id);
				auto context = YAML::Load(userData).as<Context>();
				context.Init(id);
				_persistent[id] = context;
				_dirty[id] = false;
				_locks[id];
			}

			Packs::Load(id);
			Devices::Load(id);
			Actors::Load(id);

			logger::info("loaded context {} successfully", id);

		} catch (const std::exception& e) {
			logger::error("failed to load context {}: {}", path, e.what());
		} catch (...) {
			logger::error("failed to load context {}", path);
		}
	}
}

Meta* Contexts::GetEventData(const std::string& a_id, bool a_persist)
{
	const auto splits = Util::Split(a_id, "/");

	if (splits.size() != 3)
		return nullptr;

	auto& data = a_persist ? _persistent : _runtime;

	if (data.count(splits[1])) {
		auto& context = data[a_id];
		return context.GetEventData(splits[1], splits[2]);
	}

	return nullptr;
}

GenericData* Contexts::GetEventField(const std::string& a_id, const std::string& a_key, bool a_persist)
{
	if (const auto& data = GetEventData(a_id, a_persist)) {
		return data->GetValue(a_key);
	}

	return nullptr;
}
void Contexts::Persist(const std::string& a_id)
{
	if (!_persistent.count(a_id)) {
		return;
	}

	std::unique_lock lock{ _locks[a_id] };

	auto context = _persistent[a_id];

	YAML::Node node{ context };
	const std::string file{ std::format("Data/SKSE/AdversityFramework/UserData/{}.yaml", a_id) };

	std::ofstream fout(file);
	fout << node;

	_dirty.erase(a_id);
}

void Contexts::PersistAll()
{
	for (const auto& [id, context] : _persistent) {
		if (_dirty[id]) {
			Persist(id);
		}
	}
}