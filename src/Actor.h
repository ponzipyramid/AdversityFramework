#pragma once

#include "Util.h"
#include "Trait.h"
#include "Meta.h"

namespace Adversity
{
	class Actor : public Meta
	{
	public:
		inline std::string GetId() const { return _base->GetName(); }
		inline std::vector<std::string> GetTraits() { return _traits; }
	private:
		RE::TESNPC* _base;
		std::vector<std::string> _traits;

		friend struct YAML::convert<Actor>;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<Actor>
	{
		static bool decode(const Node& node, Actor& rhs)
		{
			const auto id = node["id"].as<std::string>("");
			rhs._base = RE::TESForm::LookupByEditorID<RE::TESNPC>(id);

			const auto traits = node["traits"].as<std::vector<std::string>>(std::vector<std::string>{});
			for (const auto& trait : traits) {
				rhs._traits.push_back(Util::Lower(trait));
			}

			rhs.Read(node["data"]);

			return true;
		}

		static Node encode(const Actor& rhs)
		{
			Node node;

			node["id"] = rhs._base->GetFormEditorID();
			node["traits"] = rhs._traits;
			node["data"] = rhs.Write();

			return node;
		}
	};
}