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

	void MCMConfig::LoadDefaults()
	{
		logger::info("Loading default configuration");
		try {
			const auto config = YAML::Load("{RestrictAggressive: true,AllowCreatures: false,NPCSaveVoice: false,UseStrapons: true,RedressVictim: true,UndressAnimation: false,UseLipSync: ,true,UseExpressions: true,RefreshExpressions: true,ScaleActors: false,UseCum: true,AllowFFCum: false,DisablePlayer: false,AutoTFC: false,AutoAdvance: true,ForeplayStage: false,OrgasmEffects: false,RaceAdjustments: true,BedRemoveStanding: true,UseCreatureGender: false,LimitedStrip: false,RestrictSameSex: false,SeparateOrgasms: false,RemoveHeelEffect: false,AdjustTargetStage: false,ShowInMap: false,DisableTeleport: true,SeedNPCStats: true,DisableScale: true,LipsFixedValue: true,AnimProfile: 1,AskBed: 1,NPCBed: 0,OpenMouthSize: 80,UseFade: 0,Backwards: 54,AdjustStage: 157,AdvanceAnimation: 57,ChangeAnimation: 24,AdjustChange: 37,AdjustForward: 38,AdjustSideways: 40,AdjustUpward: 39,RealignActors: 26,MoveScene: 27,RestoreOffsets: 12,RotateScene: 22,EndAnimation: 207,ToggleFreeCamera: 81,TargetActor: 49,AdjustSchlong: 46,LipsSoundTime: 0,LipsPhoneme: 1,LipsMinValue: 20,LipsMaxValue: 50,CumTimer: 120,ShakeStrength: 0.699999988,AutoSUCSM: 5,MaleVoiceDelay: 5,FemaleVoiceDelay: 4,ExpressionDelay: 2,VoiceVolume: 1,SFXDelay: 3,SFXVolume: 1,LeadInCoolDown: 0,LipsMoveTime: 0.200000003,iStripForms: [1032555423, 1, 1032554497, 1, 4719365, 1, 16901, 1, 83952148, 0, 83952148, 0, 352389654, 1, 352389654, 1], fTimers: [30, 20, 15, 15, 9, 10, 10, 10, 8, 8, 20, 15, 10, 10, 4], OpenMouthMale: [0, 0.800000012, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16],OpenMouthFemale: [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16], ChangePositions: 13}");
			auto mcm = Script::GetScriptObject(GameForms::ConfigQuest, "sslSystemConfig");
		for (auto&& setting : settings) {
			setting->Load(config, mcm);
		}
	}
	catch (const std::exception& e)
	{
		logger::error(e.what());
	}
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
		auto mcm = Script::GetScriptObject(GameForms::ConfigQuest, "sslSystemConfig");
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
		const auto mcm = Script::GetScriptObject(GameForms::ConfigQuest, "sslSystemConfig");
		for (auto&& setting : settings) {
			setting->Save(config, mcm);
		}
		std::ofstream{ CONFIGPATH("Settings.yaml") } << config;
	} catch (const std::exception& e) {
		logger::error(e.what());
	}
}

}	 // namespace Settings
