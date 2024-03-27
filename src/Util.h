#pragma once
#include "UI.h"

namespace Adversity
{
	class Util
	{
	public:
		template <typename T>
		static inline T* GetFormById(RE::FormID a_id)
		{
			return RE::TESDataHandler::GetSingleton()->LookupForm<T>(a_id, "Adversity Framework.esm");
		}
		static inline int GetWeightedIndex(std::vector<int> a_weights)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::discrete_distribution<int> d{ a_weights.begin(), a_weights.end() };

			return d(gen);
		}
		static inline std::string Lower(std::string a_str)
		{
			std::string data{ a_str };
			std::transform(data.begin(), data.end(), data.begin(),
				[](unsigned char c) { return (char)std::tolower(c); });

			return data;
		}
	};
}