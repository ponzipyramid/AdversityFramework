#pragma once
#include "Pack.h"

namespace Adversity
{
	class Packs
	{
	public:
		static void Init();
		static void SwitchDialogueContexts(std::string a_context);
		static std::string GetRulePack(std::string a_rule);
		static std::string GetRuleName(std::string a_rule);
		static RE::TESQuest* GetPackQuest(std::string a_pack);
		static bool RuleHasContext(std::string a_rule, std::string a_context);
		static std::string GetRuleDesc(std::string a_rule);

	private:
		static inline std::unordered_map<std::string, std::vector<RE::TESGlobal*>> _contexts;

		static inline std::vector<Pack> _packs;
		static inline std::unordered_map<std::string, Pack*> _packsById;
		static inline std::unordered_map<std::string, Rule*> _rules;
	};
}