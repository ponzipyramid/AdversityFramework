#include "Outfits.h"
#include "Util.h"
#include "Rules.h"
#include "Devices.h"

using namespace Adversity;

namespace Adversity::Helpers
{
	class IItemChangeVisitor
	{
	public:
		virtual ~IItemChangeVisitor() {}

		virtual RE::BSContainer::ForEachResult Visit(RE::InventoryEntryData*) { return RE::BSContainer::ForEachResult::kContinue; }; 
		virtual bool ShouldVisit([[maybe_unused]] RE::InventoryEntryData*, [[maybe_unused]] RE::TESBoundObject*) { return true; }
		virtual RE::BSContainer::ForEachResult Unk_03(RE::InventoryEntryData* a_entryData, [[maybe_unused]] void* a_arg2, bool* a_arg3)
		{
			*a_arg3 = true;
			return Visit(a_entryData);
		}

		RE::InventoryChanges::IItemChangeVisitor& AsNativeVisitor() { return *(RE::InventoryChanges::IItemChangeVisitor*)this; }
	};

	class WornVisitor : public IItemChangeVisitor
	{
	public:
		WornVisitor(std::function<RE::BSContainer::ForEachResult(RE::InventoryEntryData*)> a_fun) :
			_fun(a_fun){};

		virtual RE::BSContainer::ForEachResult Visit(RE::InventoryEntryData* a_entryData) override
		{
			return _fun(a_entryData);
		}

	private:
		std::function<RE::BSContainer::ForEachResult(RE::InventoryEntryData*)> _fun;
	};
}

Outfit* Outfits::GetOutfit(std::string a_context, std::string a_name)
{
	std::string id{ Util::Lower(std::format("{}/{}", a_context, a_name)) };
	return GetOutfit(id);
}

Outfit* Outfits::GetOutfit(std::string a_id)
{
	return _outfits.count(a_id) ? &_outfits[a_id] : nullptr;
}

Variant* Outfits::GetVariant(std::string a_id)
{
	return _variants.count(a_id) ? &_variants[a_id] : nullptr;
}

void Outfits::Load(std::string a_dir, std::string a_context)
{
	const std::string dir{ std::format("{}/outfits", a_dir) };

	if (!fs::is_directory(dir)) {
		logger::warn("{} has no outfits directory", a_context);
		return;
	}

	for (const auto& a : fs::directory_iterator(dir)) {
		if (fs::is_directory(a)) {
			continue;
		}

		if (!Util::IsYAML(a.path()))
			continue;

		const auto path{ a.path().string() };
		const auto filename{ a.path().filename().replace_extension().string() };

		try {
			auto outfitFile = YAML::LoadFile(path);
			auto outfit = outfitFile.as<Outfit>();
			const std::string outfitId{ std::format("{}/{}", a_context, outfit.name) };
			outfit.id = outfitId;

			_outfits.insert({ outfitId, outfit });

			for (auto i = 0; i < outfit.variants.size(); i++) {
				const std::string variantId{ std::format("{}/{}", outfit.id, i) };
				outfit.variants[i].id = variantId;
				_variants.insert({ variantId, outfit.variants[i] });
			}

			logger::info("loaded outfit {} successfully", filename);

		} catch (const std::exception& e) {
			logger::error("failed to load outfit {}: {}", filename, e.what());
		} catch (...) {
			logger::error("failed to load outfit {}", filename);
		}
	}
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

	const auto& active = Rules::GetActive();
	std::vector<RE::BGSKeyword*> excludeKwds{ Devices::GetLockableKwd() };
	excludeKwds.reserve(active.size() + 1);
	
	for (auto rule : active) {
		if (const auto kwd = rule->GetKwd())
			excludeKwds.push_back(kwd);
	}

	std::vector<Variant*> variants;
	for (const auto& id : a_ids) {
		bool valid = true;
		const auto variant = GetVariant(id);
		if (!variant) {
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

				if (wornPiece && wornPiece->HasKeywordInArray(excludeKwds, false)) {
					valid = false;
				} else if (!wornPiece && !piece.nothing) {
					valid = false;
				}
			}
		}

		if (valid)
			return true;
	}

	return false;
}