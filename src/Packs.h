#pragma once
#include "Pack.h"

namespace Adversity
{
	class Packs
	{
	public:
		static void Load(std::string a_path, std::string a_context);
		static Pack* GetById(std::string a_pack);
		static std::vector<Pack*> GetByContext(std::string a_context);
		static std::vector<std::string> GetIds(std::vector<Pack*> a_packs);
	private:
		static inline std::unordered_map<std::string, Pack> _packs;
		static inline std::unordered_map<std::string, std::vector<Pack*>> _contexts;
	};
}