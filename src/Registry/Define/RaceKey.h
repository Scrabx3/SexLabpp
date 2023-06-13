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

	struct RaceHandler
	{
		static RaceKey GetRaceKey(RE::Actor* a_actor);
		static bool HasRaceKey(RE::Actor* a_actor, const RE::BSFixedString& a_racekey);
		static bool HasRaceKey(RE::Actor* a_actor, RaceKey a_racekey);
	};

}	 // namespace Registry
