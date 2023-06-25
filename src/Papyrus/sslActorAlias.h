#pragma once

namespace Papyrus::sslActorAlias
{
	void LockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias);
	void UnlockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(LockActorImpl, "sslActorLibrary", false);
		REGISTERFUNC(UnlockActorImpl, "sslActorLibrary", false);

		return true;
	}
}	 // namespace Papyrus::sslActorAlias
