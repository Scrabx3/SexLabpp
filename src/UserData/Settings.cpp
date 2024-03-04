#include "Settings.h"

#include <SimpleIni.h>

void Settings::Initialize()
{
	InitializeYAML();
	InitializeINI();
}

void Settings::InitializeYAML()
{
	if (!fs::exists(YAMLPATH)) {
		logger::error("No Settings file (yaml) in {}", YAMLPATH);
		return;
	}

	try {
		const auto yaml = YAML::LoadFile(YAMLPATH);
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
			case VariantType::STRING:
				{
					auto s = std::get<std::string*>(w->second);
					*s = node.second.as<std::string>();
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
		logger::info("Finished loading yaml settings");
	} catch (const std::exception& e) {
		logger::error("Unable to laod settings, error: {}", e.what());
	}
}

void Settings::InitializeINI()
{
	if (!fs::exists(INIPATH)) {
		logger::error("No Settings file (ini) in {}", INIPATH);
		return;
	}

	CSimpleIniA inifile{};
	inifile.SetUnicode();
	const auto ec = inifile.LoadFile(INIPATH);
	if (ec < 0) {
		logger::error("Failed to read .ini Settings, Error: {}", ec);
		return;
	}

	const auto ReadIni = [&inifile]<typename T>(const char* a_section, const char* a_option, T& a_out) {
		if (!inifile.GetValue(a_section, a_option))
			return;

		if constexpr (std::is_same<T, int>::value || std::is_same<T, uint32_t>::value) {
			a_out = static_cast<T>(inifile.GetLongValue(a_section, a_option));
		} else if (std::is_same<T, float>::value) {
			a_out = static_cast<T>(inifile.GetDoubleValue(a_section, a_option));
		} else if (std::is_same<T, bool>::value) {
			a_out = static_cast<T>(inifile.GetBoolValue(a_section, a_option));
		}
	};
#define READINI(section, out) ReadIni(section, #out, out);

	// Animation
	READINI("Animation", iFurniturePrefWeight);
	READINI("Animation", fScanRadius);
	READINI("Animation", fMinScale);
	READINI("Animation", bAllowDead);

	// Creature
	READINI("Creature", bAshHopper)
	READINI("Creature", bBear)
	READINI("Creature", bBoar)
	READINI("Creature", bBoarMounted)
	READINI("Creature", bBoarSingle)
	READINI("Creature", bCanine)
	READINI("Creature", bChaurus)
	READINI("Creature", bChaurusHunter)
	READINI("Creature", bChaurusReaper)
	READINI("Creature", bChicken)
	READINI("Creature", bCow)
	READINI("Creature", bDeer)
	READINI("Creature", bDog)
	READINI("Creature", bDragon)
	READINI("Creature", bDragonPriest)
	READINI("Creature", bDraugr)
	READINI("Creature", bDwarvenBallista)
	READINI("Creature", bDwarvenCenturion)
	READINI("Creature", bDwarvenSphere)
	READINI("Creature", bDwarvenSpider)
	READINI("Creature", bFalmer)
	READINI("Creature", bFlameAtronach)
	READINI("Creature", bFox)
	READINI("Creature", bFrostAtronach)
	READINI("Creature", bGargoyle)
	READINI("Creature", bGiant)
	READINI("Creature", bGiantSpider)
	READINI("Creature", bGoat)
	READINI("Creature", bHagraven)
	READINI("Creature", bHare)
	READINI("Creature", bHorker)
	READINI("Creature", bHorse)
	READINI("Creature", bIceWraith)
	READINI("Creature", bLargeSpider)
	READINI("Creature", bLurker)
	READINI("Creature", bMammoth)
	READINI("Creature", bMudcrab)
	READINI("Creature", bNetch)
	READINI("Creature", bRiekling)
	READINI("Creature", bSabrecat)
	READINI("Creature", bSeeker)
	READINI("Creature", bSkeever)
	READINI("Creature", bSlaughterfish)
	READINI("Creature", bSpider)
	READINI("Creature", bSpriggan)
	READINI("Creature", bStormAtronach)
	READINI("Creature", bTroll)
	READINI("Creature", bVampireLord)
	READINI("Creature", bWerewolf)
	READINI("Creature", bWisp)
	READINI("Creature", bWispmother)
	READINI("Creature", bWolf)

	// Distance
	READINI("Distance", fDistanceHead);
	READINI("Distance", fDistanceFoot);
	READINI("Distance", fDistanceHand);
	READINI("Distance", fDistanceCrotchFront);
	READINI("Distance", fDistanceCrotchBack);
	READINI("Distance", fDistanceCrotchBonus);
	READINI("Distance", fAnglePenetration);
	READINI("Distance", fAngleMouth);

#undef READINI

	logger::info("Finished loading .ini settings");
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
		case VariantType::STRING:
			{
				settings[s.first] = *std::get<std::string*>(s.second);
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
