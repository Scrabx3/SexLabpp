#pragma once

namespace SexLab
{
	bool IsFuta(RE::Actor* a_actor);	 // Assumes female base sex
	bool IsNPC(const RE::Actor* a_actor);
	bool IsVampire(const RE::Actor* a_actor);

	bool IsBed(const RE::TESObjectREFR* a_reference);

	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor);
	void SetVehicle(RE::Actor* a_this, RE::TESObjectREFR* a_vehicle);

	template <class T>
	constexpr void ToLower(T& str)
	{
		std::transform(str.cbegin(), str.cend(), str.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
	}

	template <class T>
	bool IsEqualString(T lhs, T rhs)
	{
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
			[](char lhs, char rhs) { return tolower(lhs) == tolower(rhs); });
	}
}