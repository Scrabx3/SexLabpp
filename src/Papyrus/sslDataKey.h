#pragma once

namespace SLPP::DataKey
{
#define REGISTERFUNC(func, c) a_vm->RegisterFunction(#func##sv, c, func, true)

	/**	DataKeys are defines as follows: (Why not making it an enum? Idk)
	 * 0  - Female
	 * 1  - Male
	 * 2  - Futa
	 * 3  - FCr
	 * 4  - MCr
	 * 5  - OVERRIDE - MALE
	 * 6  - OVERRIDE - FEMALE
	 * 7  - UNDEFINED
	 * 8  - CreatureType Bit  0 - 1
	 * 9  - CreatureType Bit  2 - 3
	 * 10 - CreatureType Bit  4 - 7
	 * 11 - CreatureType Bit  8 - 15
	 * 12 - CreatureType Bit 16 - 31
	 * 13 - CreatureType Bit 32 - 63
	 * 14 - CreatureType Bit 64 - 127
	 * 15 - Pad16
	 * 16 - IsVictim
	 * 17 - IsVampire
	 * 18 - Pad19
	 * 19 - Pad20
	 * 20 - Pad21
	 * 21 - Pad22
	 * 22 - Pad23
	 * 23 - Pad24
	 * 24 - Pad25
	 * 25 - Pad26
	 * 26 - Pad27
	 * 27 - Pad28
	 * 28 - Pad29
	 * 29 - Pad30
	 * 30 - Pad31
	 * 31 - Blank Key Indicator
	 */

	// Building
	uint32_t BuildDataKeyNative(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_ref, bool abIsVictim, uint32_t a_raceid);

	// std::vector<uint32_t> BuildDataKeyArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, int32_t aiVictimIdx);
	// std::vector<uint32_t> BuildDataKeyArrayEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, std::vector<bool> a_isvictim);

	// std::vector<uint32_t> BuildSortedActorKeyArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, int32_t a_victimidx);
	// std::vector<uint32_t> BuildSortedActorKeyArrayEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_ref, std::vector<bool> a_victimidx);

	// Sort & Compare
	std::vector<uint32_t> SortDataKeys(RE::StaticFunctionTag*, std::vector<uint32_t> a_keys);
	bool IsLess(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp);
	bool Match(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp);
	bool MatchArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<uint32_t> a_key, std::vector<uint32_t> a_cmp);

	inline bool IsMale(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & SexLab::Gender::Male; }
	inline bool IsFemale(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & SexLab::Gender::Female; }
	inline bool IsPureFemale(RE::StaticFunctionTag*, uint32_t a_key) { return (a_key & (SexLab::Gender::Female | SexLab::Gender::Futa)) == SexLab::Gender::Female; }
	inline bool IsFuta(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & SexLab::Gender::Futa; }
	inline bool IsCreature(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & (SexLab::Gender::Crt_Male | SexLab::Gender::Crt_Female); }
	inline bool IsMaleCreature(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & SexLab::Gender::Crt_Male; }
	inline bool IsFemaleCreature(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & SexLab::Gender::Crt_Female; }

	inline bool IsVictim(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & (1 << 16); }
	inline bool IsVampire(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & (1 << 17); }

	inline bool HasOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & (SexLab::Gender::Overwrite_Female | SexLab::Gender::Overwrite_Male); }
	inline bool IsMaleOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & SexLab::Gender::Overwrite_Male; }
	inline bool IsFemaleOverwrite(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & SexLab::Gender::Overwrite_Female; }

	inline uint32_t GetRaceID(RE::StaticFunctionTag*, uint32_t a_key) { return a_key & 0xFF00; }
	// std::string GetRaceKey(RE::StaticFunctionTag*, uint32_t a_key);
	// uint32_t GetRaceIDByRaceKey(RE::StaticFunctionTag*, std::string a_racekey);

	uint32_t GetLegacyGenderByKey(RE::StaticFunctionTag*, uint32_t a_key);
	uint32_t BuildByLegacyGenderNative(RE::StaticFunctionTag*, uint32_t a_legacygender, int a_raceid);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(BuildDataKeyNative, "sslActorData");

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

		return true;
	}
}
