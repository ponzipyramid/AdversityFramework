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
			_config = Meta{ a_intfc };
		}

		inline std::string GetId() { return _id; }

		inline void Init(const std::string& a_id) 
		{
			_id = a_id;
		}

		inline Meta* GetConfig() { return &_config; }

		inline Meta* GetEventData(const std::string& a_pack, const std::string& a_name, bool a_create = false)
		{
			const auto id{ a_pack + "/" + a_name };

			if (a_create) {
				logger::info("creating");
				_events[id];
			}

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
			_config.Serialize(a_intfc);
		}

	private:
		std::string _id;

		Metadata _events;
		Meta _config;

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
			rhs._config = node["config"].as<Meta>(Meta{});

			return true;
		}

		static Node encode(const Context& rhs)
		{
			Node node;

			node["events"] = rhs._events;
			node["config"] = rhs._config;
		
			return node;
		}
	};
}