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

	// const RE::TESQuest* Config;
	// Script::ObjectPtr GetConfig() { return Script::CreateObjectPtr(Config, "sslSystemConfig"); }


	inline bool LoadData()
	{
		// LOOKUPMOD(Config, 0xD62, "SexLab.esp");
		LOOKUPMOD(GenderFaction, 0x043A43, "SexLab.esm");

		return true;
	}
} // namespace SLPP
