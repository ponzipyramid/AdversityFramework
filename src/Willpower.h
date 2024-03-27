#pragma once
#include "Util.h"

namespace Adversity
{
	class Willpower
	{
	public:
		static inline void Init()
		{
			_willpower = Util::GetFormById<RE::TESGlobal>(0x808);
			_willpowerHi = Util::GetFormById<RE::TESGlobal>(0x80D);
			_willpowerLo = Util::GetFormById<RE::TESGlobal>(0x80E);

			_resist = Util::GetFormById<RE::TESGlobal>(0x810);
			_resistMax = Util::GetFormById<RE::TESGlobal>(0x812);
		}

		static inline float GetLowWillpower()
		{
			return _willpowerLo->value;
		}

		static inline float GetHighWillpower()
		{
			return _willpowerHi->value;
		}

		static inline bool IsWillpowerLow()
		{
			return GetWillpower() <= GetLowWillpower();
		}

		static inline bool IsWillpowerHigh()
		{
			return GetWillpower() >= GetHighWillpower();
		}

		static inline float GetWillpower()
		{
			return _willpower->value;
		}

		static inline float GetResistance()
		{
			return _resist->value;
		}

		static inline void ModResistance(float a_delta)
		{
			auto val = _resist->value;
			val += a_delta;
			if (a_delta > 0.f) {
				val = val > _resistMax->value ? val : _resistMax->value;
			} else {
				if (val < 0.f) {
					_willpower->value -= 1;

					if (_willpower->value <= 0.f) {
						_willpower->value = 0.f;
						_resist->value = 0.f;
					} else {
						_resist->value = val + GetRecoveryResist();
					}
				}
			}
		}

		static inline float GetRecoveryResist()
		{
			// TODO: factor in devices worn, etc.
			return _resistMax->value;
		}
	private:

		static inline RE::TESGlobal* _resist;
		static inline RE::TESGlobal* _resistMax;
		static inline RE::TESGlobal* _willpower;
		static inline RE::TESGlobal* _willpowerLo;
		static inline RE::TESGlobal* _willpowerHi;
	};
}