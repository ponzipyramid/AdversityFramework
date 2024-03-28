#pragma once
#include "Packs.h"
#include "Rules.h"
#include "Util.h"
#include "Willpower.h"
#include "Config.h"

namespace
{
	using namespace Adversity;

	std::vector<std::string> Filter(std::vector<std::string>& a_rules, std::function<bool(Rule* a_rule)> a_check)
	{
		auto rules = Rules::GetByIds(a_rules);
		std::vector<std::string> ids;
		rules.reserve(a_rules.size());

		for (auto id : a_rules) {
			if (auto rule = Rules::GetById(id)) {
				if (a_check(rule))
					ids.push_back(rule->GetId());
			}
		}

		return ids;
	}

	std::vector<int> Weigh(std::vector<std::string>& a_rules, std::function<int(Rule* a_rule)> a_calc)
	{
		std::vector<int> weights;
		weights.reserve(a_rules.size());

		for (auto id : a_rules) {
			if (auto rule = Rules::GetById(id))
				weights.push_back(a_calc(rule));
			else
				weights.push_back(0);
		}

		return weights;
	}
}

namespace Adversity::Papyrus
{
	constexpr std::string_view className = "AdversityFramework";

	std::vector<std::string> GetPacks(RE::StaticFunctionTag*, std::string a_context)
	{
		return Packs::GetIds(Packs::GetByContext(a_context));
	}

	RE::TESQuest* GetPackQuest(RE::StaticFunctionTag*, std::string a_pack)
	{
		if (auto pack = Packs::GetById(a_pack))
			return pack->GetQuest();
		return nullptr;
	}

	std::string GetRuleName(RE::StaticFunctionTag*, std::string a_rule)
	{
		if (auto rule = Rules::GetById(a_rule))
			return rule->GetName();
		return "";
	}

	std::string GetRuleDesc(RE::StaticFunctionTag*, std::string a_rule)
	{
		if (auto rule = Rules::GetById(a_rule))
			return rule->GetDesc();
		return "";
	}

	std::string GetRuleContext(RE::StaticFunctionTag*, std::string a_rule)
	{
		if (auto rule = Rules::GetById(a_rule))
			return rule->GetContext();
		return "";
	}

	std::string GetRulePack(RE::StaticFunctionTag*, std::string a_rule)
	{
		if (auto rule = Rules::GetById(a_rule))
			return rule->GetPackId();
		return "";
	}

	int GetRuleStatus(RE::StaticFunctionTag*, std::string a_rule)
	{
		if (auto rule = Rules::GetById(a_rule))
			return rule->GetStatus();
		return -1;
	}

	auto GetRuleTags(RE::StaticFunctionTag*, std::string a_rule)
	{
		if (auto rule = Rules::GetById(a_rule))
			return rule->GetTags();
		return std::vector<std::string>{};
	}

	std::vector<std::string> GetPackRules(RE::StaticFunctionTag*, std::string a_pack)
	{
		return Rules::GetIds(Rules::GetInPack(a_pack));
	}

	std::vector<std::string> GetContextRules(RE::StaticFunctionTag*, std::string a_context)
	{
		return Rules::GetIds(Rules::GetInContext(a_context));
	}

	std::vector<std::string> FilterRulesByStatus(RE::StaticFunctionTag*, std::vector<std::string> a_rules, int a_status)
	{
		return Filter(a_rules, [a_status](Rule* a_rule) {
			return a_rule->GetStatus() == a_status;
		});
	}

	std::vector<std::string> FilterRulesBySelectable(RE::StaticFunctionTag*, std::vector<std::string> a_rules)
	{
		std::vector<Rule*> active = Rules::GetActive();

		return Filter(a_rules, [&active](Rule* a_rule) {

			bool compatible = true;

			for (auto rule : active) {
				if (rule->GetId() == a_rule->GetId() || rule->Conflicts(a_rule)) {
					compatible = false;
					break;
				}
			}
			return compatible;
		});
	}

	std::vector<std::string> FilterRulesBySeverity(RE::StaticFunctionTag*, std::vector<std::string> a_rules, int a_severity, bool a_greater, bool a_equal)
	{
		return Filter(a_rules, [a_severity, a_greater, a_equal](Rule* a_rule) {
			const auto severity = a_rule->GetSeverity();

			if (a_equal && severity == a_severity)
				return true;
			if (a_greater && severity > a_severity)
				return true;
			if (!a_greater && severity < a_severity)
				return true;

			return false;
		});
	}

