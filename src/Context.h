#pragma once

#include "Util.h"
#include "Meta.h"

namespace Adversity
{
	using Metadata = std::unordered_map<std::string, Meta>;

	class Context
	{
	public:
		inline std::string GetId() { return _id; }

		inline void Init(const std::string& a_id) 
		{
			_id = a_id;
		}

		inline Meta* GetEventData(const std::string& a_pack, const std::string& a_name)
		{
			const auto id{ a_pack + "/" + a_name };
			return _events.count(id) ? &_events[id] : nullptr;
		}
	private:
		std::string _id;

		Metadata _events;

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
			rhs._events = node["events"].as<Metadata>();
			return true;
		}

		static Node encode(const Context& rhs)
		{
			Node node;

			node["events"] = rhs._events;
		
			return node;
		}
	};
}