#include "sslActorLibrary.h"

#include "Registry/Validation.h"
#include "UserData/StripData.h"

namespace Papyrus::ActorLibrary
{
	int32_t ValidateActorImpl(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
		{
			a_vm->TraceStack("Cannot validate a none reference", a_stackID);
			return -1;
		}
		const auto code = Registry::IsValidActorImpl(a_actor);
		// Gotta do some number tweaking to stay consistent on the papyrus sade
		return code == 0 ? -2 : code;
	}

	void WriteStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form, bool a_neverstrip)
	{
		if (!a_form) {
			a_vm->TraceStack("Cannot write strip settings for a none reference", a_stackID);
			return;
		}
		const auto strip = a_neverstrip ? UserData::Strip::NoStrip : UserData::Strip::Always;
		UserData::StripData::GetSingleton()->AddArmor(a_form, strip);
	}

	void EraseStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		if (!a_form) {
			a_vm->TraceStack("Cannot erase strip setting of a none reference", a_stackID);
			return;
		}
		UserData::StripData::GetSingleton()->RemoveArmor(a_form);
	}

	void EraseStripAll(RE::StaticFunctionTag*)
	{
		UserData::StripData::GetSingleton()->RemoveArmorAll();
	}

	int32_t CheckStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		if (!a_form) {
			a_vm->TraceStack("Cannot check strip settings for none reference", a_stackID);
			return 0;
		}
		switch (UserData::StripData::GetSingleton()->CheckStrip(a_form)) {
		case UserData::Strip::NoStrip:
			return -1;
		case UserData::Strip::Always:
			return 1;
		default:
			return 0;
		}
	}

}	 // namespace Papyrus::ActorLibrary
