#pragma once


namespace SexLab::DataKey
{
	enum Key : uint32_t
	{
		// Gender
		Male = SexLab::Gender::Male,
		Female = SexLab::Gender::Female,
		Futa = SexLab::Gender::Futa,
		Human = (Male | Female | Futa),

		Crt_Male = SexLab::Gender::Crt_Male,
		Crt_Female = SexLab::Gender::Crt_Female,
		Creature = (Crt_Male | Crt_Female),

		Overwrite_Male = SexLab::Gender::Overwrite_Male,
		Overwrite_Female = SexLab::Gender::Overwrite_Female,

		UNDEFINED = SexLab::Gender::UNDEFINED,

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

		ExtraDataCount = 2,

		// Blank
		Empty = 0,
		Blank = 1U << 31
	};

	// TODO: uint32_t GetRaceID(RE::Actor*);

	uint32_t BuildKey(RE::Actor* a_ref, bool a_victim, uint32_t a_raceid);
	uint32_t BuildCustomKey(uint32_t a_gender, uint32_t a_raceid, std::vector<bool> a_extradata);

	std::vector<uint32_t> SortKeys(const std::vector<uint32_t>& a_keys);

	bool IsLess(uint32_t a_key, uint32_t a_cmp);
	bool MatchKey(uint32_t a_key, uint32_t a_match);
	bool MatchArray(const std::vector<uint32_t>& a_key, const std::vector<uint32_t>& a_match);

}	 // namespace SexLab::DataKey
