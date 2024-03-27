#include "Rules.h"
#include "Config.h"

using namespace Adversity;

void Rules::Load(std::string a_context, std::string a_pack, std::vector<Rule>& a_rules)
{
	for (auto& rule : a_rules) {
		rule.Init(a_context, a_pack);
		_rules[rule.GetId()] = &rule;
	}
}

Rule* Rules::GetById(std::string a_id)
{
	return _rules.count(a_id) ? _rules[a_id] : nullptr;
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

std::vector<Rule*> Rules::GetActive()
{
	std::vector<Rule*> active;

	for (auto [_, rule] : _rules) {
		if (rule->GetStatus() == Rule::Status::Active)
			active.push_back(rule);
	}

	return active;
}