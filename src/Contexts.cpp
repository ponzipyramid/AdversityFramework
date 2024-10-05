#include "Contexts.h"
#include "Packs.h"
#include "Devices.h"
#include "Actors.h"
#include "Serialization.h"

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
				auto context = YAML::LoadFile(userData).as<Context>();
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

void Contexts::Persist(const std::string& a_id)
{
	if (!_persistent.count(a_id)) {
		return;
	}

	std::unique_lock lock{ _locks[a_id] };

	auto context = _persistent[a_id];


	YAML::Node node{ context };
	const std::string file{ std::format("Data/SKSE/AdversityFramework/UserData/{}.yaml", a_id) };

	logger::info("persisting context: {} {}", a_id, file);

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

void Contexts::Save(SKSE::SerializationInterface* a_intfc)
{
	Serialization::Write(a_intfc, _runtime.size());
	for (const auto& [id, context] : _runtime) {
		Serialization::Write(a_intfc, id);
		context.Serialize(a_intfc);
	}
}

void Contexts::Load(SKSE::SerializationInterface* a_intfc)
{
	auto i = Serialization::Read<std::size_t>(a_intfc);
	for (; i > 0; i--) {
		const auto& id = Serialization::Read<std::string>(a_intfc);
		Context context{ id, a_intfc };
		_runtime[context.GetId()] = context;
	}
}

void Contexts::Revert()
{
	_runtime.clear();
}