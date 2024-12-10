#pragma once

namespace Registry
{
	bool IsNPC(const RE::Actor* a_actor);
	bool IsVampire(const RE::Actor* a_actor);
	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor);

	template <class T, class U>
	bool StringCmpCI(T lhs, U rhs)
	{
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
			[](char lhs, char rhs) { return tolower(lhs) == tolower(rhs); });
	}
	std::vector<std::string_view> StringSplit(const std::string_view a_view, const char a_delim = ',');
}
