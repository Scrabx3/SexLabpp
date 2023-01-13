#include "Settings.h"

void Settings::LoadData()
{
	try {
		const auto& handler = RE::TESDataHandler::GetSingleton();
		const auto root = YAML::LoadFile("Data\\SKSE\\SexLabpp\\SchlongsOfSkyrim.yaml");
		for (auto&& i : root["Blacklist"]) {
			const auto fac = handler->LookupForm<RE::TESFaction>(i["ID"].as<uint32_t>(), i["ESP"].as<std::string>());
			if (fac)
				SOS_ExcludeFactions.push_back(fac);
		}
		logger::info("Loaded {} Schlongs of Skyrim excluded factions", SOS_ExcludeFactions.size());
	} catch (const std::exception& e) {
		logger::error("Unable to load data. Error: {}", e.what());
	}
}