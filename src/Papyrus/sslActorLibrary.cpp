#include "sslActorLibrary.h"

#include "Registry/Validation.h"

namespace Papyrus::ActorLibrary
{
	int32_t ValidateActorImpl(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::Actor* a_actor)
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

}	 // namespace Papyrus::ActorLibrary
