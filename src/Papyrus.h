#pragma once
#include "Packs.h"
#include "Util.h"
#include "Willpower.h"
#include "Devices.h"
#include "Outfits.h"
#include "Tattoos.h"
#include "Events.h"
#include "Actors.h"

namespace
{
	using namespace Adversity;

	std::vector<std::string> Filter(std::vector<Event*> a_items, std::function<bool(Event* a_event)> a_check)
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
}

namespace Adversity::Papyrus
{
	std::vector<std::string> GetContextEvents(RE::StaticFunctionTag*, std::string a_context)
	{
		return Events::GetIds(Events::GetInContext(a_context));
	}

	std::vector<std::string> GetContextTags(RE::StaticFunctionTag*, std::string a_context)
	{
		const auto& events = Events::GetInContext(a_context);
		std::set<std::string> tags;

		for (auto ev : events) {
			for (const auto& tag : ev->GetTags()) {
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
	std::vector<std::string> GetPackEvents(RE::StaticFunctionTag*, std::string a_pack)
	{
		return Events::GetIds(Events::GetInPack(a_pack));
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
		if (!a_actor || !a_kwd) {
			return std::vector<RE::TESObjectARMO*>{};
		}

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
		} else {
			logger::info("failed to find event {}", a_event);
		}

		return "";
	}

	std::string GetEventPack(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->GetPackId();
		} else {
			logger::info("failed to find event {}", a_event);
		}

		return "";
	}

	std::string GetEventDesc(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->GetDesc().empty() ? ev->GetName() : ev->GetDesc();
		} else {
			logger::info("failed to find event {}", a_event);
		}

		return "";
	}

	auto GetEventTags(RE::StaticFunctionTag*, std::string a_event)
	{
		if (auto ev = Events::GetById(a_event))
			return ev->GetTags();
		return std::vector<std::string>{};
	}

	auto GetEventSeverity(RE::StaticFunctionTag*, std::string a_event)
	{
		if (auto ev = Events::GetById(a_event))
			return ev->GetSeverity();
		return -1;
	}

	bool IsExclusive(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->IsExclusive();
		} else {
			logger::info("failed to find event {}", a_event);
		}

