#pragma once

namespace Papyrus::ActorLibrary
{
	int32_t ValidateActorImpl(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);

	void WriteStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form, bool a_neverstrip);
	void EraseStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form);
	void EraseStripAll(RE::StaticFunctionTag*);
	int32_t CheckStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(ValidateActorImpl, "sslActorLibrary", true);

		return true;
  }
} // namespace Papyrus::ActorLibrary
