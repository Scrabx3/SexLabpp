#pragma once

#include "SexLab/DataKey.h"

namespace SLPP::DataKey
{
#define REGISTERFUNC(func, c) a_vm->RegisterFunction(#func##sv, c, func, true)

	using Key = SexLab::DataKey::Key;

	// Building
	int32_t BuildDataKeyNative(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_ref, bool abIsVictim, std::string a_racekey);
	int32_t BuildCustomKey(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, uint32_t a_gender, std::string a_racekey);

	// std::vector<int32_t> BuildDataKeyArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, int32_t aiVictimIdx);
	// std::vector<int32_t> BuildDataKeyArrayEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, std::vector<bool> a_isvictim);

	// std::vector<int32_t> BuildSortedDataKeyArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, int32_t a_victimidx);
	// std::vector<int32_t> BuildSortedDataKeyArrayEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, std::vector<bool> a_victimidx);

	inline int32_t BuildBlankKey(RE::StaticFunctionTag*) { return Key::Blank; }

	// Sort & Compare
	std::vector<int32_t> SortDataKeys(RE::StaticFunctionTag*, const std::vector<int32_t> a_keys);
	bool IsLess(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp);
	bool Match(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp);
	bool MatchArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<int32_t> a_keys, std::vector<int32_t> a_cmp);

	int32_t GetGender(RE::StaticFunctionTag*, uint32_t a_key);
	inline bool IsMale(RE::StaticFunctionTag*, uint32_t a_key) { return GetGender(nullptr, a_key) == 0; }
	inline bool IsFemale(RE::StaticFunctionTag*, uint32_t a_key) { return GetGender(nullptr, a_key) == 1; }
	inline bool IsFuta(RE::StaticFunctionTag*, uint32_t a_key) { return GetGender(nullptr, a_key) == 2; }
	inline bool IsCreature(RE::StaticFunctionTag*, uint32_t a_key) { return GetGender(nullptr, a_key) >= 3; }
	inline bool IsMaleCreature(RE::StaticFunctionTag*, uint32_t a_key) { return GetGender(nullptr, a_key) == 3; }
	inline bool IsFemaleCreature(RE::StaticFunctionTag*, uint32_t a_key) { return GetGender(nullptr, a_key) == 4; }

	inline bool IsVictim(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Victim; }
	inline bool IsVampire(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Vampire; }
	// inline bool IsAmputeeAL(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::AmputeeAL; }
	// inline bool IsAmputeeAR(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::AmputeeAR; }
	// inline bool IsAmputeeLL(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::AmputeeLL; }
	// inline bool IsAmputeeLR(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::AmputeeLR; }
	inline bool IsDead(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Dead; }

	inline bool HasOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & (Key::Overwrite_Female | Key::Overwrite_Male); }
	inline bool IsMaleOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Overwrite_Male; }
	inline bool IsFemaleOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & Key::Overwrite_Female; }

	std::string GetRaceKey(RE::StaticFunctionTag*, uint32_t a_key);

	int32_t GetLegacyGenderByKey(RE::StaticFunctionTag*, uint32_t a_key);
	int32_t BuildByLegacyGender(RE::StaticFunctionTag*, int32_t a_legacygender, std::string a_racekey);

	int32_t AddOverwrite(RE::StaticFunctionTag*, uint32_t a_key, bool a_female);
	int32_t RemoveOverWrite(RE::StaticFunctionTag*, uint32_t a_key, bool a_female);
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

		REGISTERFUNC(GetGender, "sslActorData");
		REGISTERFUNC(IsMale, "sslActorData");
		REGISTERFUNC(IsFemale, "sslActorData");
		REGISTERFUNC(IsFuta, "sslActorData");
		REGISTERFUNC(IsCreature, "sslActorData");
		REGISTERFUNC(IsMaleCreature, "sslActorData");
		REGISTERFUNC(IsFemaleCreature, "sslActorData");

		REGISTERFUNC(IsVictim, "sslActorData");
		REGISTERFUNC(IsVampire, "sslActorData");
		REGISTERFUNC(IsDead, "sslActorData");

		REGISTERFUNC(HasOverwrite, "sslActorData");
		REGISTERFUNC(IsMaleOverwrite, "sslActorData");
		REGISTERFUNC(IsFemaleOverwrite, "sslActorData");

		REGISTERFUNC(GetRaceKey, "sslActorData");

		REGISTERFUNC(GetLegacyGenderByKey, "sslActorData");
		REGISTERFUNC(BuildByLegacyGender, "sslActorData");

		REGISTERFUNC(AddOverwrite, "sslActorData");
		REGISTERFUNC(RemoveOverWrite, "sslActorData");
		REGISTERFUNC(NeutralizeCreatureGender, "sslActorData");

		return true;
	}
}
