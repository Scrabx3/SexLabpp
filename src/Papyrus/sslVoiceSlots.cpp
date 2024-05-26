#include "sslVoiceSlots.h"

#include "Registry/Voice.h"

namespace Papyrus::VoiceSlots
{
	RE::BSFixedString SelectVoice(RE::StaticFunctionTag*, RE::Actor* a_actor)
  {
    return SelectVoiceByTags(nullptr, a_actor, "");
  }

	RE::BSFixedString SelectVoiceByTags(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_tags)
	{
		return SelectVoiceByTagsA(nullptr, a_actor, { a_tags.data() });
	}

	RE::BSFixedString SelectVoiceByTagsA(RE::StaticFunctionTag*, RE::Actor* a_actor, std::vector<std::string_view> a_tags)
	{
		auto s = Registry::Voice::GetSingleton();
		auto v = a_actor ? s->GetVoice(a_actor, { a_tags }) : s->GetVoice({ a_tags });
		return v ? v->name : "";
	}

	RE::BSFixedString GetSavedVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
  {
    if (!a_actor) {
      a_vm->TraceStack("Actor is none", a_stackID);
      return "";
    }
    auto v = Registry::Voice::GetSingleton()->GetSavedVoice(a_actor->GetFormID());
    return v ? v->name : "";
  }

	void StoreVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_voice)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Voice::GetSingleton()->SaveVoice(a_actor->GetFormID(), a_voice);
	}

	void DeleteVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Voice::GetSingleton()->ClearVoice(a_actor->GetFormID());
	}

	std::vector<RE::BSFixedString> GetAllVoices(RE::StaticFunctionTag*)
  {
    return Registry::Voice::GetSingleton()->GetAllVoiceNames();
  }

	RE::BSFixedString SelectVoiceByRace(RE::StaticFunctionTag*, RE::BSFixedString a_racekey)
	{
		auto v = Registry::Voice::GetSingleton()->GetVoice(Registry::RaceHandler::GetRaceKey(a_racekey));
    return v ? v->name : "";
	}

}	 // namespace Papyrus::VoiceSlots
