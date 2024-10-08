#include "Devices.h"
#include "Util.h"

using namespace Adversity;

void Devices::Init()
{
	_lockableKwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("zad_Lockable");
	_deviceKwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("zad_InventoryDevice");

	_excludeKwds.push_back(_lockableKwd);
	if (const auto& plugKwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("zad_DeviousPlug")) {
		_excludeKwds.push_back(plugKwd);
	}

	Devices::Load("default");
}

void Devices::Load(std::string a_context)
{
	const std::string base{ "data/skse/adversityframework" };
	const std::string path{ a_context == "default" ? std::format("{}/devices.json", base) : std::format("{}/contexts/{}/Config/devices.json", base, a_context) };

	if (!fs::exists(path)) {
		logger::warn("{} context has no devices config", a_context);
		return;
	}
	
	try {
		std::ifstream file(path);
		auto data = json::parse(file);
		for (auto& [key, node] : data.items()) {
			auto field = Util::Lower(key);
			if (field == "sets") {
				const auto sets = node.template get<std::vector<std::string>>();
				_sets[a_context].reserve(sets.size());
				std::transform(sets.begin(), sets.end(), std::back_inserter(_sets[a_context]), [](std::string a_str) {
					return Util::Lower(a_str);
				});
			} else if (field.starts_with("zad_")) {

				const auto devices = node.template get<std::vector<Device>>();

				std::vector<Device> valid;
				std::copy_if(devices.begin(), devices.end(), std::back_inserter(valid), [=](Device a_device) {
					return a_device.armo != nullptr;
				});

				_devices[a_context][field] = valid;
				for (auto& device : _devices[a_context][field]) {
					_pieces[device.armo->GetFormID()] = &device;
				}
			}
		}

		logger::info("loaded devices from {} successfully", a_context);
	} catch (std::exception& e) {
		logger::error("failed to parse devices {} due to {}", path, e.what());
	} catch (...) {
		logger::error("failed to parse devices {}", path);
	}
}

std::vector<RE::TESObjectARMO*> Devices::GetDevicesByKeyword(std::string a_context, RE::Actor* a_actor, RE::BGSKeyword* a_kwd)
{
	logger::info("GetDevicesByKeyword: Start - {} {} {}", a_context, a_actor->GetName(), a_kwd->GetFormEditorID());

	const auto inv = a_actor->GetInventory([](RE::TESBoundObject& a_object) {
		return a_object.IsArmor();
	}, false);

	const auto& sets = _sets[a_context];

	std::set<RE::BGSKeyword*> seenKwds;
	std::set<std::string> wornSets;
	std::set<std::string> hasSets;

	for (const auto& [item, invData] : inv) {
		const auto& [count, entry] = invData;
		if (count > 0) {
			const auto armor = item->As<RE::TESObjectARMO>();
			if (armor->HasKeyword(_deviceKwd)) { // inventory device
				const std::string name{ armor->GetName() };
				
				for (const auto& set : sets) {
					if (Util::Lower(name).starts_with(set)) {
						hasSets.insert(set);

						if (entry->IsWorn()) {
							wornSets.insert(set);
						}
					}
				}
			} else if (armor->HasKeyword(_lockableKwd)) { // rendered device
				for (const auto& kwd : armor->GetKeywords()) {
					std::string name{ kwd->GetFormEditorID() };
					if (Util::Lower(name).starts_with("zad_devious")) {
						seenKwds.insert(kwd);
					}
				}
			}
		}
	}

	std::vector<RE::BGSKeyword*> kwds{ seenKwds.begin(), seenKwds.end() };

	std::vector<RE::TESObjectARMO*> valid;
	std::vector<RE::TESObjectARMO*> filteredBySet;
	std::vector<RE::TESObjectARMO*> filteredByWornSets;
	std::vector<RE::TESObjectARMO*> filteredByHasSets;

	const auto& deviceMap = _devices.count(a_context) ? _devices[a_context] : _devices["default"];
	const std::string kwdName{ Util::Lower(a_kwd->GetFormEditorID()) };
	
	if (deviceMap.count(kwdName)) {
		const auto& [_, devicesList] = *deviceMap.find(kwdName);

		valid.reserve(devicesList.size());
		filteredBySet.reserve(devicesList.size());
		filteredByWornSets.reserve(devicesList.size());

		for (const auto& device : devicesList) {
			const auto armo = device.armo;

			if (!armo) {
				continue;
			}

			if (!armo->HasKeywordInArray(kwds, false)) {
				valid.push_back(device.armo);

				for (const auto& set : sets) {
					if (Util::Lower(device.name).starts_with(set)) {
						filteredBySet.push_back(armo);

						if (hasSets.contains(set)) {
							filteredByHasSets.push_back(armo);
						}

						if (wornSets.contains(set)) {
							filteredByWornSets.push_back(armo);
						}

					}
				}
			}
		}
	} else {
		logger::error("Devices::GetDevicesByKeyword: unable to find devices for {}", kwdName);
	}

	logger::info("GetDevicesByKeyword: End - {} {} {}", filteredByWornSets.size(), filteredBySet.size(), valid.size());

	if (!filteredByWornSets.empty())
		return filteredByWornSets;

	if (!filteredByHasSets.empty())
		return filteredByHasSets;

	if (!filteredBySet.empty())
		return filteredBySet;

	return valid;
}

bool Devices::DeviceMatches(std::string a_name, std::vector<std::string> a_filters)
{
	for (const auto& cont : a_filters) {
		if (Util::Lower(a_name).find(Util::Lower(cont)) != std::string::npos) {
			return true;
		}
	}

	return false;
}