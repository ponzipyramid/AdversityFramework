#pragma once

#include "Util.h"

namespace Adversity
{
	class Context
	{
	public:
		inline std::string GetName() { return _name; }
		inline std::string GetId() { return _id; }
	private:
		std::string _name;
		std::string _id;

		friend struct YAML::convert<Context>;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<Context>
	{
		static bool decode(const Node& node, Context& rhs)
		{
			rhs._name = node["name"].as<std::string>();
			rhs._id = Util::Lower(rhs._name);
			return !rhs._name.empty();
		}
	};
}