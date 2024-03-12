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

	std::vector<RE::Actor*> GetAllTrackedActors(RE::StaticFunctionTag*);
	void SetStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id, float a_value);
	float GetStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id);

		std::vector<RE::BSFixedString> GetAllCustomStatIDs(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	bool HasCustomStat(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat);
	void SetCustomStatFlt(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat, float a_value);
	void SetCustomStatStr(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat, RE::BSFixedString a_value);
	float GetCustomStatFlt(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat, float a_default);
	RE::BSFixedString GetCustomStatStr(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat, RE::BSFixedString a_default);
	void DeleteCustomStat(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat);

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
		REGISTERFUNC(GetAllTrackedActors, "sslActorStats", true);
		REGISTERFUNC(SetStatistic, "sslActorStats", true);
		REGISTERFUNC(GetStatistic, "sslActorStats", true);

		REGISTERFUNC(GetAllCustomStatIDs, "sslActorStats", true);
		REGISTERFUNC(HasCustomStat, "sslActorStats", true);
		REGISTERFUNC(SetCustomStatFlt, "sslActorStats", true);
		REGISTERFUNC(SetCustomStatStr, "sslActorStats", true);
		REGISTERFUNC(GetCustomStatFlt, "sslActorStats", true);
		REGISTERFUNC(GetCustomStatStr, "sslActorStats", true);
		REGISTERFUNC(DeleteCustomStat, "sslActorStats", true);

		REGISTERFUNC(GetAllEncounters, "sslActorStats", true);
		REGISTERFUNC(GetAllEncounteredVictims, "sslActorStats", true);
		REGISTERFUNC(GetAllEncounteredAssailants, "sslActorStats", true);
		REGISTERFUNC(GetMostRecentEncounter, "sslActorStats", true);
		REGISTERFUNC(AddEncounter, "sslActorStats", true);
		REGISTERFUNC(GetLastEncounterTime, "sslActorStats", true);
		REGISTERFUNC(GetTimesMet, "sslActorStats", true);
		REGISTERFUNC(GetTimesVictimzed, "sslActorStats", true);
		REGISTERFUNC(GetTimesAssaulted, "sslActorStats", true);
		REGISTERFUNC(GetEncounterTypesCount, "sslActorStats", true);

		REGISTERFUNC(GetEveryStatisticID, "sslActorStats", true);
		REGISTERFUNC(GetLegacyStatistic, "sslActorStats", true);
		REGISTERFUNC(GetAllLegycSkills, "sslActorStats", true);
		REGISTERFUNC(SetLegacyStatistic, "sslActorStats", true);

		return true;
	}
}	 // namespace Papyrus::ActorStats
