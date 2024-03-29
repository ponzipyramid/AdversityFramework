#pragma once

#include "Context.h"

namespace Adversity
{
	class Contexts
	{
	public:
		static void Init();
	private:
		static inline std::vector<Context> _contexts;
	};
}