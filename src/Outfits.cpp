#include "Outfits.h"
#include "Util.h"
#include "Rules.h"
#include "Devices.h"

using namespace Adversity;

void Outfits::Load(std::string a_context, std::string a_pack)
{
	Util::ProcessEntities<Outfit>(a_context, a_pack , "outfits", [&a_context](std::string a_id, Outfit a_outfit) {
		a_outfit.id = a_id;

		if (a_outfit.Validate()) {
			_outfits.insert({ a_id, a_outfit });

			auto& outfit{ _outfits[a_id] };

			for (auto i = 0; i < outfit.variants.size(); i++) {
				const std::string variantId{ std::format("{}/{}", a_outfit.id, i) };
				outfit.variants[i].id = variantId;
				_variants.insert({ variantId, outfit.variants[i] });
			}

			for (auto& mapping : a_outfit.mappings) {
				_mappings[mapping.in] = mapping;
			}
		} else {
			throw std::exception{ "failed to validate" };
		}
	});
}

Outfit* Outfits::GetOutfit(std::string a_context, std::string a_name)
{
	std::string id{ std::format("{}/{}", a_context, a_name) };
	return GetOutfit(id);
}

Outfit* Outfits::GetOutfit(std::string a_id)
{
	a_id = Util::Lower(a_id);
	return _outfits.count(a_id) ? &_outfits[a_id] : nullptr;
}

Variant* Outfits::GetVariant(std::string a_id)
{
	a_id = Util::Lower(a_id);
	return _variants.count(a_id) ? &_variants[a_id] : nullptr;
}

Mapping* Outfits::GetMapping(RE::TESObjectARMO* a_in)
{
	return _mappings.count(a_in) ? &_mappings[a_in] : nullptr;
}

bool Outfits::Validate(std::vector<std::string> a_ids)
{
	if (a_ids.empty()) return true;

	std::unordered_set<RE::TESObjectARMO*> worn;
	std::vector<RE::TESObjectARMO*> wornList;
	const auto inv = RE::PlayerCharacter::GetSingleton()->GetInventory([](RE::TESBoundObject& a_object) {
		return a_object.IsArmor();
	}, false);

	for (const auto& [item, invData] : inv) {
		const auto& [count, entry] = invData;
		if (count > 0 && entry->IsWorn()) {
			if (const auto armor = item->As<RE::TESObjectARMO>()) {
				worn.insert(armor);
				wornList.push_back(armor);
			}
		}
	}

	const auto& active = Rules::Filter([](Rule* a_rule) { return a_rule->GetStatus() == PackItem::Status::Active; });
	std::vector<RE::BGSKeyword*> allowedKwds{ Devices::GetLockableKwd() };
	allowedKwds.reserve(active.size() + 1);
	
	for (auto rule : active) {
		if (const auto kwd = rule->GetKwd())
			allowedKwds.push_back(kwd);
	}

	logger::info("validating - START");
	for (const auto id : a_ids) {
		logger::info("validating - {}", id);
	}

	std::vector<Variant*> variants;
	for (const auto& id : a_ids) {
		bool valid = true;
		const auto variant = GetVariant(id);
		if (!variant) {
			logger::info("{} variant could not be found", id);
			continue;
		}

		for (const auto& piece : variant->pieces) {
			if (!piece.armo)
				continue;

			if (piece.optional)
				continue;

			if (!worn.contains(piece.armo)) {
				
				RE::TESObjectARMO* wornPiece = nullptr;
				for (const auto armo : wornList) {
					if (armo->HasPartOf(piece.armo->GetSlotMask())) {
						wornPiece = armo;
						break;
					}
				}

				if ((wornPiece && !wornPiece->HasKeywordInArray(allowedKwds, false)) || (!wornPiece && !piece.nothing)) {
					logger::info("{} is not a valid due", id);
					valid = false;
					break;
				}
			}
		}

		if (valid)
			return true;
	}

	return false;
}