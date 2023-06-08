#pragma once

namespace GameForms
{
#define LOOKUPMOD(form, formid, mod)                                                                             \
	form = RE::TESDataHandler::GetSingleton()->LookupForm<std::remove_pointer<decltype(form)>::type>(formid, mod); \
	if (!form)                                                                                                     \
		return false;

#define LOOKUPGAME(form, formid)                                                     \
	form = RE::TESForm::LookupByID<std::remove_pointer<decltype(form)>::type>(formid); \
	if (!form)                                                                         \
		return false;

	// Vanilla Forms
	inline RE::BGSKeyword* Vampire;
	inline RE::BGSKeyword* DLC2RieklingMountedKeyword;

	// SexLab Forms
	inline const RE::TESFaction* GenderFaction;
	inline const RE::TESFaction* AnimatingFaction;

	inline const RE::TESQuest* ConfigQuest;

	inline bool LoadData()
	{
		LOOKUPGAME(Vampire, 0xA82BB);
		LOOKUPGAME(DLC2RieklingMountedKeyword, 0x0403A159);

		LOOKUPMOD(GenderFaction, 0x043A43, "SexLab.esm");
		LOOKUPMOD(AnimatingFaction, 0xE50F, "SexLab.esm");

		LOOKUPMOD(ConfigQuest, 0xD62, "SexLab.esm");

		return true;
	}
} // namespace SLPP
