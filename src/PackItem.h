#pragma once

#include "Util.h"

namespace Adversity
{
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
		bool ReqsMet();

	protected:
		std::string _id;
		std::string _packId;
		std::string _name;
		RE::TESGlobal* _global = nullptr;
		std::string _desc;
		int _severity;
		std::unordered_set<std::string> _tags;
		std::string _context;
		std::vector<std::string> _reqs;
	};
}