#include "Outfits.h"
#include "Util.h"
#include "Events.h"
#include "Devices.h"

using namespace Adversity;

void Outfits::Load(std::string a_context, std::string a_pack)
{
	Util::ProcessEntities<Outfit>(a_context, a_pack, "outfits", [](std::string a_id, Outfit a_outfit) {
		Load(a_id, a_outfit);
	});
}

void Outfits::Load(std::string a_id, Outfit a_outfit)
{
	a_outfit.id = a_id;

	if (a_outfit.Validate()) {
		_outfits.insert({ a_id, a_outfit });

		auto& outfit{ _outfits[a_id] };

		for (auto i = 0; i < outfit.variants.size(); i++) {
			const std::string variantId{ std::format("{}/{}", a_outfit.id, i) };
			outfit.variants[i].id = variantId;
			_variants.insert({ variantId, outfit.variants[i] });
		}
	} else {
		throw std::exception{ "failed to validate" };
	}
}

void Outfits::Reload(std::string a_context, std::string a_pack, std::string a_name) {
	const std::string file{ std::format("data/skse/adversityframework/contexts/{}/packs/{}/Outfits/{}.yaml", a_context, a_pack, a_name) };
	
	const std::string id{ std::format("{}/{}", a_context, Util::Lower(a_name)) };
	auto config = YAML::LoadFile(file);
	Load(id, config.as<Outfit>());
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

Variant* Outfits::GetNextOutfit(std::string a_variant)
{
	a_variant = Util::Lower(a_variant);

	logger::info("GetNextOutfit - {}", a_variant);

	if (const auto variant = GetVariant(a_variant)) {

		if (variant->sequence.empty())
			return nullptr;

		const auto severity = variant->severity + 1;

		std::unordered_set<std::string> sequences;

		for (const auto& seq : variant->sequence) {
			sequences.insert(seq.key);
		}

		logger::info("GetNextOutfit - {} - {}", severity, sequences.size());

		std::vector<Variant*> candidates;

		auto splits = Util::Split(a_variant, "/");
		splits.pop_back();
		if (const auto outfit = GetOutfit(Util::Join(splits, "/"))) {
			for (auto& var : outfit->variants) {
				if (var.severity == severity) {
					for (const auto& seq : var.sequence) {
						if (sequences.contains(seq.key)) {
							candidates.push_back(&var);
						}
					}
				}
			}
		}

		logger::info("GetNextOutfit - {}", candidates.size());

		if (candidates.empty())
			return nullptr;

		return candidates[Util::Random(0, candidates.size() - 1)];
	}

	return nullptr;
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

	const auto& active = Events::Filter([](Event* a_rule) { return a_rule->GetStatus() == Event::Status::Active; });
	std::vector<RE::BGSKeyword*> allowedKwds{ Devices::GetLockableKwd() };
	allowedKwds.reserve(active.size() + 1);
	
	for (auto rule : active) {
		const auto kwds = rule->GetKwds();
		for (const auto kwd : kwds) {
			allowedKwds.push_back(kwd);
		}
	}

	logger::info("validating - START");
	for (const auto id : a_ids) {
		logger::info("validating - {}", id);
	}

	bool valid = true;
	std::vector<Variant*> variants;
	for (const auto& id : a_ids) {
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
					logger::info("{} is not valid due to {} instead of {}", id, wornPiece ? wornPiece->GetName() : "none", piece.armo->GetName());
					valid = false;
					break;
				}
			}
		}
	}
	logger::info("Variant Valid = {}", valid);
	return valid;
}

bool Outfits::AddVariant(std::string a_context, std::string a_pack, std::string a_name)
{
	logger::info("AddVariant - {}/{}/{}", a_context, a_pack, a_name);

	if (auto outfit = GetOutfit(a_context, a_name)) {
		const auto& inventory = RE::PlayerCharacter::GetSingleton()->GetInventory();

		std::vector<RE::TESObjectARMO*> worn;
		for (const auto& [item, meta] : inventory) {
			const auto& [count, data] = meta;

			if (data->IsWorn()) {
				if (const auto& armo = item->As<RE::TESObjectARMO>()) {
					logger::info("found armo: {} {} {}", armo->GetFormEditorID(), armo->GetName(), Util::GetFormIDString(armo));

					if (armo->HasKeywordString("SOS_Genitals") || armo->HasKeywordString("SOS_PubicHair")) {
						continue;
					}

					worn.emplace_back(armo);
				}
			}
		}
	
		logger::info("Worn: {}", worn.size());
		
		if (worn.empty()) {
			return false;
		}

		YAML::Node variantNode;

		variantNode["severity"] = 1;
		for (const auto& armo : worn) {
			YAML::Node piece;
			piece["name"] = armo->GetName();
			piece["id"] = std::format("{}|{}", Util::GetFormIDString(armo), armo->GetFile(0)->GetFilename());
			variantNode["pieces"].push_back(piece);
		}

		const std::string file{ std::format("data/skse/adversityframework/contexts/{}/packs/{}/Outfits/{}.yaml", a_context, a_pack, a_name) };
		auto config = YAML::LoadFile(file);

		config["variants"].push_back(variantNode);

		std::ofstream fout(file);
		fout << config;

		return true;
	} else {
		logger::error("AddVariant - failed to find outfit");
	}

	return false;
}