#pragma once

namespace Papyrus::AnimationSlots
{
	std::vector<RE::BGSRefAlias*> GetByTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int32_t a_actorcount, std::vector<std::string_view> a_tags);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetByTagsImpl, "sslAnimationSlots", true);

		return true;
	}
} // namespace Papyrus::AnimationSlots
