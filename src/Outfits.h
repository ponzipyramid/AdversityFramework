#pragma once

#include "Util.h"

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

	struct Mapping
	{
		RE::TESObjectARMO* in = nullptr;
		std::vector<Variant> out;
	};

	struct Outfit
	{
		std::string id;
		std::string name;
		std::vector<Variant> variants;
		std::vector<Mapping> mappings;
	};

	class Outfits
	{
	public:
		static void Load(std::string a_context);
		static Outfit* GetOutfit(std::string a_context, std::string a_name);
		static Outfit* GetOutfit(std::string a_id);
		static Variant* GetVariant(std::string a_id);
		static Mapping* GetMapping(RE::TESObjectARMO* a_in);
		static bool Validate(std::vector<std::string> a_ids);
	private:
		static inline std::unordered_map<std::string, Outfit> _outfits;
		static inline std::unordered_map<std::string, Variant> _variants;
		static inline std::unordered_map<RE::TESObjectARMO*, Mapping> _mappings;
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

			if (id.starts_with("0x")) {
				const auto splits{ Util::Split(id, "|") };

				if (splits.size() == 2) {
					const auto formId{ std::stol(splits[0], NULL, 0) };
					const auto modName{ splits[1] };

					logger::info("Piece FormId: {} {}", splits[0], splits[1]);
					logger::info("Piece FormId: {} {}", formId, modName);

					rhs.armo = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(formId, modName);
				}
			} else {
				rhs.armo = RE::TESForm::LookupByEditorID<RE::TESObjectARMO>(id);	
			}

			rhs.optional = node["optional"].as<std::string>("false") == "true";
			rhs.nothing = node["nothing"].as<std::string>("false") == "true";

			if (!rhs.armo)
				logger::info("piece {} could not be found", id);

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
	struct convert<Mapping>
	{
		static bool decode(const Node& node, Mapping& rhs)
		{
			const auto in = node["name"].as<std::string>();
			rhs.in = RE::TESForm::LookupByEditorID<RE::TESObjectARMO>(in);
			rhs.out = node["out"].as<std::vector<Variant>>();

			return !rhs.in || rhs.out.empty();
		}
	};

	template <>
	struct convert<Outfit>
	{
		static bool decode(const Node& node, Outfit& rhs)
		{
			rhs.name = node["name"].as<std::string>();
			rhs.variants = node["variants"].as<std::vector<Variant>>(std::vector<Variant>{});
			rhs.mappings = node["mappings"].as<std::vector<Mapping>>(std::vector<Mapping>{});

			return !rhs.name.empty() && (!rhs.variants.empty() || !rhs.mappings.empty());
		}
	};
}