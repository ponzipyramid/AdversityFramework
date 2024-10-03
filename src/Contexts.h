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
		static void Save(SKSE::SerializationInterface* a_intfc);
		static void Load(SKSE::SerializationInterface* a_intfc);
		static void Revert();

		template <typename T>
		static T GetValue(const std::string& a_id, const std::string& a_key, T a_default, bool a_persist)
		{
			if (const auto data = GetData(a_id, a_persist)) {
				if (const auto field = data->GetValue(a_key)) {
					if (std::holds_alternative<T>(*field)) {
						return std::get<T>(*field);
					}
				}
			}

			return a_default;
		}
		template <typename T>
		static bool SetValue(std::string a_id, std::string a_key, T a_val, bool a_persist)
		{
			if (const auto data = GetData(a_id, a_persist)) {
				data->SetValue(a_key, GenericData{ a_val });
				return true;
			}

			return false;
		}

		template <typename T>
		static std::optional<T> GetEventValue(const std::string& a_id, const std::string& a_key, bool a_persist)
		{
			if (const auto field = GetEventField(a_id, a_key, a_persist)) {				
				if (std::holds_alternative<T>(*field)) {
					return std::optional<T>{ std::get<T>(*field) };
				}
			}

			return std::nullopt;
		}
		template <typename T>
		static bool SetEventValue(std::string a_id, std::string a_key, T a_val, bool a_persist)
		{
			if (const auto data = GetEventData(a_id, a_persist, true)) {
				logger::info("found data: {} - {}", a_id, a_persist);

				const auto& splits = Util::Split(a_id, "/");

				if (splits.empty())
					return false;

				logger::info("setting value {}", a_key);

				_dirty[Util::Lower(splits[0])] = true;
				data->SetValue(a_key, GenericData{a_val});
				return true;
			}

			logger::info("did not find data");

			return false;
		}
	private:
		static Meta* GetData(const std::string& a_id, bool a_persist);
		
		static Meta* GetEventData(const std::string& a_id, bool a_persist, bool a_create = false);
		static GenericData* GetEventField(const std::string& a_id, const std::string& a_key, bool a_persist);

		static inline std::unordered_map<std::string, Context> _runtime;

		static inline std::unordered_map<std::string, Context> _persistent;
		static inline std::unordered_map<std::string, boolean> _dirty;
		static inline std::unordered_map<std::string, std::mutex> _locks;
	};
}