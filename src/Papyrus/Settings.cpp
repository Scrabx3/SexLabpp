#include "Settings.h"

namespace Settings
{
	void StripConfig::Load()
	{
		try {
			const auto read = [](const std::vector<std::string>& vec, std::vector<RE::FormID>& dest) -> void {
				const auto handler = RE::TESDataHandler::GetSingleton();
				for (auto&& str : vec) {
					const auto w = str.find('|');
					if (w == std::string::npos) {
						logger::error("Entry {} has invalid syntax", str);
						continue;
					}
					const auto id = std::stoi(str.substr(0, w).data());
					const auto esp = str.substr(w + 1);
					const auto formid = handler->LookupFormID(id, esp);
					if (formid)
						dest.push_back(formid);
				}
			};
			_root = YAML::LoadFile(CONFIGPATH("Stripping.yaml"));
			read(_root["AlwaysStrip"].as<std::vector<std::string>>(), AlwaysStrip);
			read(_root["NoStrip"].as<std::vector<std::string>>(), NoStrip);
		} catch (const std::exception& e) {
			logger::error("Unable to load data. Error: {}", e.what());
		}
		logger::info("Loaded {} \'NoStrip\' and {} \'AlwaysStrip\' armors", NoStrip.size(), AlwaysStrip.size());
	}

	void StripConfig::Save()
	{
		try {
			const auto save = [this](const std::vector<RE::FormID>& vec, const char* access) -> void {
				for (auto&& formid : vec) {
					std::string modname;
					uint32_t rawid;
					const auto modidx = formid >> 24;
					if (modidx == 254) {
						const auto file = RE::TESDataHandler::GetSingleton()->LookupLoadedLightModByIndex(static_cast<uint16_t>(formid >> 12));
						modname = file ? file->fileName : ""s;
						rawid = 0xFFF;
					} else {
						const auto file = RE::TESDataHandler::GetSingleton()->LookupLoadedModByIndex(static_cast<uint8_t>(modidx));
						modname = file ? file->fileName : ""s;
						rawid = 0xFFFFFF;
					}
					if (modname.empty() || !rawid) {	// Dont wanna save runtime generated objects
						logger::error("Unable to identify modname for id {}", formid);
						continue;
					}
					std::stringstream s{};
					s << "0x" << std::hex << rawid;
					_root[access].push_back(std::string{ s.str() + '|' + modname });
				}
			};
			save(AlwaysStrip, "AlwaysStrip");
			save(NoStrip, "NoStrip");

			std::ofstream{ CONFIGPATH("Stripping.yaml") } << _root;
		} catch (const std::exception& e) {
			logger::error("Unable to save StripConfig. Error: {}", e.what());
		}
	}

	void StripConfig::AddArmor(RE::TESForm* a_form, Strip a_type)
	{
		switch (a_type) {
		case Strip::Always:
			AlwaysStrip.push_back(a_form->formID);
			std::sort(AlwaysStrip.begin(), AlwaysStrip.end());
			break;
		case Strip::Never:
			NoStrip.push_back(a_form->formID);
			std::sort(NoStrip.begin(), NoStrip.end());
			break;
		default:
			logger::error("<StripConfig::AddArmor> Invalid stripping type {}", a_type);
			break;
		}
	}

	void StripConfig::RemoveArmor(RE::TESForm* a_form, Strip a_type){
		switch (a_type) {
		case Strip::Always:
			{
				const auto r = std::remove(AlwaysStrip.begin(), AlwaysStrip.end(), a_form->formID);
				AlwaysStrip.erase(r, AlwaysStrip.end());
				break;
			}
		case Strip::Never:
			{
				const auto r = std::remove(NoStrip.begin(), NoStrip.end(), a_form->formID);
				NoStrip.erase(r, NoStrip.end());
				break;
			}
		default:
			logger::error("<StripConfig::AddArmor> Invalid stripping type {}", a_type);
			break;
		}
	}

	StripConfig::Strip StripConfig::CheckStrip(RE::TESForm* a_form)
	{
		const auto id = a_form->formID;
		if (std::binary_search(NoStrip.begin(), NoStrip.end(), id))
			return Strip::Never;

		if (std::binary_search(AlwaysStrip.begin(), AlwaysStrip.end(), id))
			return Strip::Always;

		return Strip::Never;		
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

}	 // namespace Settings
