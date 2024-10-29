#pragma once

struct StringCmp
{
	bool operator()(const std::string& a_lhs, const std::string& a_rhs) const { return _strcmpi(a_lhs.c_str(), a_rhs.c_str()) < 0; }
};

struct Settings
{
	static void Initialize();			 // Pre LoadData
	static void InitializeData();	 // Post LoadData
	static void Save();

	// --- MCM
#define MCM_SETTING(STR, DEFAULT) static inline decltype(DEFAULT) STR{ DEFAULT };
#include "mcm.def"
#undef MCM_SETTING

	using SettingsVariants = std::variant<float*, std::string*, bool*, int*, std::vector<float>*, std::vector<int>*>;
	static inline std::map<std::string, SettingsVariants, StringCmp> table{
#define MCM_SETTING(STR, DEFAULT) { #STR##s, &STR },
#include "mcm.def"
#undef MCM_SETTING
	};

	// --- INI
#define INI_SETTING(STR, DEFAULT, CAT) static inline decltype(DEFAULT) STR{ DEFAULT };
#include "config.def"
#undef INI_SETTING

	// --- Misc
	static inline std::vector<RE::FormID> SOS_ExcludeFactions{};

private:
	static void InitializeYAML();
	static void InitializeINI();
};
