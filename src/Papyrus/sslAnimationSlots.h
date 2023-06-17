#pragma once

namespace Papyrus::AnimationSlots
{
	int32_t GetAllocatedSize(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst);

	std::vector<RE::BGSRefAlias*> GetByTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int32_t a_actorcount, std::vector<std::string_view> a_tags);
	std::vector<RE::BGSRefAlias*> GetByTypeImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int32_t a_actorcount, int32_t a_males, int32_t a_females, std::vector<std::string_view> a_tags);
	std::vector<RE::BGSRefAlias*> PickByActorsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions, std::vector<std::string_view> a_tags);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetAllocatedSize, "sslAnimationSlots", true);

		REGISTERFUNC(GetByTagsImpl, "sslAnimationSlots", true);
		REGISTERFUNC(GetByTypeImpl, "sslAnimationSlots", true);
		REGISTERFUNC(PickByActorsImpl, "sslAnimationSlots", true);

		return true;
	}
} // namespace Papyrus::AnimationSlots