		return false;
	}

	int GetEventStatus(RE::StaticFunctionTag*, std::string a_event)
	{
		if (const auto ev = Events::GetById(a_event)) {
			return ev->GetStatus();
		} else {
			logger::info("failed to find event {}", a_event);
		}

		return -1;
	}

	bool SetEventStatus(RE::StaticFunctionTag*, std::string a_event, int a_status)
	{
		if (auto ev = Events::GetById(a_event)) {
			if (a_status > (int)Event::Status::Active)
				return false;

			const auto status = (Event::Status)a_status;

			// prevent any state except disabled when reqs are not met
			if (!ev->ReqsMet() && a_status != Event::Status::Disabled) {
				return false;
			}

			ev->SetStatus(status);
			return true;
		} else {
			logger::info("failed to find event {}", a_event);
		}
		return false;
	}

	void SetLock(RE::StaticFunctionTag*, bool a_enable)
	{
		Util::GetFormById<RE::TESGlobal>(0x81B)->value = (float)a_enable;
	}

	std::vector<std::string> FilterEventsByStatus(RE::StaticFunctionTag*, std::vector<std::string> a_events, int a_status)
	{
		const auto events { Events::GetByIds(a_events) };
		return Filter(events, [a_status](Event* a_rule) {
			return a_rule->GetStatus() == a_status;
		});
	}

	std::vector<std::string> FilterEventsBySeverity(RE::StaticFunctionTag*, std::vector<std::string> a_events, int a_severity, bool a_greater, bool a_equal)
	{
		const auto events{ Events::GetByIds(a_events) };
		return Filter(events, [a_severity, a_greater, a_equal](Event* a_rule) {
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
	
	std::vector<std::string> FilterEventsByTags(RE::StaticFunctionTag*, std::vector<std::string> a_events, std::vector<std::string> a_tags, bool a_all, bool a_invert)
	{
		const auto events{ Events::GetByIds(a_events) };
		return Filter(events, [&a_tags, a_all, a_invert](Event* a_rule) {
			return a_rule->HasTags(a_tags, a_all) != a_invert;
		});
	}

	std::vector<std::string> FilterEventsByConflict(RE::StaticFunctionTag*, std::vector<std::string> a_events)
	{
		const auto& active = Events::Filter([](Event* a_rule) {
			const auto status{ a_rule->GetStatus() };
			return status == Event::Status::Active || status == Event::Status::Reserved;
		});

		const auto events{ Events::GetByIds(a_events) };
		const auto filtered{
			Events::Filter(events, [&active](Event* a_rule) {
				if (a_rule->GetStatus() == Event::Status::Disabled)
					return false;

				bool compatible = true;

				for (auto rule : active) {
					if (rule->Conflicts(a_rule) || rule->GetId() == a_rule->GetId()) {
						compatible = false;
						break;
					}
				}

				return compatible;
			})
		};

		return Events::GetIds(filtered);
	}
	inline std::vector<int> WeighEventsByActor(RE::StaticFunctionTag*, std::string a_context, RE::Actor* a_actor, std::vector<std::string> a_events, int a_weight, bool a_considerDislikes, bool a_stack)
	{		
		const auto traits = Actors::GetTraits(a_context, a_actor);
		std::vector<int> weights;
		for (const auto& id : a_events) {
			int weight = 0;
			if (const auto ev = Events::GetById(id)) {
				for (const auto& trait : traits) {
					if (const auto pref = trait->Prefers(ev)) {
						if (pref > 0 || a_considerDislikes) {
							weight += a_weight * pref;
							if (!a_stack) {
								break;
							}
						}
					}
				}
			}

			weights.push_back(weight);
		}

		return weights;
	}
	inline std::vector<std::string> GetTraits(RE::StaticFunctionTag*, std::string a_context, RE::Actor* a_actor)
	{
		return Actors::GetTraitIds(a_context, a_actor);
	}

	inline std::string GetActorString(RE::StaticFunctionTag*, std::string a_context, RE::Actor* a_actor, std::string a_key, std::string a_default)
	{
		return Actors::GetValue<std::string>(a_context, a_actor, a_key, a_default);
	}
	inline bool GetActorBool(RE::StaticFunctionTag*, std::string a_context, RE::Actor* a_actor, std::string a_key, bool a_default)
	{
		return Actors::GetValue<bool>(a_context, a_actor, a_key, a_default);
	}
	inline void SetActorBool(RE::StaticFunctionTag*, std::string a_context, RE::Actor* a_actor, std::string a_key, bool a_val, bool a_persist)
	{
		Actors::SetValue<bool>(a_context, a_actor, a_key, a_val, a_persist);
	}

	inline bool RegisterFuncs(VM* a_vm)
	{	
		// contexts
		REGISTERFUNC(GetContextEvents)
		REGISTERFUNC(GetContextTags)
		REGISTERFUNC(GetPacks)
		
		// packs
		REGISTERFUNC(GetPackQuest)
		REGISTERFUNC(GetPackEvents)
		REGISTERFUNC(GetPackName)

		// events
		REGISTERFUNC(GetEventName)
		REGISTERFUNC(GetEventPack)
		REGISTERFUNC(GetEventDesc)
		REGISTERFUNC(GetEventTags)
		REGISTERFUNC(GetEventSeverity)
		REGISTERFUNC(GetEventStatus)
		REGISTERFUNC(SetEventStatus)
		REGISTERFUNC(IsExclusive)
		REGISTERFUNC(SetLock)
		REGISTERFUNC(FilterEventsByStatus)
		REGISTERFUNC(FilterEventsBySeverity)
		REGISTERFUNC(FilterEventsByTags)
		REGISTERFUNC(WeighEventsByActor)

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

		// actors
		REGISTERFUNC(GetActorString)
		REGISTERFUNC(GetActorBool)
		REGISTERFUNC(SetActorBool)

		return true;
	}
}