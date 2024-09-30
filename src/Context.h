#pragma once

#include "Util.h"
#include "Meta.h"
#include "Serialization.h"

namespace Adversity
{
	using Metadata = std::unordered_map<std::string, Meta>;

	class Context
	{
	public:
		Context() = default;
		Context(SKSE::SerializationInterface* a_intfc)
		{
			_id = Serialization::Read<std::string>(a_intfc);
			auto i = Serialization::Read<std::size_t>(a_intfc);
			for (; i > 0; i--) {
				const auto id = Serialization::Read<std::string>(a_intfc);
				_events[id] = Meta{ a_intfc };
			}
		}

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

		inline void Serialize(SKSE::SerializationInterface* a_intfc) const
		{
			Serialization::Write(a_intfc, _id);
			Serialization::Write(a_intfc, _events.size());
			for (const auto& [id, data] : _events) {
				Serialization::Write(a_intfc, id);
				data.Serialize(a_intfc);
			}
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
			rhs._events = node["events"].as<Metadata>(Metadata{});
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