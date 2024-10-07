#pragma once

#include "Util.h"
#include "Trait.h"
#include "Meta.h"

namespace Adversity
{
	struct AttachedTrait
	{
		std::string id;
		int8_t rank;
	};

	class Actor : public Meta
	{
	public:
		Actor() = default;
		Actor(RE::Actor* a_base) :
			_base(a_base) {}
		inline RE::FormID GetId() const { return _base ? _base->GetFormID() : 0; }
		inline std::vector<AttachedTrait> GetTraits() { return _traits; }
	private:
		RE::Actor* _base;
		std::vector<AttachedTrait> _traits;

		friend struct YAML::convert<Actor>;
	};
}

namespace YAML
{
	using namespace Adversity;

	template<>
	struct convert<AttachedTrait>
	{
		static bool decode(const Node& node, AttachedTrait& rhs)
		{
			rhs.id = Util::Lower(node["id"].as<std::string>());
			rhs.rank = node["rank"].as<int8_t>(0);

			return true;
		}

		static Node encode(const AttachedTrait& rhs)
		{
			Node node;

			node["id"] = rhs.id;
			node["rank"] = rhs.rank;

			return node;
		}
	};

	template <>
	struct convert<Actor>
	{
		static bool decode(const Node& node, Actor& rhs)
		{
			const auto id = node["id"].as<std::string>("");
			rhs._base = Util::GetFormFromString<RE::Actor>(id);

			logger::info("Actor Data: {} = {}", id, rhs._base != nullptr);

			rhs._traits = node["traits"].as<std::vector<AttachedTrait>>(std::vector<AttachedTrait>{});
			
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