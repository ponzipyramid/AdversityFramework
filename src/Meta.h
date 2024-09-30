#pragma once

#include "Trait.h"
#include "Util.h"

namespace Adversity
{
	using GenericData = std::variant<
		bool,
		int,
		float,
		std::string,
		RE::TESForm*,
		std::vector<bool>,
		std::vector<int>,
		std::vector<float>,
		std::vector<std::string>,
		std::vector<RE::TESForm*>>;

	class Meta
	{
	public:
		void ReadData(const YAML::Node& a_node)
		{
			for (YAML::const_iterator it = a_node.begin(); it != a_node.end(); ++it) {
				const auto key = Util::Lower(it->first.as<std::string>());

				if (it->second.IsSequence()) {
					const auto rawValues = it->second.as<std::vector<std::string>>(std::vector<std::string>{});

					GenericData values;
					if (!rawValues.empty()) {
						auto index = ConvertToGeneric(rawValues[0]).index();
						bool valid = true;

						std::vector<GenericData> convertedValues;
						convertedValues.reserve(rawValues.size());

						for (const auto& val : rawValues) {
							const auto converted = ConvertToGeneric(val);
							if (index != converted.index()) {
								valid = false;
							}
							convertedValues.push_back(converted);
						}

						if (!valid) {
							index = 3;
						}

						switch (index) {
						case 0:
							values = CreateList<bool>(convertedValues);
							break;
						case 1:
							values = CreateList<int>(convertedValues);
							break;
						case 2:
							values = CreateList<float>(convertedValues);
							break;
						case 3:
							values = CreateList<std::string>(convertedValues);
							break;
						case 4:
							values = CreateList<RE::TESForm*>(convertedValues);
							break;
						}
					}

					_data[key] = std::make_pair(rawValues, values);
				} else if (it->second.IsScalar()) {
					const auto rawValue = it->second.as<std::string>("");
					const auto value = ConvertToGeneric(rawValue);
					_data[key] = std::make_pair(std::vector<std::string>{ rawValue }, value);
				}
			}
		}

		YAML::Node WriteData() const
		{
			YAML::Node node;

			for (const auto& [key, value] : _data) {
				if (value.first.size() > 1) {
					node[key] = value.first;
				} else {
					node[key] = value.first.empty() ? "" : value.first[0];
				}
			}

			return node;
		}

		inline GenericData* GetValue(std::string a_key)
		{
			a_key = Util::Lower(a_key);
			if (_data.count(a_key)) {
				return &_data[a_key].second;
			}

			return nullptr;
		}
		inline bool SetValue(std::string a_key, GenericData a_value)
		{
			if (_data.count(a_key)) {
				std::vector<std::string> raw;
				switch (a_value.index()) {
				case 0:
					{
						raw.push_back(std::to_string(std::get<bool>(a_value)));
						break;
					}
				case 1:
					{
						raw.push_back(std::to_string(std::get<int>(a_value)));
						break;
					}
				case 2:
					{
						raw.push_back(std::to_string(std::get<float>(a_value)));
						break;
					}
				case 3:
					{
						raw.push_back(std::get<std::string>(a_value));
						break;
					}
				case 4:
					{
						const auto form = std::get<RE::TESForm*>(a_value);
						raw.push_back(form ? form->GetFormEditorID() : "");
						break;
					}
				case 5:
					{
						const auto list = std::get<std::vector<bool>>(a_value);
						for (const auto item : list) {
							raw.push_back(std::to_string(item));
						}
						break;
					}
				case 6:
					{
						const auto list = std::get<std::vector<int>>(a_value);
						for (const auto item : list) {
							raw.push_back(std::to_string(item));
						}
						break;
					}
				case 7:
					{
						const auto list = std::get<std::vector<float>>(a_value);
						for (const auto item : list) {
							raw.push_back(std::to_string(item));
						}
						break;
					}
				case 8:
					{
						const auto list = std::get<std::vector<std::string>>(a_value);
						for (const auto item : list) {
							raw.push_back(item);
						}
						break;
					}
				case 9:
					{
						const auto list = std::get<std::vector<RE::TESForm*>>(a_value);
						for (const auto item : list) {
							raw.push_back(item ? item->GetFormEditorID() : "");
						}
						break;
					}
				default:
					break;
				}

				_data[a_key] = std::make_pair(raw, a_value);
				return true;
			} else {
				return false;
			}
		}
	private:
		GenericData ConvertToGeneric(const std::string& a_str)
		{
			GenericData value = a_str;

			if (const auto form = Util::GetFormFromString(a_str)) {
				value = form;
			} else if (a_str == "true" || a_str == "false") {
				value = (bool)(a_str == "true");
			} else if (Util::IsNumeric(a_str)) {
				if (a_str.contains('.')) {
				} else {
					value = (float)std::stof(a_str);
				}
				value = a_str.contains('.') ? (float)std::stof(a_str) : (int)std::stoi(a_str);
			}

			return value;
		}

		template <typename T>
		std::vector<T> CreateList(std::vector<GenericData>& a_values)
		{
			std::vector<T> converted;
			converted.reserve(a_values.size());
			for (const auto& val : a_values) {
				converted.push_back(std::get<T>(val));
			}
			return converted;
		}

		std::unordered_map<std::string, std::pair<std::vector<std::string>, GenericData>> _data;
		friend struct YAML::convert<Meta>;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<Meta>
	{
		static bool decode(const Node& node, Meta& rhs)
		{
			rhs.ReadData(node);
			return true;
		}

		static Node encode(const Meta& rhs)
		{
			return rhs.WriteData();
		}
	};
}