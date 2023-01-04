#pragma once


namespace SexLab::DataKey
{
	enum Key : uint32_t
	{
		// Gender
		Male = 1U << 0,
		Female = 1U << 1,
		Futa = 1U << 2,
		Human = (Male | Female | Futa),

		Crt_Male = 1U << 3,
		Crt_Female = 1U << 4,
        Creature = (Crt_Male | Crt_Female),

		Overwrite_Male = 1U << 5,
		Overwrite_Female = 1U << 6,

		UNDEFINED = 1U << 7,

		// Race
		Crt0 = 1U << 8,
		Crt1 = 1U << 9,
		Crt2 = 1U << 10,
		Crt3 = 1U << 11,
		Crt4 = 1U << 12,
		Crt5 = 1U << 13,
		Crt6 = 1U << 14,
		Crt7 = 1U << 15,

		CrtTotal = (Crt0 | Crt1 | Crt2 | Crt3 | Crt4 | Crt5 | Crt6 | Crt7),

		// Extra
		Victim = 1U << 16,
		Vampire = 1U << 17,

		// Blank
		Empty = 0,
		Blank = 1U << 31
	};

	// TODO: uint32_t GetRaceID(RE::Actor*);

	uint32_t BuildKey(RE::Actor* a_ref, bool a_victim, uint32_t a_raceid);

    std::vector<uint32_t> SortKeys(const std::vector<uint32_t>& a_keys);

    bool IsLess(uint32_t a_key, uint32_t a_cmp);
    bool MatchKey(uint32_t a_key, uint32_t a_match);
    bool MatchArray(const std::vector<uint32_t>& a_key, const std::vector<uint32_t>& a_match);

}	 // namespace SexLab::DataKey
