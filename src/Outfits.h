#pragma once

#include "Util.h"

namespace Adversity
{
	struct Piece
	{
		inline bool IsValid() const { return armo != nullptr; }
		RE::TESObjectARMO* armo = nullptr;
		bool optional;
		bool nothing;
	};

	struct Variant
	{
		inline bool IsValid() const {
			for (const auto& piece : pieces) {
				if (!piece.IsValid()) {
					return false;
				}
			}

			return true;
		}
		std::string id;
		std::vector<Piece> pieces;
		std::unordered_set<std::string> tags;
	};

	struct Mapping
	{
		inline bool Validate()
		{
			std::erase_if(out, [](Variant a_rule) {
				return a_rule.IsValid();
			});

			return !in && !out.empty();
		}
		RE::TESObjectARMO* in = nullptr;
		std::vector<Variant> out;
	};

	struct Outfit
	{
		inline bool Validate()
		{
			const auto outfitId{ id };
			int index = 0;
			std::erase_if(variants, [outfitId, &index](Variant a_var) {
				if (a_var.IsValid())
					return false;

				logger::info("{} variant at index {} is invalid", outfitId, index++);
				return true;
			});

			index = 0;
			std::erase_if(mappings, [outfitId, &index](Mapping a_map) {
				if (a_map.Validate())
					return false;

				logger::info("{} mapping at index {} is invalid", outfitId, index++);
				return true;
			});

			return !id.empty() && !name.empty() && (!variants.empty() || !mappings.empty());
		}
		std::string id;
		std::string name;
		std::vector<Variant> variants;
		std::vector<Mapping> mappings;
	};

	class Outfits
	{
	public:
		static void Load(std::string a_context, std::string a_pack);
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

					rhs.armo = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(formId, modName);
				}
			} else {
				rhs.armo = RE::TESForm::LookupByEditorID<RE::TESObjectARMO>(id);	
			}

			if (rhs.armo) {
				const auto kwds = node["addKeywords"].as<std::vector<std::string>>(std::vector<std::string>{});
				for (const auto& kwd : kwds) {
					Util::AddKwd(rhs.armo, kwd);
				}
			}

			rhs.optional = node["optional"].as<std::string>("false") == "true";
			rhs.nothing = node["nothing"].as<std::string>("false") == "true";

			return true;
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

			return true;
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

			return true;
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

			const auto kwds = node["addKeywords"].as<std::vector<std::string>>(std::vector<std::string>{});
			for (const auto& variant : rhs.variants) {
				for (const auto& piece : variant.pieces) {
					if (const auto armo = piece.armo) {
						for (const auto& kwd : kwds) {
							Util::AddKwd(armo, kwd);
						}
					}
				}
			}

			return true;
		}
	};
}