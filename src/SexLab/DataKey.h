#pragma once


namespace SexLab::DataKey
{
	/// Semi Mandatory: 'at least' cover the compared key. E.g. vampire animation only on vampire but vampire can animate everything
	/// Mandatory: Exact match. E.g. necro animation only for dead actors AND dead actors CANNOT animate "for-living" animations
	enum Key : uint32_t
	{
		// Gender (Semi Mandatory)
		Male = 1 << 0,
		Female = 1 << 1,
		Futa = 1 << 2,
		Creature = 1 << 3,

		Overwrite_Male = 1 << 4,
		Overwrite_Female = 1 << 5,

		GendersBaseTotal = (Male | Female | Futa),
		GendersTotal = (Male | Female | Futa | Creature),
		GenderBits = 6,

		// Race (Mandatory)
		Crt0 = 1U << 6,		//	0 ~ 1
		Crt1 = 1U << 7,		//	2 ~ 3
		Crt2 = 1U << 8,		//	4 ~ 7
		Crt3 = 1U << 9,		//	8 ~ 15
		Crt4 = 1U << 10,	//	15 ~ 31
		Crt5 = 1U << 11,	//	32 ~ 63

		CrtTotal = (Crt0 | Crt1 | Crt2 | Crt3 | Crt4 | Crt5),
		CrtBits = 6,

		// Extra (Semi Mandatory)
		Victim = 1U << 12,
		Vampire = 1U << 13,

		AmputeeAR = 1U << 14,	 // Arm Right
		AmputeeAL = 1U << 15,	 // Arm Left
		AmputeeLR = 1U << 16,	 // leg Right
		AmputeeLL = 1U << 17,	 // Leg Left

		Dog = 1U << 14,		// Canines only
		Wolf = 1U << 15,	// Canines only
		// Extra (Mandatory)
		Dead = 1U << 18,

		ExtraMandatory = (Dead),
		ExtraBits = 7,

		MandatoryKeys = (CrtTotal | ExtraMandatory),

		// Blank
		Empty = 0,
		Blank = 1U << 31
	};

	void AddGender(const RE::Actor* a_actor, SKSE::stl::enumeration<Key, std::uint32_t>& a_key);
	// void AddRaceID(const RE::Actor* a_actor, stl::enumeration<Gender, std::uint32_t>& a_key);
	void AddRaceID(std::string& a_racekey, stl::enumeration<Key, std::uint32_t>& a_key);

	uint32_t BuildKey(RE::Actor* a_ref, bool a_victim, std::string& a_racekey);
	uint32_t BuildCustomKey(uint32_t a_gender, std::string a_racekey);

	bool IsLess(uint32_t a_key, uint32_t a_cmp);
	bool MatchKey(uint32_t a_key, uint32_t a_match);

}	 // namespace SexLab::DataKey