	std::vector<std::string> FilterRulesByTags(RE::StaticFunctionTag*, std::vector<std::string> a_rules, std::vector<std::string> a_tags, bool a_all, bool a_invert)
	{
		return Filter(a_rules, [&a_tags, a_all, a_invert](Rule* a_rule) {
			return a_rule->HasTags(a_tags, a_all) != a_invert;
		});
	}

	std::vector<int> WeighRulesBySeverity(RE::StaticFunctionTag*, std::vector<std::string> a_rules, int a_mode)
	{
		const auto weights = Config::Get()->weights.severity;
		auto sevWeight = weights.low;
		if (a_mode == 1) {
			sevWeight = weights.med;
		} else {
			sevWeight = weights.high;
		}

		return Weigh(a_rules, [a_mode, &sevWeight](Rule* a_rule) {
			const auto severity = a_rule->GetSeverity();
			return sevWeight[severity - 1];
		});
	}

	std::vector<int> WeighRulesByTags(RE::StaticFunctionTag*, std::vector<std::string> a_rules, std::vector<std::string> a_tags, bool a_per)
	{
		const auto weight = Config::Get()->weights.tag;

		return Weigh(a_rules, [&a_tags, a_per, weight](Rule* a_rule) {
			int total = 0;

			for (auto& tag : a_tags) {
				if (a_rule->HasTag(tag)) {
					total += weight;
					if (!a_per)
						break;
				}
			}
			
			return total;
		});
	}

	std::vector<int> SumArrays(RE::StaticFunctionTag*, std::vector<int> a_1, std::vector<int> a_2)
	{
		std::vector<int> ans;
		if (a_1.size() != a_2.size()) {
			logger::error("mismatched number of weights {} {}", a_1.size(), a_2.size());
			return ans;
		}
		
		ans.resize(a_1.size());
		for (auto i = 0; i < a_1.size(); i++) {
			ans[i] = a_1[i] + a_2[i];
		}

		return ans;
	}

	bool SetRuleStatus(RE::StaticFunctionTag*, std::string a_rule, int a_status)
	{
		if (auto rule = Rules::GetById(a_rule)) {
			rule->SetStatus((Rule::Status)a_status);
			return true;
		} else {
			logger::info("failed to find rule {}", a_rule);
		}
		return false;
	}

	float GetWillpower(RE::StaticFunctionTag*)
	{
		return Willpower::GetWillpower();
	}

	bool IsWillpowerLow(RE::StaticFunctionTag*)
	{
		return Willpower::IsWillpowerLow();
	}

	bool IsWillpowerHigh(RE::StaticFunctionTag*)
	{
		return Willpower::IsWillpowerHigh();
	}

	float GetResistance(RE::StaticFunctionTag*)
	{
		return Willpower::GetResistance();
	}

	void ModResistance(RE::StaticFunctionTag*, float a_delta)
	{
		Willpower::ModResistance(a_delta);
	}

	int GetWeightedIndex(RE::StaticFunctionTag*, std::vector<int> a_weights)
	{
		return Util::GetWeightedIndex(a_weights);
	}

	inline bool RegisterFuncs(VM* a_vm)
	{	
		// packs
		REGISTERFUNC(GetPacks)
		REGISTERFUNC(GetPackQuest)
		
		// rules
		REGISTERFUNC(GetRuleName)
		REGISTERFUNC(GetRuleDesc)
		REGISTERFUNC(GetRuleName)
		REGISTERFUNC(GetRuleContext)
		REGISTERFUNC(GetRulePack)
		REGISTERFUNC(GetRuleStatus)
		REGISTERFUNC(GetRuleTags)
		
		REGISTERFUNC(SetRuleStatus)

		REGISTERFUNC(GetPackRules)
		REGISTERFUNC(GetContextRules)

		REGISTERFUNC(FilterRulesByStatus)
		REGISTERFUNC(FilterRulesBySelectable)
		REGISTERFUNC(FilterRulesBySeverity)
		REGISTERFUNC(FilterRulesByTags)

		REGISTERFUNC(WeighRulesBySeverity)
		REGISTERFUNC(WeighRulesByTags)

		// willpower 
		REGISTERFUNC(GetWillpower)
		REGISTERFUNC(GetResistance)
		REGISTERFUNC(ModResistance)
		REGISTERFUNC(IsWillpowerLow)
		REGISTERFUNC(IsWillpowerHigh)

		// util
		REGISTERFUNC(GetWeightedIndex)
		REGISTERFUNC(SumArrays)

		return true;
	}
}