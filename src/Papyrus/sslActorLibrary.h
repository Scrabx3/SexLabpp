#pragma once

namespace Papyrus::ActorLibrary
{
#define REGISTERFUNC(func, dont_delay) a_vm->RegisterFunction(#func##sv, "sslActorLibrary", func, dont_delay)

	int32_t ValidateActor(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::Actor* a_actor);

	inline bool Register(VM* a_vm)
	{
    REGISTERFUNC(ValidateActor, true);

		return true;
  }
} // namespace Papyrus::ActorLibrary
