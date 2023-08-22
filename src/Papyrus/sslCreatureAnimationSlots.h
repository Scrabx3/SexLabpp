#pragma once

namespace Papyrus::CreatureAnimationSlots
{
	std::vector<RE::BSFixedString> GetByRaceKeyTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst,
		int32_t a_actorcount, RE::BSFixedString a_racekey, std::vector<std::string_view> a_tags);
	std::vector<RE::BSFixedString> GetByCreatureActorsTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst,
		int32_t a_actorcount, std::vector<RE::Actor*> a_creatures, std::vector<std::string_view> a_tags);
	std::vector<RE::BSFixedString> GetByRaceGendersTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst,
		int32_t a_actorcount, RE::BSFixedString a_racekey, int32_t a_malecrt, int32_t a_femalecrt, std::vector<std::string_view> a_tags);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetByRaceKeyTagsImpl, "sslCreatureAnimationSlots", true);
		REGISTERFUNC(GetByCreatureActorsTagsImpl, "sslCreatureAnimationSlots", true);
		REGISTERFUNC(GetByRaceGendersTagsImpl, "sslCreatureAnimationSlots", true);

		return true;
	}
}	 // namespace Papyrus::CreatureAnimationSlots
