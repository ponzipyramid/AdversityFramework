#include "Rules.h"
#include "Config.h"
#include "Util.h"

using namespace Adversity;

void Rules::Load(std::string a_context, std::string a_pack, std::vector<Rule>& a_rules)
{
	for (auto& rule : a_rules) {

		rule.Init(a_context, a_pack);

		logger::info("loading rule {} {} {}", a_context, a_pack, rule.GetId());
		
		_rules[rule.GetId()] = rule;

		auto ref = GetById(rule.GetId());
		_contexts[a_context].push_back(ref);
		_packs[a_pack].push_back(ref);
	}
}

Rule* Rules::GetById(std::string a_id)
{
	a_id = Util::Lower(a_id);
	return _rules.count(a_id) ? &_rules[a_id] : nullptr;
}

std::vector<Rule*> Rules::GetByIds(std::vector<std::string> a_ids)
{
	std::vector<Rule*> rules;

	for (auto id : a_ids) {
		if (auto rule = GetById(id))
			rules.push_back(rule);
	}

	return rules;
}

std::vector<Rule*> Rules::GetInContext(std::string a_context)
{
	return _contexts[a_context];
}

std::vector<Rule*> Rules::GetInPack(std::string a_pack)
{
	return _packs[a_pack];
}

std::vector<std::string> Rules::GetIds(std::vector<Rule*> a_rules)
{
	std::vector<std::string> ids;
	for (auto rule : a_rules) {
		ids.push_back(rule->GetId());
	}

	return ids;
}

std::vector<Rule*> Rules::Filter(const std::function<bool(Rule* a_rule)> a_check)
{
	std::vector<Rule*> filtered;

	for (auto& [_, rule] : _rules) {
		if (a_check(&rule))
			filtered.push_back(&rule);
	}

	return filtered;
}

std::vector<Rule*> Rules::Filter(const std::vector<Rule*> a_rules, std::function<bool(Rule* a_rule)> a_check)
{
	std::vector<Rule*> filtered;

	for (auto rule : a_rules) {
		if (a_check(rule))
			filtered.push_back(rule);
	}

	return filtered;
}