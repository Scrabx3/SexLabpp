#pragma once

#include "RaceKey.h"

namespace Registry
{
	static inline constexpr size_t MAX_ACTOR_COUNT = 5;

	enum class HeaderFragment
	{
		None = 0,

		AllowBed = 1ULL << 0,
	};
	static inline constexpr size_t HeaderFragmentSize = 1;

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
		// HandShackle = (Yoke | Arminder),
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

	using HeaderUnderlying = std::underlying_type<HeaderFragment>::type;
	using FragmentUnderlying = std::underlying_type<PositionFragment>::type;
	using FragmentHash = std::bitset<MAX_ACTOR_COUNT * PositionFragmentSize + HeaderFragmentSize>;

	RaceKey FragmentAsRaceKey(PositionFragment a_fragment);
	PositionFragment RaceKeyAsFragment(RaceKey a_racekey);

	PositionFragment MakeFragmentFromActor(RE::Actor* a_actor, bool a_submissive);
	FragmentHash CombineFragments(const std::vector<PositionFragment>& a_fragments, HeaderFragment a_header);

} // namespace Registry
