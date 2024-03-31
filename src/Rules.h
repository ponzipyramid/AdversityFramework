#pragma once

#include "Rule.h"
#include "Util.h"

namespace Adversity
{
	class Rules
	{
	public:
		static void Load(std::string a_context, std::string a_pack, std::vector<Rule>& a_rules);
		static Rule* GetById(std::string a_id);
		static std::vector<Rule*> GetByIds(std::vector<std::string> a_ids);
		static std::vector<Rule*> GetInContext(std::string a_context);
		static std::vector<Rule*> GetInPack(std::string a_pack);
		static std::vector<std::string> GetIds(std::vector<Rule*> a_rules);
		static std::vector<Rule*> Filter(std::function<bool(Rule* a_rule)> a_check);
	private:
		static inline std::unordered_map<std::string, Rule> _rules;
		static inline std::unordered_map<std::string, std::vector<Rule*>> _contexts;
		static inline std::unordered_map<std::string, std::vector<Rule*>> _packs;
	};
}