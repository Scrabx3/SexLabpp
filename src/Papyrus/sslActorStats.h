#pragma once

#include "Registry/Stats.h"

namespace Papyrus::ActorStats
{
	std::vector<RE::BSFixedString> GetEveryStatisticID(RE::StaticFunctionTag*);
	float GetLegacyStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id);
	std::vector<float> GetAllLegycSkills(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	void SetLegacyStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id, float a_value);

	inline bool Register(VM* a_vm)
	{

		REGISTERFUNC(GetEveryStatisticID, "sslActorAlias", true);
		REGISTERFUNC(GetLegacyStatistic, "sslActorAlias", true);
		REGISTERFUNC(GetAllLegycSkills, "sslActorAlias", true);
		REGISTERFUNC(SetLegacyStatistic, "sslActorAlias", true);

		return true;
	}
} // namespace Papyrus::ActorStats
