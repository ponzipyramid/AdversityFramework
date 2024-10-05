#pragma once

#include "Util.h"

namespace Adversity
{
	struct Sequence
	{
		std::string key;
	};

	struct Piece
	{
		inline bool IsValid() const { return armo != nullptr; }
		std::string id;
		std::string name;
		RE::TESObjectARMO* armo = nullptr;
		bool optional;
		bool nothing;
	};

	struct Variant
	{
		inline bool IsValid() const 
		{
			for (const auto& piece : pieces) {
				if (!piece.IsValid()) {
					logger::info("piece: {} {} is invalid", piece.name, piece.id);
					return false;
				}
			}

			return true;
		}
		inline bool HasTags(std::vector<std::string> a_tags, bool a_all) const
		{
			if (a_all) {
				for (const auto& tag : a_tags) {
					if (!tags.contains(tag))
						return false;
				}

				return true;
			} else {
				for (const auto& tag : a_tags) {
					if (tags.contains(tag))
						return true;
				}

				return false;
			}
		}
		std::string id;
		std::vector<Piece> pieces;
		std::vector<Sequence> sequence;
		int severity;
		std::unordered_set<std::string> tags;

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


			return !id.empty() && !name.empty() && !variants.empty();
		}
		std::string id;
		std::string name;
		std::vector<Variant> variants;
	};

	class Outfits
	{
	public:
		static void Load(std::string a_context, std::string a_pack);
		static void Load(std::string a_id, Outfit a_outfit);
		static void Reload(std::string a_context, std::string a_pack, std::string a_name);
		static Outfit* GetOutfit(std::string a_context, std::string a_name);
		static Outfit* GetOutfit(std::string a_id);
		static Variant* GetVariant(std::string a_id);
		static Variant* GetNextOutfit(std::string a_variant);
		static bool Validate(std::vector<std::string> a_ids);
		static bool AddVariant(std::string a_context, std::string a_pack, std::string a_name);
	private:
		static inline std::unordered_map<std::string, Outfit> _outfits;
		static inline std::unordered_map<std::string, Variant> _variants;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<Sequence>
	{
		static bool decode(const Node& node, Sequence& rhs)
		{
			rhs.key = node["key"].as<std::string>();
			return true;
		}
	};

	template <>
	struct convert<Piece>
	{
		static bool decode(const Node& node, Piece& rhs)
		{
			rhs.id = node["id"].as<std::string>();
			rhs.name = node["name"].as<std::string>("");
			rhs.armo = Util::GetFormFromString<RE::TESObjectARMO>(rhs.id);

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

			const auto kwds = node["addKeywords"].as<std::vector<std::string>>(std::vector<std::string>{});
			for (const auto& piece : rhs.pieces) {
				if (const auto& armo = piece.armo) {
					for (const auto& kwd : kwds) {
						Util::AddKwd(armo, kwd);
					}
				}
			}

			rhs.severity = node["severity"].as<int>(0);
			rhs.sequence = node["sequence"].as<std::vector<Sequence>>(std::vector<Sequence>{});

			auto tags = node["tags"].as<std::vector<std::string>>(std::vector<std::string>{});
			rhs.tags = std::unordered_set<std::string>{ tags.begin(), tags.end() };

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