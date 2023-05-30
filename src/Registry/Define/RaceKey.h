#pragma once

namespace Registry
{
	enum class RaceType : uint8_t;
	enum class ExtraRace : uint8_t;
	using RaceKey = std::pair<RaceType, ExtraRace>;

	enum class RaceType : uint8_t
	{
		Human = 0,
		AshHopper,
		Bear,
		Boar,
		Canine,
		Chaurus,
		ChaurusHunter,
		ChaurusReaper,
		Chicken,
		Cow,
		Deer,
		DragonPriest,
		Dragon,
		Draugr,
		DwarvenBallista,
		DwarvenCenturion,
		DwarvenSphere,
		DwarvenSpider,
		Falmer,
		FlameAtronach,
		Fox,
		FrostAtronach,
		Gargoyle,
		Giant,
		Goat,
		Hagraven,
		Horker,
		Horse,
		IceWraith,
		Lurker,
		Mammoth,
		Mudcrab,
		Netch,
		Rabbit,
		Riekling,
		Sabrecat,
		Seeker,
		Skeever,
		Slaughterfish,
		StormAtronach,
		Spider,
		LargeSpider,
		GiantSpider,
		Spriggan,
		Troll,
		VampireLord,
		Werewolf,
		Wispmother,
		Wisp,
	};

	enum class ExtraRace : uint8_t
	{
		None,
		// base = canine
		Wolf,
		Dog,
		// base = boar
		BoarSingle,
		BoarMounted,
	};


	static inline std::vector<std::string> LegacyRaceKeys{
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

	// std::optional<RaceKey> MapStringToRaceKey(const std::string_view a_string);
	// std::optional<std::string_view> MapRaceKeyToString(const RaceKey a_key);

	// std::pair<int, ExtraRace> GetRaceID(std::string a_racekey);
	// std::string GetRaceKey(const std::pair<int, ExtraRace> a_raceid);
	// std::string GetRaceKey(const uint32_t a_raceid);

}	 // namespace Registry
