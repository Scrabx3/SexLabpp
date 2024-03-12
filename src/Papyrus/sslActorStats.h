#pragma once

#include "Registry/Stats.h"

namespace Papyrus::ActorStats
{
	static inline constexpr auto Purity{ "SSLLEGACY_Purity" };
	static inline constexpr auto Lewdness{ "SSLLEGACY_Lewd" };
	static inline constexpr auto Foreplay{ "SSLLEGACY_Foreplay" };

	enum class LegacyStatistics
	{
		L_Foreplay,
		XP_Vaginal,
		XP_Anal,
		XP_Oral,
		L_Pure,
		L_Lewd,
		Times_Males,
		Times_Females,
		Times_Creatures,
		Times_Masturbation,
		Times_Aggressor,
		Times_Victim,
		SexCount,		// how often the actor had intercourse in general
		PlayerSex,	// how often the actor had intercourse with the player
		Sexuality,
		TimeSpent,	// time spent in scenes, there is no reference in what scale this is measured
		LastSex_RealTime,
		LastSex_GameTime,
		Times_VaginalCount,
		Times_AnalCount,
		Times_OralCount,

		Total
	};

	std::vector<RE::Actor*> GetAllEncounters(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	std::vector<RE::Actor*> GetAllEncounteredVictims(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	std::vector<RE::Actor*> GetAllEncounteredAssailants(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	RE::Actor* GetMostRecentEncounter(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int a_encountertype);
	void AddEncounter(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner, int a_encountertype);
	float GetLastEncounterTime(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner);
	int GetTimesMet(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner);
	int GetTimesVictimzed(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_assailant);
	int GetTimesAssaulted(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_victim);
	std::vector<int> GetEncounterTypesCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner);

	void ResetStatistics(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);

	std::vector<RE::BSFixedString> GetEveryStatisticID(RE::StaticFunctionTag*);
	float GetLegacyStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id);
	std::vector<float> GetAllLegycSkills(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	void SetLegacyStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id, float a_value);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetAllEncounters, "sslActorAlias", true);
		REGISTERFUNC(GetAllEncounteredVictims, "sslActorAlias", true);
		REGISTERFUNC(GetAllEncounteredAssailants, "sslActorAlias", true);
		REGISTERFUNC(GetMostRecentEncounter, "sslActorAlias", true);
		REGISTERFUNC(AddEncounter, "sslActorAlias", true);
		REGISTERFUNC(GetLastEncounterTime, "sslActorAlias", true);
		REGISTERFUNC(GetTimesMet, "sslActorAlias", true);
		REGISTERFUNC(GetTimesVictimzed, "sslActorAlias", true);
		REGISTERFUNC(GetTimesAssaulted, "sslActorAlias", true);
		REGISTERFUNC(GetEncounterTypesCount, "sslActorAlias", true);

		REGISTERFUNC(GetEveryStatisticID, "sslActorAlias", true);
		REGISTERFUNC(GetLegacyStatistic, "sslActorAlias", true);
		REGISTERFUNC(GetAllLegycSkills, "sslActorAlias", true);
		REGISTERFUNC(SetLegacyStatistic, "sslActorAlias", true);

		return true;
	}
}	 // namespace Papyrus::ActorStats
