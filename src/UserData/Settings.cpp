#include "Settings.h"

#include <SimpleIni.h>

// TODO: Read ini settings

void Settings::Initialize()
{
	const auto& path = CONFIGPATH("Settings.yaml");
	if (!fs::exists(path))
		return;

	try {
		const auto yaml = YAML::LoadFile(path);
		for (auto&& node : yaml) {
			auto keyname = node.first.as<std::string>();
			auto w = table.find(keyname);
			if (w == table.end()) {
				logger::error("Unrecognized setting: {}", keyname);
				continue;
			}

			switch (VariantType(w->second.index())) {
			case VariantType::INT:
				{
					auto s = std::get<int*>(w->second);
					*s = node.second.as<int>();
				}
				break;
			case VariantType::FLOAT:
				{
					auto s = std::get<float*>(w->second);
					*s = node.second.as<float>();
				}
				break;
			case VariantType::BOOL:
				{
					auto s = std::get<bool*>(w->second);
					*s = node.second.as<bool>();
				}
				break;
			case VariantType::INTARRAY:
				{
					auto s = std::get<std::vector<int>*>(w->second);
					const auto value = node.second.as<std::vector<int>>();
          if (value.size() != s->size())
            logger::error("Invalid array length for setting {}, expected {} but got {}", keyname, s->size(), value.size());
					*s = value;
				}
				break;
			case VariantType::FLOATARRAY:
				{
					auto s = std::get<std::vector<float>*>(w->second);
					const auto value = node.second.as<std::vector<float>>();
					if (value.size() != s->size())
						logger::error("Invalid array length for setting {}, expected {} but got {}", keyname, s->size(), value.size());
					*s = value;
				}
			default:
				logger::error("Unreocnigzed setting type for setting {}", keyname);
				break;
			}
		}
		logger::info("Finished loading user settings");
	} catch (const std::exception& e) {
		logger::error("Unable to laod settings, error: {}", e.what());
	}
}

void Settings::InitializeData()
{
	const auto& handler = RE::TESDataHandler::GetSingleton();
	const auto root = YAML::LoadFile(CONFIGPATH("SchlongsOfSkyrim.yaml"));
	for (auto&& i : root["Blacklist"]) {
		const auto esp = i["ESP"].as<std::string>();
		logger::info("Looking for non-schlongs in esp {}", esp);
		const auto node = i["ID"];
		if (node.IsSequence()) {
			for (auto&& id : node) {
				const auto formid = id.as<uint32_t>();
				const auto fac = handler->LookupFormID(formid, esp);
				if (fac) {
					logger::info("Adding {} / {}", esp, formid);
					SOS_ExcludeFactions.push_back(fac);
				}
			}
		} else {	// no sequence, simple entry
			const auto formid = node.as<uint32_t>();
			const auto fac = handler->LookupFormID(formid, esp);
			if (fac) {
				logger::info("Adding {} / {}", esp, formid);
				SOS_ExcludeFactions.push_back(fac);
			}
		}
	}
}

void Settings::Save()
{
	YAML::Node settings{};
	for (auto&& s : table) {
		switch (VariantType(s.second.index())) {
		case VariantType::INT:
			{
				settings[s.first] = *std::get<int*>(s.second);
			}
			break;
		case VariantType::FLOAT:
			{
				settings[s.first] = *std::get<float*>(s.second);
			}
			break;
		case VariantType::BOOL:
			{
				settings[s.first] = *std::get<bool*>(s.second);
			}
			break;
		case VariantType::INTARRAY:
			{
				settings[s.first] = *std::get<std::vector<int>*>(s.second);
			}
			break;
		case VariantType::FLOATARRAY:
			{
				settings[s.first] = *std::get<std::vector<float>*>(s.second);
			}
			break;
		default:
			logger::error("Unreocnigzed setting type for setting {}", s.first);
			break;
		}
	}
	std::ofstream fout(CONFIGPATH("Settings.yaml"));
	fout << settings;
	logger::info("Finished saving user settings");
}