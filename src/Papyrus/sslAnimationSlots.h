#pragma once

namespace Papyrus::AnimationSlots
{
	std::vector<RE::BSFixedString> GetByTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int32_t a_actorcount, std::vector<std::string_view> a_tags);
	std::vector<RE::BSFixedString> GetByTypeImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int32_t a_actorcount, int32_t a_males, int32_t a_females, std::vector<std::string_view> a_tags);
	std::vector<RE::BSFixedString> PickByActorsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions, std::vector<std::string_view> a_tags);

	std::vector<RE::BSFixedString> CreateProxyArray(RE::TESQuest* a_qst, uint32_t a_returnsize, uint32_t crt_specifier);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetByTagsImpl, "sslAnimationSlots", true);
		REGISTERFUNC(GetByTypeImpl, "sslAnimationSlots", true);
		REGISTERFUNC(PickByActorsImpl, "sslAnimationSlots", true);

		REGISTERFUNC(CreateProxyArray, "sslAnimationSlots", true);

		return true;
	}
} // namespace Papyrus::AnimationSlots
