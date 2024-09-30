#pragma once

#include "Context.h"

namespace Adversity
{
	class Contexts
	{
	public:
		static void Init();
		static void Persist(const std::string& a_id);
		static void PersistAll();

		template <typename T>
		static T GetEventValue(const std::string& a_id, std::string a_key, T a_default)
		{
			if (const auto data = GetEventField(a_id, a_key)) {
				if (std::holds_alternative<T>(*data)) {
					return std::get<T>(*data);
				} else {
					return a_default;
				}
			}

			return a_default;
		}
		template <typename T>
		static bool SetEventValue(std::string a_id, std::string a_key, T a_val)
		{
			if (const auto data = GetEventData(a_id)) {
				_dirty[a_id] = true;
				data->SetValue(a_key, GenericData{a_val});
				return true;
			}

			return false;
		}
	private:
		static Meta* GetEventData(const std::string& a_id);
		static GenericData* GetEventField(const std::string& a_id, const std::string& a_key);

		static inline std::unordered_map<std::string, Context> _contexts;
		static inline std::unordered_map<std::string, boolean> _dirty;
		static inline std::unordered_map<std::string, std::mutex> _locks;
	};
}