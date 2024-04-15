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
			Clean
		};

		Type type;
		std::unordered_set<int> slots;
		bool exclusive;
		bool With(Conflict a_other);
	};

	class PackItem
	{
	public:
		enum Status
		{
			Disabled,
			Inactive,
			Reserved,
			Selected,
			Paused,
			Active
		};

		inline void Init(std::string a_context, std::string a_pack)
		{
			if (_id.empty()) {
				_context = a_context;

				// pack id alr contains context
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
		inline void SetStatus(Status a_status) { _global->value = (float)a_status; }
		bool HasTags(std::vector<std::string> a_tags, bool a_all);
		bool HasTag(std::string a_tag);
		bool Conflicts(PackItem* a_rule);
		bool ReqsMet();
	protected:
		std::string _id;
		std::string _packId;
		std::string _name;
		RE::TESGlobal* _global = nullptr;
		std::string _desc;
		int _severity;
		std::vector<Conflict> _conflicts;
		std::unordered_set<std::string> _tags;
		std::string _context;
		std::vector<std::string> _reqs;
		std::unordered_set<std::string> _excludes;
		std::unordered_set<std::string> _compatible;
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
			const auto slots = node["slots"].as<std::vector<int>>();
			rhs.slots = std::unordered_set<int>{ slots.begin(), slots.end() };
			rhs.exclusive = node["exclusive"].as<std::string>("") == "true";
			return rhs.type != Conflict::Type::Unknown;
		}
	};
}