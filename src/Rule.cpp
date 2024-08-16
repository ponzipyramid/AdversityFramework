#include "Rule.h"

using namespace Adversity;

bool Conflict::With(Conflict a_other)
{
	if (type == Type::Filth && a_other.type == Type::Clean) {
		return true;
	}

	if (type == Type::Wear && a_other.type == Type::Wear) {
		for (const auto slot : slots) {
			if (a_other.slots.contains(slot))
				return true;
		}
	}

	if (type == Type::Outfit && a_other.type == Type::Naked) {
		return true;
	}

	if (type == Type::HeavyBondage && a_other.type == Type::HeavyBondage) {
		logger::info("Heavy bondage conflict");
		return true;
	}

	if (type == Type::Outfit && a_other.type == Type::Outfit) {
		logger::info("outfit conflict = {} {}", exclusive, a_other.exclusive);
		return true;
	}

	return false;
}