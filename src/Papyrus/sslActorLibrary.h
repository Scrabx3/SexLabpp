#pragma once

namespace Papyrus::ActorLibrary
{
	int32_t ValidateActor(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::Actor* a_actor);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(ValidateActor, "sslActorLibrary", true);

		return true;
  }
} // namespace Papyrus::ActorLibrary
