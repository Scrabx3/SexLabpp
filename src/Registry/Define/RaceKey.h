#pragma once

namespace Registry
{
	/* 
		Canine can animate both dog and wolf but dog can only animte dog/wolf only wolf
		BoarAny can animate both BoarSingle and BoarMounted but BoarMounted can only animateBoarMounted/BoarSingle only BoarSingle
	*/

	enum class RaceKey : uint8_t
	{
		Human = 0,

		AshHopper,
		Bear,
		Boar,
		BoarMounted,
		BoarSingle,
		Canine,
		Chaurus,
		ChaurusHunter,
		ChaurusReaper,
		Chicken,
		Cow,
		Deer,
		Dog,
		Dragon,
		DragonPriest,
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
		GiantSpider,
		Goat,
		Hagraven,
		Hare,
		Horker,
		Horse,
		IceWraith,
		LargeSpider,
		Lurker,
		Mammoth,
		Mudcrab,
		Netch,
		Riekling,
		Sabrecat,
		Seeker,
		Skeever,
		Slaughterfish,
		Spider,
		Spriggan,
		StormAtronach,
		Troll,
		VampireLord,
		Werewolf,
		Wisp,
		Wispmother,
		Wolf,

		None = static_cast<std::underlying_type_t<RaceKey>>(-1),
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

	struct RaceHandler
	{
		static RaceKey GetRaceKey(RE::Actor* a_actor);
	};

	// std::optional<RaceKey> MapStringToRaceKey(const std::string_view a_string);
	// std::optional<std::string_view> MapRaceKeyToString(const RaceKey a_key);

	// std::pair<int, RaceExtra> GetRaceID(std::string a_racekey);
	// std::string GetRaceKey(const std::pair<int, RaceExtra> a_raceid);
	// std::string GetRaceKey(const uint32_t a_raceid);

}	 // namespace Registry
