#pragma once
#include "Packs.h"
#include "Rules.h"
#include "Util.h"
#include "Willpower.h"
#include "Config.h"
#include "Devices.h"
#include "Outfits.h"
#include "Tattoos.h"

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

	std::vector<std::string> GetContextRules(RE::StaticFunctionTag*, std::string a_context)
	{
		return Rules::GetIds(Rules::GetInContext(a_context));
	}

	std::vector<std::string> GetContextTags(RE::StaticFunctionTag*, std::string a_context)
	{
		const auto& rules = Rules::GetInContext(a_context);
		std::set<std::string> tags;

		for (auto rule : rules) {
			for (const auto& tag : rule->GetTags()) {
				tags.insert(tag);
			}
		}

		return std::vector<std::string>{ tags.begin(), tags.end() };
	}

	std::vector<std::string> GetPackRules(RE::StaticFunctionTag*, std::string a_pack)
	{
		return Rules::GetIds(Rules::GetInPack(a_pack));
	}

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

	std::string GetRuleHint(RE::StaticFunctionTag*, std::string a_rule)
	{
		if (auto rule = Rules::GetById(a_rule))
			return rule->GetHint().empty() ? rule->GetDesc() : rule->GetHint();
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

	std::vector<std::string> FilterRulesByStatus(RE::StaticFunctionTag*, std::vector<std::string> a_rules, int a_status)
	{
		return Filter(a_rules, [a_status](Rule* a_rule) {
			return a_rule->GetStatus() == a_status;
		});
	}

	std::vector<std::string> FilterRulesBySelectable(RE::StaticFunctionTag*, std::vector<std::string> a_rules)
	{
		std::unordered_set<Rule*> allowed;
		const auto& active = Rules::Filter([&allowed](Rule* a_rule) {
			const auto status{ a_rule->GetStatus() };

			if (status == Rule::Status::Reserved) {
				allowed.insert(a_rule);
				return true;
			}

			return status == Rule::Status::Active; 
		});

		return Filter(a_rules, [&active, &allowed](Rule* a_rule) {

			bool compatible = true;

			if (!allowed.contains(a_rule)) {
				for (auto rule : active) {
					if ((rule->GetId() == a_rule->GetId() && rule->GetStatus() == Rule::Status::Active) || rule->Conflicts(a_rule)) {
						compatible = false;
						break;
					}
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

	std::vector<std::string> FilterRulesByRequirements(RE::StaticFunctionTag*, std::vector<std::string> a_rules, std::vector<std::string> a_currRules)
	{
		std::set<std::string> seen;
		for (auto& id : a_currRules) {
			if (auto rule = Rules::GetById(id)) {
				for (const auto& tag : rule->GetTags()) {
					seen.insert(tag);
				}
			}
		}

		// TODO: tolerate OR operators
		return Filter(a_rules, [&a_currRules, &seen](Rule* a_rule) {
			const auto& tags = a_rule->GetTags();
			const auto& reqs = Util::FilterByPrefix(tags, "requires");

			for (const auto& req : reqs) {

				const auto value{ Util::RemovePrefix(req, "requires:") };

				const auto& splits = Util::Split(value, "|");

				bool sat = splits.empty();

				for (auto& split : splits) {
					if (seen.contains(split)) {
						sat = true;
						break;
					}
				}
				
				if (!sat)
					return false;
			}

			return true;
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
		if (a_status > (int)Rule::Status::Active)
			return false;

		const auto status = (Rule::Status)a_status;
		
		if (auto rule = Rules::GetById(a_rule)) {
			if (status == Rule::Status::Inactive && !rule->ReqsMet()) { // prevent going to neutral when reqs not met 
				return false;
			}
			rule->SetStatus(status);
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

	std::vector<RE::TESObjectARMO*> ArmorArray(RE::StaticFunctionTag*, unsigned int a_size, RE::TESObjectARMO* a_fill) 
	{
		return std::vector<RE::TESObjectARMO*>{ a_size, a_fill };
	}

	std::vector<std::string> FilterByPrefix(RE::StaticFunctionTag*, std::vector<std::string> a_strs, std::string a_prefix)
	{
		return Util::FilterByPrefix(a_strs, a_prefix);
	}

	std::vector<std::string> RemovePrefix(RE::StaticFunctionTag*, std::vector<std::string> a_strs, std::string a_prefix)
	{
		std::vector<std::string> transform;
		std::size_t len{ a_prefix.size() };

		std::transform(a_strs.begin(), a_strs.end(), std::back_inserter(transform), [&a_prefix, len](std::string a_str) {			
			return Util::RemovePrefix(a_str, a_prefix);
		});

		return transform;
	}

	std::vector<RE::TESObjectARMO*> GetDevicesByKeyword(RE::StaticFunctionTag*, std::string a_context, RE::BGSKeyword* a_kwd)
	{
		return Devices::GetDevicesByKeyword(a_context, a_kwd);
	}

	std::vector<RE::TESObjectARMO*> FilterRenderedByWorn(RE::StaticFunctionTag*, std::vector<RE::TESObjectARMO*> a_devices, std::vector<RE::TESObjectARMO*> a_kwds)
	{
		return Devices::FilterRenderedByWorn(a_devices, a_kwds);
	}

	std::vector<std::string> GetOutfits(RE::StaticFunctionTag*, std::string a_context, std::string a_name)
	{
		const auto outfit = Outfits::GetOutfit(a_context, a_name);
		std::vector<std::string> variants;
		variants.reserve(outfit->variants.size());

		for (const auto& variant : outfit->variants) {
			variants.push_back(variant.id);
		}

		return variants;
	}

	std::vector<RE::TESObjectARMO*> GetOutfitPieces(RE::StaticFunctionTag*, std::string a_id)
	{
		const auto variant = Outfits::GetVariant(a_id);
		std::vector<RE::TESObjectARMO*> pieces;
		pieces.reserve(variant->pieces.size());
		
		for (const auto& piece : variant->pieces) {
			pieces.push_back(piece.armo);
		}

		return pieces;
	}

	int GetNumGroups(RE::StaticFunctionTag*, std::string a_context, std::string a_name)
	{
		return (int)Tattoos::GetGroups(a_context, a_name).size();
	}

	std::vector<std::string> GetTattooGroup(RE::StaticFunctionTag*, std::string a_context, std::string a_name, int a_index)
	{
		const auto& groups = Tattoos::GetGroups(a_context, a_name);
		std::vector<std::string> tattoos;

		if (a_index < groups.size()) {
			const auto& tats = groups[a_index]->tattoos;
			for (const auto& tat : tats) {
				tattoos.push_back(std::format("{}<>{}", tat.section, tat.name));
			}
		}

		return tattoos;
	}

	bool ValidateOutfits(RE::StaticFunctionTag*, std::vector<std::string> a_ids)
	{
		return Outfits::Validate(a_ids);
	}

	inline bool RegisterFuncs(VM* a_vm)
	{	
		// contexts
		REGISTERFUNC(GetContextRules)
		REGISTERFUNC(GetContextTags)
		REGISTERFUNC(GetPacks)
		
		// packs
		REGISTERFUNC(GetPackQuest)
		REGISTERFUNC(GetPackRules)
		
		// rules
		REGISTERFUNC(GetRuleName)
		REGISTERFUNC(GetRuleDesc)
		REGISTERFUNC(GetRuleHint)
		REGISTERFUNC(GetRuleName)
		REGISTERFUNC(GetRuleContext)
		REGISTERFUNC(GetRulePack)
		REGISTERFUNC(GetRuleStatus)
		REGISTERFUNC(GetRuleTags)
		
		REGISTERFUNC(SetRuleStatus)

		REGISTERFUNC(FilterRulesByStatus)
		REGISTERFUNC(FilterRulesBySelectable)
		REGISTERFUNC(FilterRulesBySeverity)
		REGISTERFUNC(FilterRulesByTags)
		REGISTERFUNC(FilterRulesByRequirements)

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
		REGISTERFUNC(ArmorArray)
		REGISTERFUNC(FilterByPrefix)
		REGISTERFUNC(RemovePrefix)

		// devices
		REGISTERFUNC(GetDevicesByKeyword)
		REGISTERFUNC(FilterRenderedByWorn)

		// outfits
		REGISTERFUNC(GetOutfits)
		REGISTERFUNC(GetOutfitPieces)
		REGISTERFUNC(ValidateOutfits)

		// tattoos
		REGISTERFUNC(GetNumGroups)
		REGISTERFUNC(GetTattooGroup)

		return true;
	}
}