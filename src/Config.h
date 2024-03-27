#pragma once

namespace Adversity
{
	struct ConfigData
	{
		struct Weights
		{
			struct Severity
			{
				std::vector<int> low{ 0, 0, 0, 0, 0 };
				std::vector<int> med{ 0, 0, 0, 0, 0 };
				std::vector<int> high{ 0, 0, 0, 0, 0 };
			};
			Severity severity;
			int tag;
		};
		Weights weights;
	};

	class Config
	{
	public:
		static inline ConfigData* Get()
		{
			static ConfigData data;
			static std::string path{ "Data/SKSE/AdversityFramework/config.yaml" };
			try {
				data = YAML::LoadFile(path).as<ConfigData>();	
			} catch (...) { }
			return &data;
		}
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<ConfigData>
	{
		static bool decode(const Node& node, ConfigData& rhs)
		{
			auto weights = node["weights"];
			auto severity = weights["severity"];

			rhs.weights.severity.low = severity["low"].as<std::vector<int>>();
			rhs.weights.severity.med = severity["med"].as<std::vector<int>>();
			rhs.weights.severity.high = severity["high"].as<std::vector<int>>();

			return true;
		}
	};
}