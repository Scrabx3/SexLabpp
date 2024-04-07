#include "Expression.h"

namespace Registry
{
	Expression::Profile::Profile(const YAML::Node& a_src) :
		id(a_src["id"].IsDefined() ? a_src["id"].as<std::string>() : "Missing Name"),
		tags(a_src["tags"].IsDefined() ? a_src["tags"].as<std::vector<std::string>>() : std::vector<std::string>{}),
		enabled(!a_src["enabled"].IsDefined() || a_src["enabled"].as<bool>())
	{
		if (!a_src["data"].IsDefined())
			throw std::exception("Missing data values");

		const auto fill = [&](RE::SEXES::SEX sex) {
			auto& fields = a_src["data"][static_cast<uint32_t>(sex)];
			for (auto&& arr : fields) {
				const auto values = arr.as<std::vector<float>>();
				if (values.size() != Profile::Total)
					throw std::exception(fmt::format("Invalid value field, expected {}/32 values found", values.size()).c_str());
				auto& it = data[sex].emplace_back();
				std::copy_n(values.begin(), it.size(), it.begin());
			}
		};
		fill(RE::SEXES::kMale);
		fill(RE::SEXES::kFemale);
	}

	Expression::Profile::Profile(const nlohmann::json& a_src) :
		id([&]() {
			if (const auto strings = a_src.find("string"); strings != a_src.end())
				if (const auto name = strings->find("name"); name != strings->end())
					return name->get<std::string>();
			throw std::exception("Missing Name field");
		}())
	{
		const auto floats = a_src.find("floatList");
		if (floats == a_src.end())
			throw std::exception("Missing 'floatList' field");
		auto fill = [&](RE::SEXES::SEX sex, std::string begin) mutable {
			constexpr auto MAX_VALUE_FIELDS = 5;
			for (size_t i = 0; i < MAX_VALUE_FIELDS; i++) {
				auto fieldname = begin + std::to_string(i);
				auto field = floats->find(fieldname);
				if (field == floats->end() || !field->is_array())
					break;
				auto values = field->get<std::vector<float>>();
				if (values.size() != Profile::Total)
					throw std::exception(fmt::format("Invalid value field, expected {}/32 values found in field {}", values.size(), fieldname).c_str());
				auto& it = data[sex].emplace_back();
				std::copy_n(values.begin(), it.size(), it.begin());
			}
		};
		fill(RE::SEXES::kMale, "male");
		fill(RE::SEXES::kFemale, "female");
		if (const auto ints = a_src.find("int"); ints != a_src.end()) {
			const auto get = [&](const char* fieldname) -> bool {
				auto field = ints->find(fieldname);
				return field != ints->end() && field->get<int>() == 1;
			};
			enabled = get("enabled");
			for (auto&& tag : { "aggressor", "normal", "victim " })
				if (get(tag))
					tags.AddTag(tag);
		}
	}

	YAML::Node Expression::Profile::AsYAML() const
	{
		YAML::Node ret;
		ret["id"] = id.data();
		for (auto&& tag : tags.AsVector()) {
			ret["tags"].push_back(tag.data());
		}
		for (size_t i = 0; i < RE::SEXES::kTotal; i++) {
			for (size_t n = 0; n < data[i].size(); n++) {
				ret["data"][i].push_back(data[i][n]);
			}
		}
		ret["enabled"] = enabled;
		return ret;
	}

	Expression::Profile Expression::GetDefaultAfraid()
	{
		Profile ret{};
		ret.id = "Afraid";
		ret.data[RE::SEXES::kMale] = {
			{ 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0.1f, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0.3f, 0.3f, 3, 1 },
			{ 0, 0, 1, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 1 },
			{ 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0, 0.4f, 3, 1 },
			{ 0.3f, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0.9f, 0, 0, 9, 1 }
		};
		ret.data[RE::SEXES::kFemale] = {
			{ 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0.1f, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0.3f, 0.3f, 3, 1 },
			{ 0, 0, 1, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 1 },
			{ 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0, 0.4f, 3, 1 },
			{ 0.3f, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0.9f, 0, 0, 9, 1 },
		};
		ret.tags = { std::vector{ "Aggressor", "Afraid", "Scared", "Pain", "Negative" } };
		ret.enabled = true;
		ret.isdefault = true;
		return ret;
	}

	Expression::Profile Expression::GetDefaultAngry()
	{
		Profile ret{};
		ret.id = "Angry";
		ret.data[RE::SEXES::kMale] = {
			{ 0, 0, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.3f, 0, 0.4f },
			{ 0, 0, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 0, 0.55f },
			{ 0, 0, 0, 0.4f, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.65f, 0.65f, 0, 1 }
		};
		ret.data[RE::SEXES::kFemale] = {
			{ 0, 0, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.3f, 0, 0.4f },
			{ 0, 0, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 0, 0.55f },
			{ 0, 0, 0, 0.4f, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.65f, 0.65f, 0, 1 },
		};
		ret.tags = { std::vector{ "Aggressor", "Victim", "Mad", "Angry", "Upset" } };
		ret.enabled = true;
		ret.isdefault = true;
		return ret;
	}

