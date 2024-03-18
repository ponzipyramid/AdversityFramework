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
		template <typename T>
		static inline T* GetFormById(RE::FormID a_id)
		{
			return RE::TESDataHandler::GetSingleton()->LookupForm<T>(a_id, "Adversity Framework.esm");
		}
	private:
		static inline int _selected;
	};
}