#include "Settings.h"

namespace Settings
{
	void StripConfig::Load()
	{
		try {
			const auto handler = RE::TESDataHandler::GetSingleton();
			_root = YAML::LoadFile(CONFIGPATH("Stripping.yaml"));
			for (const auto&& mod : _root) {
				const auto esp = mod.first.as<std::string>();
				if (handler->LookupModByName(esp) == nullptr) {
					logger::info("Cannot load strip settings for {}. Plugin is not found", esp);
					continue;
				}
				for (const auto&& i : mod.second) {
					const auto id = i.first.as<uint32_t>();
					const auto formid = handler->LookupFormID(id, esp);
					strips[formid] = Strip(i.second.as<int32_t>());
				}
			}
		} catch (const std::exception& e) {
			logger::error("Unable to load data. Error: {}", e.what());
		}
		logger::info("Loaded {} custom strip settings", strips.size());
	}

	void StripConfig::Save()
	{
		try {
			for (auto&& [formid, strip] : strips) {
				const auto entry = [&]() -> std::pair<std::string, uint32_t> {
					const auto modidx = formid >> 24;
					if (modidx == 0xFE) {
						const auto file = RE::TESDataHandler::GetSingleton()->LookupLoadedLightModByIndex(static_cast<uint16_t>((formid >> 12) & 0xFFF));
						return { file ? file->fileName : ""s, (0xFFF & formid) };
					} else {
						const auto file = RE::TESDataHandler::GetSingleton()->LookupLoadedModByIndex(static_cast<uint8_t>(modidx));
						return { file ? file->fileName : ""s, (0xFFFFFF & formid) };
					}
				}();
				if (entry.first.empty()) {
					logger::error("Unable to find mod for formid {}", formid);
					continue;
				}
				_root[entry.first][entry.second] = static_cast<int32_t>(strip);
			}
			std::ofstream{ CONFIGPATH("Stripping.yaml") } << _root;
		} catch (const std::exception& e) {
			logger::error("Unable to save StripConfig. Error: {}", e.what());
		}
		logger::info("Saved custom strip settings for {} mods", _root.size());
	}

	StripConfig::Strip StripConfig::CheckStrip(RE::TESForm* a_form)
	{
		const auto it = strips.find(a_form->formID);
		if (it == strips.end() || it->second == Strip::None) {
			return CheckKeywords(a_form);
		}
		return it->second;
	}

	StripConfig::Strip StripConfig::CheckKeywords(RE::TESForm* a_form)
	{
		const auto& kwd = a_form->As<RE::BGSKeywordForm>();
		if (kwd) {
			if (kwd->ContainsKeywordString("NoStrip"))
				return Strip::NoStrip;
			if (kwd->ContainsKeywordString("AlwaysStrip"))
				return Strip::Always;
		}
		return Strip::None;
	}

	void LoadData()
	{
		try {
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
				} else {
					// if node isnt sequence, its a single integer
					const auto formid = node.as<uint32_t>();
					const auto fac = handler->LookupFormID(formid, esp);
					if (fac) {
						logger::info("Adding {} / {}", esp, formid);
						SOS_ExcludeFactions.push_back(fac);
					}
				}
			}
		} catch (const std::exception& e) {
			logger::error("Unable to load data. Error: {}", e.what());
		}
		logger::info("Loaded {} Schlongs of Skyrim excluded factions", SOS_ExcludeFactions.size());
	}

	void MCMConfig::Load()
	{
		logger::info("Loading configuration");
		if (!fs::exists(CONFIGPATH("Settings.yaml"))) {
			logger::info("No config file loaded, skipping configuration load");
			return;
		}
		try {
			const auto config = YAML::LoadFile(CONFIGPATH("Settings.yaml"));
			auto mcm = Script::CreateObjectPtr(GameForms::ConfigQuest, "sslSystemConfig");
			for (auto&& setting : settings) {
				setting->Load(config, mcm);
			}
		} catch (const std::exception& e) {
			logger::error(e.what());
		}
	}

	void MCMConfig::Save()
	{
		logger::info("Saving configuration");
		try {
			YAML::Node config{};
			const auto mcm = Script::CreateObjectPtr(GameForms::ConfigQuest, "sslSystemConfig");
			for (auto&& setting : settings) {
				setting->Save(config, mcm);
			}
			std::ofstream{ CONFIGPATH("Settings.yaml") } << config;
		} catch (const std::exception& e) {
			logger::error(e.what());
		}
	}

}	 // namespace Settings
