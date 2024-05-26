#pragma once

namespace Papyrus::VoiceSlots
{
	RE::BSFixedString SelectVoice(RE::StaticFunctionTag*, RE::Actor* a_actor);
	RE::BSFixedString SelectVoiceByTags(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_tags);
	RE::BSFixedString SelectVoiceByTagsA(RE::StaticFunctionTag*, RE::Actor* a_actor, std::vector<std::string_view> a_tags);

	RE::BSFixedString GetSavedVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	void StoreVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_voice);
	void DeleteVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);

	std::vector<RE::BSFixedString> GetAllVoices(RE::StaticFunctionTag*);
	RE::BSFixedString SelectVoiceByRace(RE::StaticFunctionTag*, RE::BSFixedString a_racekey);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(SelectVoice, "sslVoiceSlots", true);
		REGISTERFUNC(SelectVoiceByTags, "sslVoiceSlots", true);
		REGISTERFUNC(SelectVoiceByTagsA, "sslVoiceSlots", true);

		REGISTERFUNC(GetSavedVoice, "sslVoiceSlots", true);
		REGISTERFUNC(StoreVoice, "sslVoiceSlots", true);
		REGISTERFUNC(DeleteVoice, "sslVoiceSlots", true);

		REGISTERFUNC(GetAllVoices, "sslVoiceSlots", true);
		REGISTERFUNC(SelectVoiceByRace, "sslVoiceSlots", true);

		return true;
	}

}	 // namespace Papyrus::VoiceSlots
