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
		static std::optional<T> GetEventValue(const std::string& a_id, const std::string& a_key, bool a_persist)
		{
			if (const auto data = GetEventField(a_id, a_key, a_persist)) {
				if (std::holds_alternative<T>(*data)) {
					return std::optional<T>{ std::get<T>(*data) };
				}
			}

			return std::nullopt;
		}
		template <typename T>
		static bool SetEventValue(std::string a_id, std::string a_key, T a_val, bool a_persist)
		{
			if (const auto data = GetEventData(a_id, a_persist)) {
				_dirty[a_id] = true;
				data->SetValue(a_key, GenericData{a_val});
				return true;
			}

			return false;
		}
	private:
		static Meta* GetEventData(const std::string& a_id, bool a_persist);
		static GenericData* GetEventField(const std::string& a_id, const std::string& a_key, bool a_persist);

		static inline std::unordered_map<std::string, Context> _runtime;

		static inline std::unordered_map<std::string, Context> _persistent;
		static inline std::unordered_map<std::string, boolean> _dirty;
		static inline std::unordered_map<std::string, std::mutex> _locks;
	};
}