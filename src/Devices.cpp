#include "Devices.h"
#include "Util.h"

using namespace Adversity;

void Devices::Init()
{
	_lockableKwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("zad_Lockable");
	Devices::Load("default");
}

void Devices::Load(std::string a_context)
{
	const std::string base{ "data/skse/adversityframework" };
	const std::string path{ a_context == "default" ? std::format("{}/devices.json", base) : std::format("{}/contexts/{}/devices.json", base, a_context) };

	if (!fs::exists(path)) {
		logger::warn("{} context has no devices config", a_context);
		return;
	}
	
	try {
		std::ifstream file(path);
		auto data = json::parse(file);
		for (auto& [key, node] : data.items()) {
			auto field = Util::Lower(key);
			if (field == "filters") {
				_filters[a_context] = node.template get<std::vector<std::string>>();
			} else {
				_devices[a_context][field] = node.template get<std::vector<Device>>();
			}
		}

		logger::info("loaded devices from {} successfully", a_context);
	} catch (std::exception& e) {
		logger::error("failed to parse devices {} due to {}", path, e.what());
	} catch (...) {
		logger::error("failed to parse devices {}", path);
	}
}

std::vector<RE::TESObjectARMO*> Devices::GetDevicesByKeyword(std::string a_context, RE::BGSKeyword* a_kwd)
{
	std::vector<RE::TESObjectARMO*> devices;
	std::vector<RE::TESObjectARMO*> filtered;

	const auto& deviceMap = _devices.count(a_context) ? _devices[a_context] : _devices["default"];
	const std::string kwdName{ Util::Lower(a_kwd->GetFormEditorID()) };
	
	if (deviceMap.count(kwdName)) {
		const auto& [_, devicesList] = *deviceMap.find(kwdName);
		const auto& filters = _filters[a_context];

		devices.reserve(devicesList.size());
		filtered.reserve(devicesList.size());

		for (const auto& device : devicesList) {
			if (!device.armo) {
				logger::info("failed to find {}", device.name);
				continue;
			}

			devices.push_back(device.armo);

			bool matches = filters.empty();

			for (const auto& filter : filters) {
				if (device.name.contains(filter)) {
					matches = true;
					break;
				}
			}

			if (matches) {
				filtered.push_back(device.armo);
			}
		}
	} else {
		logger::error("Devices::GetDevicesByKeyword: unable to find devices for {}", kwdName);
	}

	logger::info("devices found: {} {}", devices.size(), filtered.size());
	return filtered.empty() ? devices : filtered;
}

std::vector<RE::TESObjectARMO*> Devices::FilterRenderedByWorn(std::vector<RE::TESObjectARMO*> a_devices, std::vector<RE::TESObjectARMO*> a_worn)
{
	std::set<RE::BGSKeyword*> kwdSet;
	for (auto worn : a_worn) {
		if (!worn)
			continue;

		for (auto kwd : worn->GetKeywords()) {
			std::string name{ kwd->GetFormEditorID() };
			if (Util::Lower(name).starts_with("zad_devious")) {
				kwdSet.insert(kwd);
			}
		}
	}

	std::vector<RE::BGSKeyword*> kwds{ kwdSet.begin(), kwdSet.end() }; 

	std::vector<RE::TESObjectARMO*> valid;

	std::copy_if(a_devices.begin(), a_devices.end(), std::back_inserter(valid), [=](RE::TESObjectARMO* device) {
		return device && !device->HasKeywordInArray(kwds, false);
	});

	return valid;
}