#pragma once

#include "RaceKey.h"

namespace Registry
{
	static inline constexpr size_t MAX_ACTOR_COUNT = 5;

	enum class PositionFragment
	{
		None = 0,

		Male = 1 << 0,
		Female = 1 << 1,
		Futa = (Male | Female),

		Human = 1 << 2,
		Vampire = 1 << 3,
		Yoke = 1 << 4,
		Arminder = 1 << 5,
		HandShackle = (Yoke | Arminder),
		LegsBound = 1 << 6,
		// Unused = 1 << 7,
		// Unused = 1 << 8,

		CrtBit0 = 1 << 3,
		CrtBit1 = 1 << 4,
		CrtBit2 = 1 << 5,
		CrtBit3 = 1 << 6,
		CrtBit4 = 1 << 7,
		CrtBit5 = 1 << 8,

		Submissive = 1 << 9,
		Unconscious = 1 << 10,
	};
	static inline constexpr size_t PositionFragmentSize = 11;

	using FragmentHash = std::bitset<MAX_ACTOR_COUNT * PositionFragmentSize>;

	RaceKey FragmentAsRaceKey(PositionFragment a_fragment);
	PositionFragment RaceKeyAsFragment(RaceKey a_racekey);

	PositionFragment MakeFragmentFromActor(RE::Actor* a_actor, bool a_submissive);
	std::vector<std::pair<RE::Actor*, Registry::PositionFragment>> MakeFragmentPair(const std::vector<RE::Actor*>& a_actors, const std::vector<RE::Actor*>& a_submissives);
	FragmentHash CombineFragments(const std::vector<PositionFragment>& a_fragments);

} // namespace Registry
