#pragma once

namespace Adversity
{
	struct Device
	{
		std::string name;
		RE::FormID formId;
		std::string espName;
		RE::TESObjectARMO* armo;
	};

	inline void from_json(const json& j, Device& d)
	{
		j.at("name").get_to(d.name);
		j.at("espName").get_to(d.espName);

		auto formId = j.at("formId").template get<std::string>();
		d.formId = (int)strtol(formId.c_str(), NULL, 0);

		d.armo = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(d.formId, d.espName);
	}

	class Devices
	{
	public:
		static void Init();
		static void Load(std::string a_context);
		static std::vector<RE::TESObjectARMO*> GetDevicesByKeyword(std::string a_context, RE::BGSKeyword* a_kwd);
		static std::vector<RE::TESObjectARMO*> FilterRenderedByWorn(std::vector<RE::TESObjectARMO*> a_devices, std::vector<RE::TESObjectARMO*> a_worn);
		static inline RE::BGSKeyword* GetLockableKwd() { return _lockableKwd; }
	private:
		static inline std::unordered_map<std::string, std::unordered_map<std::string, std::vector<Device>>> _devices;
		static inline std::unordered_map<std::string, std::vector<std::string>> _filters;
		static inline RE::BGSKeyword* _lockableKwd;
	};
}

