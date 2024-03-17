#pragma once
#include "Pack.h"

namespace Adversity
{
	class Packs
	{
	public:
		static void Init();
		static void SwitchDialogueContexts(std::string a_context);
	private:
		static inline std::unordered_map<std::string, std::vector<RE::TESGlobal*>> _contexts;
		static inline std::vector<Pack> _packs;
		static inline std::unordered_map<std::string, Pack*> _packsByPath;
	};
}