#include "Expression.h"

namespace Registry
{
	Expression::Profile::Profile(const YAML::Node& a_src) :
		id(a_src["id"].IsDefined() ? a_src["id"].as<std::string>() : "Missing Name"),
		version(a_src["version"].IsDefined() ? static_cast<uint8_t>(a_src["version"].as<uint32_t>()) : 0),
		enabled(!a_src["enabled"].IsDefined() || a_src["enabled"].as<bool>()),
		tags(a_src["tags"].IsDefined() ? a_src["tags"].as<std::vector<std::string>>() : std::vector<std::string>{}),
		scaling(a_src["scaling"].IsDefined() ? Scaling(a_src["scaling"].as<int32_t>()) : Scaling::Linear)
	{
		if (!a_src["data"].IsDefined())
			throw std::exception("Missing data values");

		const auto fill = [&](RE::SEXES::SEX sex) {
			auto& fields = a_src["data"][static_cast<uint32_t>(sex)];
			for (auto&& arr : fields) {
				const auto values = arr.as<std::vector<float>>();
				if (values.size() != Profile::Total) {
					const auto err = std::format("Invalid value field, expected {}/32 values found", values.size());
					throw std::exception(err.c_str());
				}
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
			for (size_t i = 1; i <= MAX_VALUE_FIELDS; i++) {
				auto fieldname = begin + std::to_string(i);
				auto field = floats->find(fieldname);
				if (field == floats->end() || !field->is_array())
					break;
				auto values = field->get<std::vector<float>>();
				if (values.size() != Profile::Total) {
					const auto err = std::format("Invalid value field, expected {}/32 values found in field {}", values.size(), fieldname);
					throw std::exception(err.c_str());
				}
				auto& it = data[sex].emplace_back();
				std::copy_n(values.begin(), it.size(), it.begin());
			}
		};
		fill(RE::SEXES::kMale, "male");
		fill(RE::SEXES::kFemale, "female");
		if (data[RE::SEXES::kMale].empty() && data[RE::SEXES::kFemale].empty()) {
			throw std::exception("Data fields have no values");
		}
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

	Expression::Profile::Profile(DefaultExpression a_default) :
		id(magic_enum::enum_name(a_default)), version(1), scaling(Scaling::Linear)
	{
		switch (a_default) {
		case DefaultExpression::Afraid:
			data[RE::SEXES::kMale] = {
				{ 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0.1f, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0.3f, 0.3f, 3, 1 },
				{ 0.3f, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0.9f, 0, 0, 9, 1 }
			};
			data[RE::SEXES::kFemale] = {
				{ 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0.1f, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0.3f, 0.3f, 3, 1 },
				{ 0.3f, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0.9f, 0, 0, 9, 1 },
			};
			tags = { std::vector{ "Aggressor", "Afraid", "Scared", "Pain", "Negative" } };
			break;
		case DefaultExpression::Angry:
			data[RE::SEXES::kMale] = {
				{ 0, 0, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.3f, 0, 0.4f },
				{ 0, 0, 0, 0.4f, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.65f, 0.65f, 0, 1 }
			};
			data[RE::SEXES::kFemale] = {
				{ 0, 0, 0, 0, 0.4f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.3f, 0, 0.4f },
				{ 0, 0, 0, 0.4f, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.65f, 0.65f, 0, 1 },
			};
			tags = { std::vector{ "Aggressor", "Victim", "Mad", "Angry", "Upset" } };
			break;
		case DefaultExpression::Happy:
			data[RE::SEXES::kMale] = {
				{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 2.0f, 0.5f },
				{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0.7f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 2.0f, 0.8f }
			};
			data[RE::SEXES::kFemale] = {
				{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 2.0f, 0.5f },
				{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0.7f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 2.0f, 0.8f }
			};
			tags = { std::vector{ "Normal", "Happy", "Consensual" } };
			break;
		case DefaultExpression::Joy:
			data[RE::SEXES::kMale] = {
				{ 0, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0.2f, 0, 0, 0, 0, 0, 0, 13.0f, 0.4f },
				{ 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 0, 0, 0, 0, 0.3f, 0.3f, 13.0f, 0.8f }
			};
			data[RE::SEXES::kFemale] = {
				{ 0.3f, 0, 0, 0, 0, 0, 0, 0.6f, 0, 0, 0, 0, 0.6f, 0, 0, 0, 0.3f, 0.3f, 0, 0, 1.0f, 1.0f, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 10, 0.45f },
				{ 0.6f, 0, 0, 0, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.3f, 0.7f, 0.7f, 1.0f, 1.0f, 0, 0, 0, 0, 0, 0, 0.7f, 0.7f, 10, 0.6f },
			};
			tags = { std::vector{ "Normal", "Happy", "Joy", "Pleasure", "Consensual" } };
			break;
		case DefaultExpression::Pained:
			data[RE::SEXES::kMale] = {
				{ 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0.1f, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0.3f, 0.3f, 3, 1 },
				{ 0.3f, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0.9f, 0, 0, 9, 1 }
			};
			data[RE::SEXES::kFemale] = {
				{ 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.3f, 0.1f, 0.1f, 0, 0, 0.5f, 0.5f, 0, 0, 0, 0, 0.3f, 0.3f, 3, 1 },
				{ 0.3f, 0, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0.9f, 0, 0, 9, 1 }
			};
			tags = { std::vector{ "Victim", "Afraid", "Pain", "Pained", "Negative" } };
			break;
		case DefaultExpression::Pleasure:
			data[RE::SEXES::kMale] = {
				{ 0, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0.2f, 0, 0, 0, 0, 0, 0, 13.0f, 0.4f },
				{ 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 0, 0, 0, 0, 0.3f, 0.3f, 13.0f, 0.8f }
			};
			data[RE::SEXES::kFemale] = {
				{ 0, 0, 0, 0, 0, 0.3f, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.0f, 0.3f },
				{ 0.4f, 0, 0, 0, 0, 0.2f, 0, 0, 0, 0, 0, 0, 0.8f, 0, 0, 0.2f, 0.15f, 0.15f, 0.25f, 0.25f, 0.6f, 0.6f, 0, 0, 0, 0, 0, 1.0f, 0.7f, 0.6f, 12.0f, 0.6f }
			};
			tags = { std::vector{ "Normal", "Happy", "Consensual", "Pleasure" } };
			break;
		case DefaultExpression::Sad:
			data[RE::SEXES::kMale] = {
				{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 2, 0.5f },
				{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0.6f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 2, 0.8f }
			};
			data[RE::SEXES::kFemale] = {
				{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.5f, 2, 0.5f },
				{ 0, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 0, 0.6f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f, 0.8f, 2, 0.8f },
			};
			tags = { std::vector{ "Normal", "Victim", "Sad" } };
			break;
		case DefaultExpression::Shy:
			data[RE::SEXES::kMale] = {
				{ 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0, 4.0f, 0.9f },
				{ 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.4f, 0, 0, 0.3f, 0, 3.0f, 0.5f }
			};
			data[RE::SEXES::kFemale] = {
				{ 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2f, 0, 0, 4.0f, 0.9f },
				{ 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5f, 0.4f, 0, 0, 0.3f, 0, 3.0f, 0.5f },
			};
			tags = { std::vector{ "Normal", "Consensual", "Nervous", "Sad", "Shy" } };
			break;
		default:
			assert(false);
			break;
		}
	}

	std::array<float, Expression::Profile::Total> Expression::Profile::GetData(RE::SEXES::SEX a_sex, float a_strength) const
	{
		if (version < 1) {
			if (data[a_sex].empty()) {
				auto ret = std::array<float, Total>{};
				ret[MoodType] = 7;
				return ret;
			}
			const auto max = static_cast<float>(data[a_sex].size()) - 1;
			const auto idx = static_cast<size_t>(std::floor((max * a_strength) / 100));
			assert(idx < data[a_sex].size() && idx >= 0);
			return data[a_sex][idx];
		} else if (data[a_sex].size() < 2) {
			logger::error("Invalid Expression Profile, {}/2 Profiles present", data[a_sex].size());
			auto ret = std::array<float, Total>{};
			ret[MoodType] = 7;
			return ret;
		}
		float multiplier;
		switch (scaling) {
		case Scaling::Linear:
			multiplier = a_strength / 100.0f;
			break;
		case Scaling::Square:
			multiplier = 0.0f;
			for (float i = 0.0f; i <= a_strength / 100.0f; i += 0.07f) {
				const auto f = pow(i - 0.55f, 2.0f) - 0.04f;
				multiplier += std::max<float>(0.0f, f);
			}
			break;
		case Scaling::Cubic:
			multiplier = 0.0f;
			for (float i = 0.0f; i <= a_strength / 100.0f; i += 0.05f) {
				const auto f = 1.1f * pow(i - 0.6f, 3.0f) + 0.08f;
				multiplier += std::max<float>(0.0f, f);
			}
			break;
		case Scaling::Exponential:
			multiplier = 0.0f;
			for (float i = 0.0f; i <= a_strength / 100.0f; i += 0.05f) {
				const auto f = static_cast<float>(pow(2048, i - 1.1f));
				multiplier += std::max<float>(0.0f, f);
			}
			break;
		}
		multiplier = std::min(1.25f, multiplier);
		std::array<float, Total> ret{};
		for (size_t i = 0; i < ret.size(); i++) {
			ret[i] = data[a_sex][0][i] + (data[a_sex][1][i] - data[a_sex][0][i]) * multiplier;
			if (i == MoodType) {
				ret[i] = std::round(ret[i]);
			}
		}
		return ret;
	}

	void Expression::Profile::Save() const
	{
		if (!has_edits)
			return;

		has_edits = false;
		YAML::Node file;
		file["id"] = id.data();
		file["version"] = static_cast<int32_t>(version);
		file["scaling"] = static_cast<int32_t>(scaling);
		for (auto&& tag : tags.AsVector()) {
			file["tags"].push_back(tag.data());
		}
		for (size_t i = 0; i < RE::SEXES::kTotal; i++) {
			if (data[i].empty()) {
				file["data"][i].push_back(std::vector<int>(32, 0));
			} else {
				for (size_t n = 0; n < data[i].size(); n++) {
					file["data"][i].push_back(data[i][n]);
				}
			}
		}
		file["enabled"] = enabled;
		std::ofstream fout(fmt::format("{}\\{}.yaml", EXPRESSIONPATH, id));
		fout << file;
	}

	const Expression::Profile* Expression::GetProfile(const RE::BSFixedString& a_id) const
	{
		auto where = _profiles.find(a_id);
		if (where == _profiles.end())
			return nullptr;

		return &where->second;
	}

	bool Expression::CreateProfile(const RE::BSFixedString& a_id)
	{
		if (_profiles.contains(a_id)) {
			logger::error("Expression {} has already been initialized", a_id);
			return false;
		}
		_profiles.emplace(a_id, Profile{ a_id });
		return true;
	}

	bool Expression::ForEachProfile(std::function<bool(const Profile&)> a_func)
	{
		for (auto&& [id, profile] : _profiles) {
			if (a_func(profile)) {
				return true;
			}
		}
		return false;
	}

	void Expression::UpdateValues(RE::BSFixedString a_id, bool a_female, int a_level, std::vector<float> a_values)
	{
		auto w = _profiles.find(a_id);
		if (w == _profiles.end())
			return;

		w->second.has_edits = true;
		auto& data = w->second.data[a_female];
		while (data.size() <= a_level) {
			data.emplace_back();
		}
		std::copy_n(a_values.begin(), data[a_level].size(), data[a_level].begin());
	}

	void Expression::UpdateTags(RE::BSFixedString a_id, const TagData& a_newtags)
	{
		auto w = _profiles.find(a_id);
		if (w == _profiles.end())
			return;

		w->second.has_edits = true;
		w->second.tags = a_newtags;
	}

	void Expression::SetScaling(RE::BSFixedString a_id, Profile::Scaling a_scaling)
	{		
		auto w = _profiles.find(a_id);
		if (w == _profiles.end())
			return;

		w->second.has_edits = true;
		w->second.scaling = a_scaling;
	}

	void Expression::SetEnabled(RE::BSFixedString a_id, bool a_enabled)
	{
		auto w = _profiles.find(a_id);
		if (w == _profiles.end())
			return;

		w->second.has_edits = true;
		w->second.enabled = a_enabled;
	}

	void Expression::Initialize()
	{
		logger::info("Loading Expressions");

		if (fs::exists(EXPRESSIONPATH) && fs::is_directory(EXPRESSIONPATH)) {
			for (auto& file : fs::directory_iterator{ LEGACY_CONFIG }) {
				auto filename = file.path().filename().string();
				try {
					const auto yaml = YAML::LoadFile(file.path().string());
					auto profile = Profile{ yaml };
					if (_profiles.emplace(profile.id, std::move(profile)).second) {
						logger::info("Added expression {}", filename);
					}
				} catch (const std::exception& e) {
					logger::info("Failed to load {}, Error = {}", filename, e.what());
				}
			}
		}

		if (fs::exists(LEGACY_CONFIG) && fs::is_directory(LEGACY_CONFIG)) {
			for (auto& file : fs::directory_iterator{ LEGACY_CONFIG }) {
				auto filename = file.path().filename().string();
				ToLower(filename);
				if (!filename.starts_with("expression"))
					continue;
				try {
					const auto jsonfile = nlohmann::json::parse(std::ifstream(file.path().string()));
					auto profile = Profile{ jsonfile };
					auto succ = _profiles.emplace(profile.id, std::move(profile));
					if (succ.second) {
						succ.first->second.has_edits = true;
						succ.first->second.Save();
						logger::info("Added legacy expression {}. You may delete this file now", filename);
					}
				} catch (const std::exception& e) {
					logger::info("Failed to update {}, Error = {}", filename, e.what());
				}
			}
		}

		constexpr auto arr = magic_enum::enum_values<DefaultExpression>();
		for (auto&& e : arr) {
			_profiles.emplace(magic_enum::enum_name(e), Profile{ e });
		}

		logger::info("Loaded {} expressions", _profiles.size());
	}

	void Expression::Save()
	{
		for (auto&& [id, profile] : _profiles) {
			profile.Save();
		}
		logger::info("Finished saving expressions");
	}

}	 // namespace Registry
