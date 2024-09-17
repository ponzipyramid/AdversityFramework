#pragma once

#include "Actors.h"

namespace Adversity::Saves
{
	inline const auto RecordName = _byteswap_ulong('ADVY');

	inline void Save(SKSE::SerializationInterface*) { Actors::PersistAll(); }
}