#include "Packs.h"

using namespace Adversity;

namespace
{
	constexpr std::string_view ext1 = ".yaml";
	constexpr std::string_view ext2 = ".yml";
	constexpr std::string_view dir = "Data/SKSE/AdversityFramework/Packs";
}

void Packs::Init()
{
	for (const auto& a : std::filesystem::directory_iterator(dir)) {
		if (std::filesystem::is_directory(a)) {
			continue;
		}

		const auto ext = a.path().extension();
		if (ext != ext1 && ext != ext2)
			continue;

		const auto path{ a.path().string() };
		const auto id{ a.path().filename().replace_extension().string() };

		try {
			logger::info("file: {}", path);

			auto packFile = YAML::LoadFile(path);
			auto pack = packFile.as<Pack>();
			pack.SetId(id);
			_packs.push_back(pack);
			_packsById[id] = &pack;

			for (auto& rule : pack.rules) {
				rule.SetPackId(id);
				_rules[rule.GetId()] = &rule;
			}

			logger::info("loaded pack {} successfully", id);

		} catch (const std::exception& e) {
			logger::error("failed to load pack {}: {}", path, e.what());
		} catch (...) {
			logger::error("failed to load pack {}", path);
		}
	}
}

void Packs::SwitchDialogueContexts(std::string a_context)
{
	for (const auto& [name, globals] : _contexts) {
		if (name == a_context) {
			for (const auto& global : globals) {
				global->value = 1.0f;
			}
		} else {
			for (const auto& global : globals) {
				global->value = 0.0f;
			}
		}
	}
}


std::string Packs::GetRulePack(std::string a_rule)
{
	return _rules.count(a_rule) ? _rules[a_rule]->GetPackId() : "";
}
std::string Packs::GetRuleName(std::string a_rule)
{
	return _rules.count(a_rule) ? _rules[a_rule]->GetName() : "";
}
std::string Packs::GetRuleDesc(std::string a_rule)
{
	return _rules.count(a_rule) ? _rules[a_rule]->GetDesc() : "";
}
bool Packs::RuleHasContext(std::string a_rule, std::string a_context)
{
	return _rules.count(a_rule) ? _rules[a_rule]->HasContext(a_context) : false;
}
RE::TESQuest* Packs::GetPackQuest(std::string a_pack)
{
	if (auto pack = _packsById[a_pack]) {
		return pack->GetQuest();
	}

	return nullptr;
}