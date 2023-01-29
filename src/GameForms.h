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

	// SexLab Forms
	inline const RE::TESFaction* GenderFaction;
	inline const RE::TESQuest* ConfigQuest;

	inline bool LoadData()
	{
		LOOKUPMOD(GenderFaction, 0x043A43, "SexLab.esm");
		LOOKUPMOD(ConfigQuest, 0xD62, "SexLab.esm");

		return true;
	}
} // namespace SLPP
