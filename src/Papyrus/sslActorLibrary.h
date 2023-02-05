#pragma once

namespace Papyrus
{
#define REGISTERFUNC(func, d) a_vm->RegisterFunction(#func##sv, "sslActorLibrary", func, d)

	bool CanAnimate(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::Actor* a_actor);
  bool CanAnimateActor(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);

	inline bool Register(VM* a_vm)
	{
    REGISTERFUNC(CanAnimate, true);
    REGISTERFUNC(CanAnimateActor, true);

		return true;
  }
} // namespace Papyrus
