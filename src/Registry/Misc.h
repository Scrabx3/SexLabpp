#pragma once

namespace Registry
{
	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor);

	template <class T, class U>
	bool StringCmpCI(T lhs, U rhs)
	{
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
			[](char lhs, char rhs) { return tolower(lhs) == tolower(rhs); });
	}
}
