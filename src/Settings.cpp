#include "Settings.h"

void Settings::LoadData()
{
	try {
		const auto& handler = RE::TESDataHandler::GetSingleton();
		const auto root = YAML::LoadFile("Data\\SKSE\\SexLabpp\\SchlongsOfSkyrim.yaml");
		for (auto&& i : root["Blacklist"]) {
			const auto esp = i["ESP"].as<std::string>();
			logger::info("Looking for non-schlongs in esp {}", esp);
			const auto node = i["ID"];
			if (node.IsSequence()) {
				for (auto&& id : node) {
					const auto formid = id.as<uint32_t>();
					const auto fac = handler->LookupForm<RE::TESFaction>(formid, esp);
					if (fac) {
						logger::info("Adding {} / {}", esp, formid);
						SOS_ExcludeFactions.push_back(fac);
					}
				}
			} else {
				// if node isnt sequence, its a single integer
				const auto formid = node.as<uint32_t>();
				const auto fac = handler->LookupForm<RE::TESFaction>(formid, esp);
				if (fac) {
					logger::info("Adding {} / {}", esp, formid);
					SOS_ExcludeFactions.push_back(fac);
				}
			}
		}
		logger::info("Loaded {} Schlongs of Skyrim excluded factions", SOS_ExcludeFactions.size());
	} catch (const std::exception& e) {
		logger::error("Unable to load data. Error: {}", e.what());
	}
}