	Expression::Profile Expression::GetDefaultHappy()
	{
		Profile ret{};
		ret.id = "Happy";
		ret.data[RE::SEXES::kMale] = {
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 2.0f, 0.5f },
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 2.0f, 0.7f },
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0.7f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 2.0f, 0.8f }
		};
		ret.data[RE::SEXES::kFemale] = {
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 2.0f, 0.5f },
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 2.0f, 0.7f },
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0.7f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 2.0f, 0.8f }
		};
		ret.tags = { std::vector{ "Normal", "Happy", "Consensual" } };
		ret.enabled = true;
		ret.isdefault = true;
		return ret;
	}

	Expression::Profile Expression::GetDefaultJoy()
	{
		Profile ret{};
		ret.id = "Joy";
		ret.data[RE::SEXES::kMale] = {
			{ 0, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0.2f, 0, 0, 0, 0, 0, 0, 13.0f, 0.4f },
			{ 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.4f, 0.4f, 8.0f, 0.4f },
			{ 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 0, 0, 0, 0, 0.3f, 0.3f, 13.0f, 0.8f }
		};
		ret.data[RE::SEXES::kFemale] = {
			{ 0.3f, 0, 0, 0, 0, 0, 0, 0.6f, 0, 0, 0, 0, 0.6f, 0, 0, 0, 0.3f, 0.3f, 0, 0, 1.0f, 1.0f, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 10, 0.45f },
			{ 0, 0, 0, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.3f, 0.3f, 0, 0, 1.0f, 1.0f, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 10, 0.6f },
			{ 0, 0, 0, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.3f, 0.3f, 0, 0, 1.0f, 1.0f, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 10, 0.6f },
			{ 0.1f, 0, 0, 0, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.3f, 0.7f, 0.7f, 1.0f, 1.0f, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 10, 0.5f },
			{ 0.6f, 0, 0, 0, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.3f, 0.7f, 0.7f, 1.0f, 1.0f, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 10, 0.6f },
		};
		ret.tags = { std::vector{ "Normal", "Happy", "Joy", "Pleasure", "Consensual" } };
		ret.enabled = true;
		ret.isdefault = true;
		return ret;
	}

	Expression::Profile Expression::GetDefaultPained()
	{
		Profile ret{};
		ret.id = "Pained";
		ret.data[RE::SEXES::kMale] = {
			{ 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0.1f, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0.3f, 0.3f, 3, 1 },
			{ 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0, 0.4f, 3, 1 },
			{ 0, 0, 1, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 1 },
			{ 0.3f, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0.9f, 0, 0, 9, 1 }
		};
		ret.data[RE::SEXES::kFemale] = {
			{ 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0.1f, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0.3f, 0.3f, 3, 1 },
			{ 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0, 0.4f, 3, 1 },
			{ 0, 0, 1, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 1 },
			{ 0.3f, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0.9f, 0, 0, 9, 1 }
		};
		ret.tags = { std::vector{ "Victim", "Afraid", "Pain", "Pained", "Negative" } };
		ret.enabled = true;
		ret.isdefault = true;
		return ret;
	}

	Expression::Profile Expression::GetDefaultPleasure()
	{
		Profile ret{};
		ret.id = "Pleasure";
		ret.data[RE::SEXES::kMale] = {
			{ 0, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0.2f, 0, 0, 0, 0, 0, 0, 13.0f, 0.4f },
			{ 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.4f, 0.4f, 8.0f, 0.4f },
			{ 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 0, 0, 0, 0, 0.3f, 0.3f, 13.0f, 0.8f }
		};
		ret.data[RE::SEXES::kFemale] = {
			{ 0, 0, 0, 0, 0, 0.3f, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.0f, 0.3f },
			{ 0.2f, 0, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.3f, 0.2f, 0.2f, 0, 0, 0, 0, 0, 0, 10, 0.5f },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0, 0, 0, 0, 0.5f, 0.5f, 0.3f, 0.3f, 0.7f, 0.4f, 0, 0, 0, 0, 0, 0, 10, 0.7f },
			{ 0, 0, 0, 0, 0.35f, 0, 0, 0, 0, 0, 0.2f, 0, 0.3f, 0, 0, 0, 0.1f, 0.1f, 0.25f, 0.25f, 0, 0, 1.0f, 1.0f, 0, 0, 0, 0, 0.3f, 0.3f, 10, 1.0f },
			{ 0.4f, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0, 0, 0.8f, 0, 0, 0.2f, 0.15f, 0.15f, 0.25f, 0.25f, 0.6f, 0.6f, 0, 0, 0, 0, 0, 1.0f, 0.7f, 0.6f, 12.0f, 0.6f }
		};
		ret.tags = { std::vector{ "Normal", "Happy", "Consensual", "Pleasure" } };
		ret.enabled = true;
		ret.isdefault = true;
		return ret;
	}

	Expression::Profile Expression::GetDefaultSad()
	{
		Profile ret{};
		ret.id = "Sad";
		ret.data[RE::SEXES::kMale] = {
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 2, 0.5f },
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 2, 0.7f },
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0.6f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 2, 0.8f }
		};
		ret.data[RE::SEXES::kFemale] = {
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 2, 0.5f },
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 2, 0.7f },
			{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0.6f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 2, 0.8f },
		};
		ret.tags = { std::vector{ "Normal", "Victim", "Sad" } };
		ret.enabled = true;
		ret.isdefault = true;
		return ret;
	}

	Expression::Profile Expression::GetDefaultShy()
	{
		Profile ret{};
		ret.id = "Shy";
		ret.data[RE::SEXES::kMale] = {
			{ 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0, 4.0f, 0.9f },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.4f, 0, 0, 0.3f, 0, 3.0f, 0.5f },
			{ 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.4f, 0, 0, 0.3f, 0, 3.0f, 0.5f }
		};
		ret.data[RE::SEXES::kFemale] = {
			{ 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0, 4.0f, 0.9f },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.4f, 0, 0, 0.3f, 0, 3.0f, 0.5f },
			{ 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.4f, 0, 0, 0.3f, 0, 3.0f, 0.5f },
		};
		ret.tags = { std::vector{ "Normal", "Consensual", "Nervous", "Sad", "Shy" } };
		ret.enabled = true;
		ret.isdefault = true;
		return ret;
	}

	const Expression::Profile* Expression::GetProfile(const RE::BSFixedString& a_id) const
	{
		auto where = _profiles.find(a_id);
		if (where == _profiles.end())
			return nullptr;

		return &where->second;
	}

	Expression::Profile* Expression::GetProfile(const RE::BSFixedString& a_id)
	{
		auto where = _profiles.find(a_id);
		if (where == _profiles.end())
			return nullptr;

		return &where->second;
	}

	bool Expression::RenameProfile(const RE::BSFixedString& a_id, const RE::BSFixedString& a_newid)
	{
		auto where = _profiles.find(a_id);
		if (where == _profiles.end())
			return false;

		_profiles[a_newid] = where->second;
		_profiles.erase(where);
		return true;
	}

	bool Expression::ForEachProfile(std::function<bool(Profile&)> a_func)
	{
		for (auto&& [id, profile] : _profiles) {
			if (a_func(profile)) {
				return true;
			}
		}
		return false;
	}

	void Expression::Initialize()
	{
		logger::info("Loading Expressions");
		const auto path_legacy = fs::path{ "Data\\SKSE\\Plugins\\SexLab\\" };
		bool has_new = false;
		if (fs::exists(path_legacy) && fs::is_directory(path_legacy)) {
			for (auto& file : fs::directory_iterator{ path_legacy }) {
				auto filename = file.path().filename().string();
				ToLower(filename);
				if (!filename.starts_with("expression"))
					continue;
				try {
					logger::info("Attempting to update legacy file {}", filename);
					const auto jsonfile = nlohmann::json::parse(std::ifstream(file.path().string()));
					const auto profile = Profile{ jsonfile };
					_profiles[profile.id] = std::move(profile);
					logger::info("Added legacy expression {}. You may delete this file now", filename);
				} catch (const std::exception& e) {
					logger::info("Failed to update {}, Error = {}", filename, e.what());
				}
			}
		}
		if (fs::exists(EXPRESSION_PATH) && fs::is_directory(EXPRESSION_PATH)) {
			for (auto& file : fs::directory_iterator{ path_legacy }) {
				auto filename = file.path().filename().string();
				try {
					logger::info("Attempting to parse expression file {}", filename);
					const auto yaml = YAML::LoadFile(file.path().string());
					const auto profile = Profile{ yaml };
					_profiles[profile.id] = std::move(profile);
					logger::info("Added expression {}", filename);
				} catch (const std::exception& e) {
					logger::info("Failed to load {}, Error = {}", filename, e.what());
				}
			}
		}
		if (has_new) {
			Save(false);
		}
		logger::info("Finished loading expressions");
	}

	void Expression::Save(bool verbose)
	{
		if (verbose) {
			logger::info("Saving expressions");
		}
		for (auto&& [id, profile] : _profiles) {
			if (profile.isdefault)
				continue;
			const auto file = profile.AsYAML();
			std::ofstream fout(fmt::format("{}\\{}", EXPRESSION_PATH, id));
			fout << file;
		}
		if (verbose) {
			logger::info("Finished saving expressions");
		}
	}

}	 // namespace Registry
