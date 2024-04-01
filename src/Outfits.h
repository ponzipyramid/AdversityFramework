#pragma once

namespace Adversity
{
	struct Piece
	{
		RE::TESObjectARMO* armo = nullptr;
		bool optional;
		bool nothing;
	};

	struct Variant
	{
		std::string id;
		std::vector<Piece> pieces;
		std::unordered_set<std::string> tags;
	};

	struct Outfit
	{
		std::string id;
		std::string name;
		std::vector<Variant> variants;
	};

	class Outfits
	{
	public:
		static void Load(std::string a_context);
		static Outfit* GetOutfit(std::string a_context, std::string a_name);
		static Outfit* GetOutfit(std::string a_id);
		static Variant* GetVariant(std::string a_id);
		static bool Validate(std::vector<std::string> a_ids);
	private:
		static inline std::unordered_map<std::string, Outfit> _outfits;
		static inline std::unordered_map<std::string, Variant> _variants;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<Piece>
	{
		static bool decode(const Node& node, Piece& rhs)
		{
			const auto id = node["id"].as<std::string>();
			rhs.armo = RE::TESForm::LookupByEditorID<RE::TESObjectARMO>(id);
			rhs.optional = node["optional"].as<std::string>("false") == "true";
			rhs.nothing = node["nothing"].as<std::string>("false") == "true";

			return rhs.armo != nullptr;
		}
	};

	template <>
	struct convert<Variant>
	{
		static bool decode(const Node& node, Variant& rhs)
		{
			rhs.pieces = node["pieces"].as<std::vector<Piece>>();
			const auto tags = node["tags"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs.tags = std::unordered_set<std::string>{ tags.begin(), tags.end() };

			return !rhs.pieces.empty();
		}
	};

	template <>
	struct convert<Outfit>
	{
		static bool decode(const Node& node, Outfit& rhs)
		{
			rhs.name = node["name"].as<std::string>();
			rhs.variants = node["variants"].as<std::vector<Variant>>();

			return !rhs.name.empty() && !rhs.variants.empty();
		}
	};
}