#pragma once
#include "Packs.h"
#include "Util.h"

namespace Adversity::Papyrus
{
	constexpr std::string_view className = "AdversityFramework";

	void SwitchDialogueContexts(RE::StaticFunctionTag*, std::string a_context)
	{
		Packs::SwitchDialogueContexts(a_context);
	}

	void InternalMessageBox(RE::StaticFunctionTag*, std::string a_msg, std::vector<std::string> a_options)
	{
		Util::ShowMessageBox(a_msg, a_options);
	}

	int GetMessageBoxValue(RE::StaticFunctionTag*)
	{
		return Util::GetMessageBoxValue();
	}

	float GetWillpower(RE::StaticFunctionTag*)
	{
		return Util::GetWillpower();
	}

	bool IsWillpowerLow(RE::StaticFunctionTag*)
	{
		return Util::IsWillpowerLow();
	}

	bool IsWillpowerHigh(RE::StaticFunctionTag*)
	{
		return Util::IsWillpowerHigh();
	}


	inline bool RegisterFuncs(VM* a_vm)
	{
		REGISTERFUNC(SwitchDialogueContexts, className)
		REGISTERFUNC(InternalMessageBox, className)
		REGISTERFUNC(GetMessageBoxValue, className)
		REGISTERFUNC(GetWillpower, className)
		REGISTERFUNC(IsWillpowerLow, className)
		REGISTERFUNC(IsWillpowerHigh, className)

		return true;
	}
}