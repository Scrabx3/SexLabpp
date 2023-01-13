#pragma once

#include "SexLab/DataKey.h"

namespace SLPP::DataKey
{
#define REGISTERFUNC(func, c) a_vm->RegisterFunction(#func##sv, c, func)

	using Key = SexLab::DataKey::Key;

	// Building
	int32_t BuildDataKeyNative(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_ref, bool abIsVictim, uint32_t a_raceid);
	int32_t BuildCustomKey(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, uint32_t a_gender, uint32_t a_raceid, std::vector<bool> a_extradata);

	// std::vector<int32_t> BuildDataKeyArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, int32_t aiVictimIdx);
	// std::vector<int32_t> BuildDataKeyArrayEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, std::vector<bool> a_isvictim);

	// std::vector<int32_t> BuildSortedDataKeyArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, int32_t a_victimidx);
	// std::vector<int32_t> BuildSortedDataKeyArrayEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, std::vector<bool> a_victimidx);

	inline int32_t BuildBlankKey(RE::StaticFunctionTag*) { return 1U << 31; }

	// Sort & Compare
	std::vector<int32_t> SortDataKeys(RE::StaticFunctionTag*, std::vector<int32_t> a_keys);
	bool IsLess(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp);
	bool Match(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp);
	bool MatchArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<int32_t> a_key, std::vector<int32_t> a_cmp);

	inline bool IsMale(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Male; }
	inline bool IsFemale(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Female; }
	inline bool IsPureFemale(RE::StaticFunctionTag*, uint32_t a_key) { return (a_key & Key::Human) == Key::Female; }
	inline bool IsFuta(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Futa; }
	inline bool IsCreature(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Creature; }
	inline bool IsMaleCreature(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Crt_Male; }
	inline bool IsFemaleCreature(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Crt_Female; }

	inline bool IsVictim(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Victim; }
	inline bool IsVampire(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Vampire; }

	inline bool HasOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & (Key::Overwrite_Female | Key::Overwrite_Male); }
	inline bool IsMaleOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Overwrite_Male; }
	inline bool IsFemaleOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Overwrite_Female; }

	inline int32_t GetRaceID(RE::StaticFunctionTag*, uint32_t a_key) { return (a_key & Key::CrtTotal) >> 8; }

	// std::string GetRaceKey(RE::StaticFunctionTag*, uint32_t a_key);
	// uint32_t GetRaceIDByRaceKey(RE::StaticFunctionTag*, std::string a_racekey);

	int32_t GetLegacyGenderByKey(RE::StaticFunctionTag*, uint32_t a_key);
	int32_t BuildByLegacyGenderNative(RE::StaticFunctionTag*, int32_t a_legacygender, int a_raceid);

	int32_t AddGenderToKey(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_gender);
	int32_t RemoveGenderFromKey(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_gender);
	void NeutralizeCreatureGender(RE::StaticFunctionTag*, std::vector<int32_t> a_keys);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(BuildDataKeyNative, "sslActorData");
		REGISTERFUNC(BuildBlankKey, "sslActorData");
		REGISTERFUNC(BuildCustomKey, "sslActorData");

		REGISTERFUNC(SortDataKeys, "sslActorData");
		REGISTERFUNC(IsLess, "sslActorData");
		REGISTERFUNC(Match, "sslActorData");
		REGISTERFUNC(MatchArray, "sslActorData");

		REGISTERFUNC(IsMale, "sslActorData");
		REGISTERFUNC(IsFemale, "sslActorData");
		REGISTERFUNC(IsPureFemale, "sslActorData");
		REGISTERFUNC(IsFuta, "sslActorData");
		REGISTERFUNC(IsCreature, "sslActorData");
		REGISTERFUNC(IsMaleCreature, "sslActorData");
		REGISTERFUNC(IsFemaleCreature, "sslActorData");
		REGISTERFUNC(IsVictim, "sslActorData");
		REGISTERFUNC(IsVampire, "sslActorData");
		REGISTERFUNC(HasOverwrite, "sslActorData");
		REGISTERFUNC(IsMaleOverwrite, "sslActorData");
		REGISTERFUNC(IsFemaleOverwrite, "sslActorData");

		REGISTERFUNC(GetRaceID, "sslActorData");

		REGISTERFUNC(GetLegacyGenderByKey, "sslActorData");
		REGISTERFUNC(BuildByLegacyGenderNative, "sslActorData");

		REGISTERFUNC(AddGenderToKey, "sslActorData");
		REGISTERFUNC(RemoveGenderFromKey, "sslActorData");
		REGISTERFUNC(NeutralizeCreatureGender, "sslActorData");

		return true;
	}
}
