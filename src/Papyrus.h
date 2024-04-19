#pragma once
#include "Packs.h"
#include "Rules.h"
#include "Util.h"
#include "Willpower.h"
#include "Config.h"
#include "Devices.h"
#include "Outfits.h"
#include "Tattoos.h"
#include "Events.h"

namespace
{
	using namespace Adversity;

	std::vector<std::string> Filter(std::vector<PackItem*> a_items, std::function<bool(PackItem* a_rule)> a_check)
	{
		std::vector<std::string> ids;

		for (const auto item : a_items)
		{
			if (a_check(item))
			{
				ids.push_back(item->GetId());
			}
		}

		return ids;
	}

	std::vector<std::string> FilterByStatus(std::vector<PackItem*> a_items, int a_status)
	{
		return Filter(a_items, [a_status](PackItem* a_rule) {
			return a_rule->GetStatus() == a_status;
		});
	}

	std::vector<std::string> FilterBySeverity(std::vector<PackItem*> a_items, int a_severity, bool a_greater, bool a_equal)
	{
		return Filter(a_items, [a_severity, a_greater, a_equal](PackItem* a_rule) {
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

	
	std::vector<std::string> FilterByTags(std::vector<PackItem*> a_items, std::vector<std::string> a_tags, bool a_all, bool a_invert)
	{
		return Filter(a_items, [&a_tags, a_all, a_invert](PackItem* a_rule) {
			return a_rule->HasTags(a_tags, a_all) != a_invert;
		});
	}

	std::vector<int> Weigh(std::vector<PackItem*> a_items, std::function<int(PackItem* a_rule)> a_calc)
	{
		std::vector<int> weights;

		for (const auto item : a_items) {
			weights.push_back(a_calc(item));
		}

		return weights;
	}

	std::vector<int> WeighBySeverity(std::vector<PackItem*> a_items, std::vector<int> a_weights)
	{
		return Weigh(a_items, [&a_weights](PackItem* a_rule) {
			const auto severity = a_rule->GetSeverity();
			return a_weights[severity - 1];
		});
	}

	std::vector<int> WeighByTags(std::vector<PackItem*> a_items, std::vector<std::string> a_tags, bool a_per, int a_weight)
	{
		return Weigh(a_items, [&a_tags, a_per, a_weight](PackItem* a_item) {
			int total = 0;

			for (auto& tag : a_tags) {
				if (a_item->HasTag(tag)) {
					total += a_weight;
					if (!a_per)
						break;
				}
			}

			return total;
		});
	}

	bool SetStatus(PackItem* a_item, int a_status)
	{
		if (a_status > (int)PackItem::Status::Active)
			return false;

		const auto status = (PackItem::Status)a_status;

		if (status == PackItem::Status::Enabled && !a_item->ReqsMet()) {  // prevent going to neutral when reqs not met
			return false;
		}

		a_item->SetStatus(status);
		return true;
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

	std::string GetPackName(RE::StaticFunctionTag*, std::string a_pack) {
		if (const auto pack = Packs::GetById(a_pack)) {
			return pack->GetName();
		}

		return "";
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
		const auto rules{ Rules::GetByIds(a_rules) };
		return FilterByStatus(std::vector<PackItem*>{ rules.begin(), rules.end() }, a_status);
	}

	std::vector<std::string> FilterRulesBySeverity(RE::StaticFunctionTag*, std::vector<std::string> a_rules, int a_severity, bool a_greater, bool a_equal)
	{
		const auto rules{ Rules::GetByIds(a_rules) };
		return FilterBySeverity(std::vector<PackItem*>{ rules.begin(), rules.end() }, a_severity, a_greater, a_equal);
	}

	std::vector<std::string> FilterRulesByTags(RE::StaticFunctionTag*, std::vector<std::string> a_rules, std::vector<std::string> a_tags, bool a_all, bool a_invert)
	{
		const auto rules{ Rules::GetByIds(a_rules) };
		return FilterByTags(std::vector<PackItem*>{ rules.begin(), rules.end() }, a_tags, a_all, a_invert);
	}

	std::vector<std::string> FilterRulesBySelectable(RE::StaticFunctionTag*, std::vector<std::string> a_rules)
	{
		std::unordered_set<Rule*> allowed;
		const auto& active = Rules::Filter([&allowed](Rule* a_rule) {
			const auto status{ a_rule->GetStatus() };

			if (status == PackItem::Status::Reserved) {
				allowed.insert(a_rule);
				return true;
			}

			return status == PackItem::Status::Active; 
		});

		const auto rules{ Rules::GetByIds(a_rules) };
		const auto filtered{ 
			Rules::Filter(rules, [&active, &allowed](Rule* a_rule) {
				if (a_rule->GetStatus() == PackItem::Status::Disabled)
					return false;

				bool compatible = true;

				if (!allowed.contains(a_rule)) {
					for (auto rule : active) {
						if ((rule->GetId() == a_rule->GetId() && rule->GetStatus() == PackItem::Status::Active) || rule->Conflicts(a_rule)) {
							compatible = false;
							break;
						}
					}
				}

				return compatible;
			}) 
		};

		return Rules::GetIds(filtered);
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
		const auto rules{ Rules::GetByIds(a_rules) };
		const auto filtered{
			Rules::Filter(rules, [&a_currRules, &seen](Rule* a_rule) {
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
			})
		};

		return Rules::GetIds(filtered);
	}

	std::vector<int> WeighRulesBySeverity(RE::StaticFunctionTag*, std::vector<std::string> a_rules, std::vector<int> a_weights)
	{
		const auto rules{ Rules::GetByIds(a_rules) };
		return WeighBySeverity(std::vector<PackItem*>{ rules.begin(), rules.end() }, a_weights);
	}

	std::vector<int> WeighRulesByTags(RE::StaticFunctionTag*, std::vector<std::string> a_rules, std::vector<std::string> a_tags, bool a_per, int a_weight)
	{
		const auto rules{ Rules::GetByIds(a_rules) };
		return WeighByTags(std::vector<PackItem*>{ rules.begin(), rules.end() }, a_tags, a_per, a_weight);
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
			return SetStatus(rule, a_status);
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

	std::vector<RE::TESObjectARMO*> GetDevicesByKeyword(RE::StaticFunctionTag*, std::string a_context, RE::Actor* a_actor, RE::BGSKeyword* a_kwd)
	{
		return Devices::GetDevicesByKeyword(a_context, a_actor, a_kwd);
	}

	std::vector<std::string> GetOutfits(RE::StaticFunctionTag*, std::string a_context, std::string a_name)
	{
		std::vector<std::string> variants;
		if (const auto outfit = Outfits::GetOutfit(a_context, a_name)) {
			
			variants.reserve(outfit->variants.size());

			for (const auto& variant : outfit->variants) {
				logger::info("adding variant {} for {}", variant.id, a_name);
				variants.push_back(variant.id);
			}

		} else {
			logger::error("failed to find outfit: {}", a_name);
		}

		return variants;		
	}

	std::vector<RE::TESObjectARMO*> GetOutfitPieces(RE::StaticFunctionTag*, std::string a_id)
	{
		std::vector<RE::TESObjectARMO*> pieces;
		
		if (const auto variant = Outfits::GetVariant(a_id)) {
			pieces.reserve(variant->pieces.size());

			for (const auto& piece : variant->pieces) {
				pieces.push_back(piece.armo);
			}
		} else {
			logger::error("failed to find variant: {}", a_id);
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

	std::string GetEventName(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->GetName();
		}

		return "";
	}

	std::string GetEventPack(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->GetPackId();
		}

		return "";
	}

	std::string GetEventDesc(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->GetName();
		}

		return "";
	}

	bool IsExclusive(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->IsExclusive();
		}

		return false;
	}

	int GetEventStatus(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->GetStatus();
		}

		return -1;
	}

	bool SetEventStatus(RE::StaticFunctionTag*, std::string a_event, int a_status)
	{
		if (auto ev = Events::GetById(a_event)) {
			return SetStatus(ev, a_status);
		} else {
			logger::info("failed to find event {}", a_event);
		}
		return false;
	}

	void SetLock(RE::StaticFunctionTag*, bool a_enable)
	{
		Util::GetFormById<RE::TESGlobal>(0x81B)->value = (float)a_enable;
	}

	std::vector<std::string> GetEventConflictingRules(RE::StaticFunctionTag*, std::string a_event)
	{
		std::vector<std::string> rules;
		
		if (const auto ev = Events::GetById(a_event)) {
			const auto& active = Rules::Filter([ev](Rule* a_rule) {
				const auto status{ a_rule->GetStatus() };
				return status == PackItem::Status::Active && a_rule->Conflicts(ev);
			});
			rules = Rules::GetIds(active);
		}

		return rules;
	}

	std::vector<std::string> FilterEventsByStatus(RE::StaticFunctionTag*, std::vector<std::string> a_events, int a_status)
	{
		const auto events { Events::GetByIds(a_events) };
		return FilterByStatus(std::vector<PackItem*>{ events.begin(), events.end() }, a_status);
	}
	
	std::vector<std::string> FilterEventsByTags(RE::StaticFunctionTag*, std::vector<std::string> a_events, std::vector<std::string> a_tags, bool a_all, bool a_invert)
	{
		const auto events{ Events::GetByIds(a_events) };
		return FilterByTags(std::vector<PackItem*>{ events.begin(), events.end() }, a_tags, a_all, a_invert);
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
		REGISTERFUNC(GetPackName)
		
		// rules
		REGISTERFUNC(GetRuleName)
		REGISTERFUNC(GetRuleDesc)
		REGISTERFUNC(GetRuleHint)
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

		// events
		REGISTERFUNC(GetEventName)
		REGISTERFUNC(GetEventPack)
		REGISTERFUNC(GetEventDesc)
		REGISTERFUNC(GetEventStatus)
		REGISTERFUNC(SetEventStatus)
		REGISTERFUNC(IsExclusive)
		REGISTERFUNC(SetLock)
		REGISTERFUNC(GetEventConflictingRules)
		REGISTERFUNC(FilterEventsByStatus)
		REGISTERFUNC(FilterEventsByTags)

		// willpower 
		REGISTERFUNC(GetWillpower)
		REGISTERFUNC(GetResistance)
		REGISTERFUNC(ModResistance)
		REGISTERFUNC(IsWillpowerLow)
		REGISTERFUNC(IsWillpowerHigh)

		// util
		REGISTERFUNC(GetWeightedIndex)
		REGISTERFUNC(SumArrays)
		REGISTERFUNC(FilterByPrefix)
		REGISTERFUNC(RemovePrefix)

		// devices
		REGISTERFUNC(GetDevicesByKeyword)

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