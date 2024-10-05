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

		static inline Context* GetContext(std::string a_context, bool a_persistent)
		{
			auto& map = (a_persistent ? _persistent : _runtime);
			const auto iter = map.find(Util::Lower(a_context));

			if (iter != map.end()) {
				return &iter->second;
			}

			return nullptr;
		}

		template <typename T>
		static T GetValue(const std::string& a_id, const std::string& a_key, T a_default, bool a_persist)
		{
			if (const auto& context = GetContext(a_id, a_persist)) {
				return context->GetValue<T>(a_key, a_default);
			}

			return a_default;
		}
		template <typename T>
		static bool SetValue(const std::string& a_id, const std::string& a_key, T a_val, bool a_persist)
		{
			if (const auto& context = GetContext(a_id, a_persist)) {
				context->SetValue<T>(a_key, a_val);
				return true;
			}

			return false;
		}
	private:
		static inline std::unordered_map<std::string, Context> _runtime;

		static inline std::unordered_map<std::string, Context> _persistent;
		static inline std::unordered_map<std::string, boolean> _dirty;
		static inline std::unordered_map<std::string, std::mutex> _locks;
	};
}