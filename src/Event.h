#pragma once

#include "Util.h"

namespace Adversity
{
	struct Conflict
	{
		enum Type
		{
			Unknown,
			Wear,
			Naked,
			Filth,
			Outfit,
			HeavyBondage,
			Clean
		};

		Type type;
		std::unordered_set<int> slots;
		bool exclusive;
		bool With(Conflict a_other);
	};

	class Event
	{
	public:
		enum Status
		{
			Disabled,
			Enabled,
			Reserved,
			Selected,
			Paused,
			Active
		};

		inline void Init(std::string a_context, std::string a_pack)
		{
			if (_id.empty()) {
				_context = a_context;
				_packId = a_pack;
				_id = std::format("{}/{}", a_pack, Util::Lower(_name));
			}
		}
		inline std::string GetPackId() { return _packId; }
		inline std::string GetName() { return _name; }
		inline std::string GetDesc() { return _desc; }
		inline std::vector<std::string> GetTags() { return std::vector<std::string>{ _tags.begin(), _tags.end() }; }
		inline std::string GetId() { return _id; }
		inline std::string GetContext() { return _context; }
		inline int GetSeverity() { return _severity; }
		inline Status GetStatus() { return static_cast<Status>(_global->value); }
		inline void SetStatus(Status a_status)
		{
			logger::info("setting {} to {}", _global->GetFormEditorID(), (int)a_status);
			_global->value = (float)a_status;
		}
		bool HasTags(std::vector<std::string> a_tags, bool a_all);
		bool HasTag(std::string a_tag);
		bool Conflicts(Event* a_rule);
		bool ReqsMet();
		inline bool IsCooldownComplete() { return _timer->value <= Util::GetGameTime(); }
		inline bool SetCooldown(float a_delta) { return _timer->value = Util::GetGameTime() + a_delta; }
		inline bool IsExclusive() { return _exclusive; }
		inline bool IsValid() { return !_name.empty() && _global; }
		inline std::vector<RE::BGSKeyword*> GetKwds() { return _kwds; }
	private:
		std::string _id;
		std::string _packId;
		std::string _name;
		RE::TESGlobal* _global = nullptr;
		std::string _desc;
		std::vector<Conflict> _conflicts;
		std::unordered_set<std::string> _tags;
		std::string _context;
		std::vector<std::string> _reqs;
		std::unordered_set<std::string> _excludes;
		std::unordered_set<std::string> _compatible;
		
		std::vector<RE::BGSKeyword*> _kwds;

		RE::TESGlobal* _timer;
		bool _exclusive;
		int _severity;

		friend struct YAML::convert<Event>;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<Conflict>
	{
		static bool decode(const Node& node, Conflict& rhs)
		{
			const auto type = node["type"].as<std::string>();
			rhs.type = magic_enum::enum_cast<Conflict::Type>(type, magic_enum::case_insensitive).value_or(Conflict::Type::Unknown);
			const auto slots = node["slots"].as<std::vector<int>>(std::vector<int>{});
			rhs.slots = std::unordered_set<int>{ slots.begin(), slots.end() };
			rhs.exclusive = node["exclusive"].as<std::string>("") == "true";
			return rhs.type != Conflict::Type::Unknown;
		}
	};

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

			const auto kwds = node["keywords"].as<std::vector<std::string>>(std::vector<std::string>{});
			for (const auto kwdStr : kwds) {
				if (const auto& kwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(kwdStr)) {
					rhs._kwds.push_back(kwd);
				}
			}

			rhs._conflicts = node["conflicts"].as<std::vector<Conflict>>(std::vector<Conflict>{});

			rhs._exclusive = node["exclusive"].as<std::string>("") == "true";

			return true;
		}
	};
}