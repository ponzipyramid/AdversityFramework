#pragma once

#include "PackItem.h"

namespace Adversity
{
	class Event : public PackItem
	{
	public:
		inline bool IsCooldownComplete() { return _timer->value <= Util::GetGameTime(); }
		inline bool SetCooldown(float a_delta) { return _timer->value = Util::GetGameTime() + a_delta; }
		inline bool IsExclusive() { return _exclusive; }
		inline bool IsValid() { return !_name.empty() && _global; }
	private:
		RE::TESGlobal* _timer;
		bool _exclusive;

		friend struct YAML::convert<Event>;
	};
}

namespace YAML
{

	template <>
	struct convert<Event>
	{
		static bool decode(const Node& node, Event& rhs)
		{
			rhs._name = node["name"].as<std::string>();
			rhs._desc = node["desc"].as<std::string>("");
			rhs._severity = node["severity"].as<int>(0);
			const auto tags = node["tags"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._tags = std::unordered_set<std::string>{ tags.begin(), tags.end() };

			const auto global = node["global"].as<std::string>();
			rhs._global = RE::TESForm::LookupByEditorID<RE::TESGlobal>(global);

			const auto timer = node["timer"].as<std::string>("");
			if (!timer.empty())
				rhs._timer = RE::TESForm::LookupByEditorID<RE::TESGlobal>(timer);

			rhs._reqs = node["requirements"].as<std::vector<std::string>>(std::vector<std::string>{});

			const auto excludes = node["excludes"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._excludes = std::unordered_set<std::string>{ excludes.begin(), excludes.end() };

			const auto compatible = node["compatible"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs._compatible = std::unordered_set<std::string>{ compatible.begin(), compatible.end() };

			rhs._exclusive = node["exclusive"].as<std::string>("true") == "true";

			return true;
		}
	};
}