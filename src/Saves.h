#pragma once

#include "Actors.h"
#include "Contexts.h"

namespace Adversity::Saves
{
	inline const auto RecordName = _byteswap_ulong('ADVY');

	inline void Save(SKSE::SerializationInterface* a_intfc) { 
		Actors::PersistAll();
		Contexts::PersistAll();

		if (!a_intfc->OpenRecord(RecordName, 0)) {
			logger::error("Unable to open record to write cosave data.");
			return;
		}

		Contexts::Save(a_intfc); 
	}

	inline void Load(SKSE::SerializationInterface* a_intfc)
	{

		uint32_t type;
		uint32_t version;
		uint32_t length;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (type == RecordName) {
				if (length > 0)
					Contexts::Load(a_intfc);
				break;
			}
		}
	}

	inline void Revert(SKSE::SerializationInterface*)
	{
		Contexts::Revert();
	}
}