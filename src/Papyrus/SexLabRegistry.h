#pragma once

namespace Papyrus::SexLabRegistry
{
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

		REGISTERFUNC(GetSex, "SexLabRegistry", true);

		return true;
	}
} // namespace Papyrus::SexLabRegistry
