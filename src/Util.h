#pragma once
#include "UI.h"

namespace Adversity
{
	class Util
	{
	public:
		static inline void ShowMessageBox(std::string a_msg, std::vector<std::string> a_options)
		{
			_selected = -1;
			UI::MessageBox::Show(a_msg, a_options, [](unsigned int a_index) {
				_selected = a_index;
			});
		}

		static inline int GetMessageBoxValue()
		{
			return _selected;
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
			return GetForm<RE::TESGlobal>(0x808)->value;
		}
	private:
		template<typename T>
		static inline T* GetForm(RE::FormID a_id)
		{
			return RE::TESDataHandler::GetSingleton()->LookupForm<T>(a_id, "Adversity Framework.esm");
		}
		static inline float GetLowWillpower()
		{
			return GetForm<RE::TESGlobal>(0x808)->value;
		}

		static inline float GetHighWillpower()
		{
			return GetForm<RE::TESGlobal>(0x808)->value;
		}

		static inline int _selected;
	};
}