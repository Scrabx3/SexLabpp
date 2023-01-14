#pragma once

namespace SexLab::RaceKey
{
  // TODO: Move this into its own file with its own logic and serialize and idk
	// Need Ashals dll sources for this first, so for the time being its gonna be hardcoded here
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

	// Make sure the key is lowercase & translate legacy racekeys
	void ValidateRaceKey(std::string& a_racekey);

	int32_t GetRaceID(const std::string& a_racekey);
	std::string GetRaceKey(const uint32_t a_raceid);

} // namespace SexLab::RaceKey
