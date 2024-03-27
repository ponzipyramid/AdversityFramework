#pragma once

namespace Adversity
{
	struct Conflict
	{
		enum Type
		{
			Unknown,
			Wear,
			Naked,
		};

		Type type;
		std::unordered_set<int> slots;
		bool With(Conflict a_other);
	};

	class Rule
	{
	public:
		enum Status
		{
			Disabled,
			Inactive,
			Reserved,
			Selected,
			Active
		};

		inline void Init(std::string a_context, std::string a_pack) {
			if (_id.empty()) {
				_context = a_context;
				
				// pack id alr contains context
				_packId = a_pack;
				_id = std::format("{}/{}", a_pack, _name);
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
		inline void SetStatus(Status a_status) { _global->value = (float) a_status; }
		bool HasTags(std::vector<std::string> a_tags, bool a_all);
		bool HasTag(std::string a_tag);
		bool Conflicts(Rule* a_rule);
	private:
		std::string _id;
		std::string _packId;
		std::string _name;
		RE::TESGlobal* _global;
		std::string _desc;
		int _severity;
		std::unordered_set<std::string> _tags;
		std::vector<Conflict> _conflicts;
		std::string _context;
		std::unordered_set<std::string> _excludes;

		friend struct YAML::convert<Rule>;
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

			return rhs.type != Conflict::Type::Unknown;
		}
	};

	template <>
	struct convert<Rule>
	{
		static bool decode(const Node& node, Rule& rhs)
		{
			rhs._name = node["name"].as<std::string>();
			rhs._desc = node["desc"].as<std::string>();
			rhs._severity = node["severity"].as<int>();
			const auto tags = node["tags"].as<std::vector<std::string>>();
			rhs._tags = std::unordered_set<std::string>{ tags.begin(), tags.end() };

			const auto globalEdid = node["global"].as<std::string>();
			rhs._global = RE::TESForm::LookupByEditorID<RE::TESGlobal>(globalEdid);

			const auto excludes = node["excludes"].as<std::vector<std::string>>();
			rhs._excludes = std::unordered_set<std::string>{ excludes.begin(), excludes.end() };
			return !rhs._name.empty() && rhs._severity >= 0 && rhs._global;
		}
	};
}