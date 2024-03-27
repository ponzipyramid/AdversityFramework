#pragma once

#include "Context.h"

namespace Adversity
{
	class Contexts
	{
	public:
		static void Init();
		static void Pause(std::string a_context);
		static void Reset(std::string a_context);
	private:
		static inline std::vector<Context> _contexts;
	};
}