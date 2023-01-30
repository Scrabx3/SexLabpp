#pragma once

namespace SexLab::RaceKey
{
	enum class ExtraRace
	{
		Wolf,	// base = Canine
		Dog,	// base = Canine

		None
	};

	// COMEBACK: This here can prbly be done more elgantly (and dynamically) if I had Ashals dll sources
	static inline std::vector<std::string> RaceKeys{
		"human"s,
		"ashhoppers"s,
		"bears"s,
		"boars"s,
		"boarsany"s,
		"boarsmounted"s,
		"canines"s,
		"chaurus"s,
		"chaurushunters"s,
		"chaurusreapers"s,
		"chickens"s,
		"cows"s,
		"deers"s,
		"dogs"s,
		"dragonpriests"s,
		"dragons"s,
		"draugrs"s,
		"dwarvenballistas"s,
		"dwarvencenturions"s,
		"dwarvenspheres"s,
		"dwarvenspiders"s,
		"falmers"s,
		"flameatronach"s,
		"foxes"s,
		"frostatronach"s,
		"gargoyles"s,
		"giants"s,
		"goats"s,
		"hagravens"s,
		"horkers"s,
		"horses"s,
		"icewraiths"s,
		"lurkers"s,
		"mammoths"s,
		"mudcrabs"s,
		"netches"s,
		"rabbits"s,
		"rieklings"s,
		"sabrecats"s,
		"seekers"s,
		"skeevers"s,
		"slaughterfishes"s,
		"stormatronach"s,
		"spiders"s,
		"largespiders"s,
		"giantspiders"s,
		"spriggans"s,
		"trolls"s,
		"vampirelords"s,
		"werewolves"s,
		"wispmothers"s,
		"wisps"s,
		"wolves"s
	};

	std::pair<int, ExtraRace> GetRaceID(std::string& a_racekey);

	std::string GetRaceKey(const std::pair<int, ExtraRace> a_raceid);
	std::string GetRaceKey(const uint32_t a_raceid);

} // namespace SexLab::RaceKey
