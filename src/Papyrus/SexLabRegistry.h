#pragma once

namespace Papyrus::SexLabRegistry
{
	enum class FurniturePreference
	{
		Disallow = 0,
		Default = 1,
		Prefer = 2,
	};

	int32_t GetRaceID(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
  int32_t MapRaceKeyToID(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_racekey);
  std::vector<int32_t> GetRaceIDA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	std::vector<int32_t> MapRaceKeyToIDA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_racekey);
	RE::BSFixedString GetRaceKey(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
  RE::BSFixedString GetRaceKeyByRace(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESRace* a_race);
  RE::BSFixedString MapRaceIDToRaceKey(RE::StaticFunctionTag*, int32_t a_raceid);
  std::vector<RE::BSFixedString> GetRaceKeyA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
  std::vector<RE::BSFixedString> GetRaceKeyByRaceA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESRace* a_race);
  std::vector<RE::BSFixedString> MapRaceIDToRaceKeyA(RE::StaticFunctionTag*, int32_t a_raceid);
  std::vector<RE::BSFixedString> GetAllRaceKeys(RE::StaticFunctionTag*, bool a_ignoreambiguous);
  int32_t GetSex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, bool a_ignoreoverwrite);

	std::vector<RE::BSFixedString> LookupScenes(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissives, FurniturePreference a_furniturepref, RE::TESObjectREFR* a_center);
	std::vector<RE::BSFixedString> LookupScenesA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives, FurniturePreference a_furniturepref, RE::TESObjectREFR* a_center);

	bool ValidateScene(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::string a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive);
	bool ValidateSceneA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::string a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives);
	std::vector<RE::BSFixedString> ValidateScenes(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<std::string> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive);
	std::vector<RE::BSFixedString> ValidateScenesA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<std::string> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives);

	bool SortByScene(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_positions, std::string a_sceneid, bool a_allowfallback);
	int32_t SortBySceneEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_positions, std::vector<std::string> a_sceneids, bool a_allowfallback);


	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetRaceID, "SexLabRegistry", true);
		REGISTERFUNC(MapRaceKeyToID, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceIDA, "SexLabRegistry", true);
		REGISTERFUNC(MapRaceKeyToIDA, "SexLabRegistry", true);

		REGISTERFUNC(GetRaceKey, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyByRace, "SexLabRegistry", true);
		REGISTERFUNC(MapRaceIDToRaceKey, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyA, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyByRaceA, "SexLabRegistry", true);
		REGISTERFUNC(MapRaceIDToRaceKeyA, "SexLabRegistry", true);

		REGISTERFUNC(GetAllRaceKeys, "SexLabRegistry", true);

		REGISTERFUNC(LookupScenes, "SexLabRegistry", true);
		REGISTERFUNC(LookupScenesA, "SexLabRegistry", true);

		REGISTERFUNC(ValidateScene, "SexLabRegistry", true);
		REGISTERFUNC(ValidateSceneA, "SexLabRegistry", true);
		REGISTERFUNC(ValidateScenes, "SexLabRegistry", true);
		REGISTERFUNC(ValidateScenesA, "SexLabRegistry", true);

		REGISTERFUNC(SortByScene, "SexLabRegistry", true);
		REGISTERFUNC(SortBySceneEx, "SexLabRegistry", true);

		return true;
	}
} // namespace Papyrus::SexLabRegistry
