#include "RaceKey.h"

namespace Registry
{
	static inline std::vector<std::pair<std::string_view, RaceKey>> race_map{
		{ "Human", { RaceType::Human, ExtraRace::None } },
		{ "Ash Hopper", { RaceType::AshHopper, ExtraRace::None } },
		{ "Bear", { RaceType::Bear, ExtraRace::None } },
		{ "Boar", { RaceType::Boar, ExtraRace::BoarSingle } },
		{ "Boar (Any)", { RaceType::Boar, ExtraRace::None } },
		{ "Boar (Mounted)", { RaceType::Boar, ExtraRace::BoarMounted } },
		{ "Canine", { RaceType::Canine, ExtraRace::None } },
		{ "Chaurus", { RaceType::Chaurus, ExtraRace::None } },
		{ "Chaurus Hunter", { RaceType::ChaurusHunter, ExtraRace::None } },
		{ "Chaurus Reaper", { RaceType::ChaurusReaper, ExtraRace::None } },
		{ "Chicken", { RaceType::Chicken, ExtraRace::None } },
		{ "Cow", { RaceType::Cow, ExtraRace::None } },
		{ "Deer", { RaceType::Deer, ExtraRace::None } },
		{ "Dog", { RaceType::Canine, ExtraRace::Dog } },
		{ "Dragon Priest", { RaceType::DragonPriest, ExtraRace::None } },
		{ "Dragon", { RaceType::Dragon, ExtraRace::None } },
		{ "Draugr", { RaceType::Draugr, ExtraRace::None } },
		{ "Dwarven Ballista", { RaceType::DwarvenBallista, ExtraRace::None } },
		{ "Dwarven Centurion", { RaceType::DwarvenCenturion, ExtraRace::None } },
		{ "Dwarven Sphere", { RaceType::DwarvenSphere, ExtraRace::None } },
		{ "Dwarven Spider", { RaceType::DwarvenSpider, ExtraRace::None } },
		{ "Falmer", { RaceType::Falmer, ExtraRace::None } },
		{ "Flame Atronach", { RaceType::FlameAtronach, ExtraRace::None } },
		{ "Fox", { RaceType::Fox, ExtraRace::None } },
		{ "Frost Atronach", { RaceType::FrostAtronach, ExtraRace::None } },
		{ "Gargoyle", { RaceType::Gargoyle, ExtraRace::None } },
		{ "Giant", { RaceType::Giant, ExtraRace::None } },
		{ "Goat", { RaceType::Goat, ExtraRace::None } },
		{ "Hagraven", { RaceType::Hagraven, ExtraRace::None } },
		{ "Horker", { RaceType::Horker, ExtraRace::None } },
		{ "Horse", { RaceType::Horse, ExtraRace::None } },
		{ "Ice Wraith", { RaceType::IceWraith, ExtraRace::None } },
		{ "Lurker", { RaceType::Lurker, ExtraRace::None } },
		{ "Mammoth", { RaceType::Mammoth, ExtraRace::None } },
		{ "Mudcrab", { RaceType::Mudcrab, ExtraRace::None } },
		{ "Netch", { RaceType::Netch, ExtraRace::None } },
		{ "Rabbit", { RaceType::Rabbit, ExtraRace::None } },
		{ "Riekling", { RaceType::Riekling, ExtraRace::None } },
		{ "Sabrecat", { RaceType::Sabrecat, ExtraRace::None } },
		{ "Seeker", { RaceType::Seeker, ExtraRace::None } },
		{ "Skeever", { RaceType::Skeever, ExtraRace::None } },
		{ "Slaughterfish", { RaceType::Slaughterfish, ExtraRace::None } },
		{ "Storm Atronach", { RaceType::StormAtronach, ExtraRace::None } },
		{ "Spider", { RaceType::Spider, ExtraRace::None } },
		{ "Large Spider", { RaceType::LargeSpider, ExtraRace::None } },
		{ "Giant Spider", { RaceType::GiantSpider, ExtraRace::None } },
		{ "Spriggan", { RaceType::Spriggan, ExtraRace::None } },
		{ "Troll", { RaceType::Troll, ExtraRace::None } },
		{ "Vampire Lord", { RaceType::VampireLord, ExtraRace::None } },
		{ "Werewolf", { RaceType::Werewolf, ExtraRace::None } },
		{ "Wispmother", { RaceType::Wispmother, ExtraRace::None } },
		{ "Wisp", { RaceType::Wisp, ExtraRace::None } },
		{ "Wolf", { RaceType::Canine, ExtraRace::Wolf } }
	};

	// std::optional<RaceKey> MapStringToRaceKey(const std::string_view a_string)
	// {
	// 	for (auto&& [str, key] : race_map) {
	// 		if (SexLab::IsEqualString(str, a_string)) {
	// 			return key;
	// 		}
	// 	}
	// 	std::nullopt;
	// }

	// std::optional<std::string_view> MapRaceKeyToString(const RaceKey a_key)
	// {
	// 	for (auto&& [str, key] : race_map) {
	// 		if (key == a_key) {
	// 			return str;
	// 		}
	// 	}
	// 	std::nullopt;
	// }


	// std::pair<int, ExtraRace> GetRaceID(std::string a_racekey)
	// {
	// 	SexLab::ToLower(a_racekey);
	// 	ExtraRace extra;
	// 	if (a_racekey == "wolves") {
	// 		a_racekey = "canines";
	// 		extra = ExtraRace::Wolf;
	// 	} else if (a_racekey == "dogs") {
	// 		a_racekey = "canines";
	// 		extra = ExtraRace::Dog;
	// 	} else {
	// 		extra = ExtraRace::None;
	// 	}
	// 	const auto w = std::find(RaceKeys.begin(), RaceKeys.end(), a_racekey);
	// 	if (w == RaceKeys.end())
	// 		return { 0, ExtraRace::None };

	// 	return { static_cast<int32_t>(w - RaceKeys.begin()), extra };
	// }

	// std::string GetRaceKey(std::pair<int, ExtraRace> a_raceid)
	// {
	// 	switch (a_raceid.second) {
	// 	case ExtraRace::Dog:
	// 		return "dogs"s;
	// 	case ExtraRace::Wolf:
	// 		return "wolves"s;
	// 	default:
	// 		return GetRaceKey(a_raceid.first);
	// 	}
	// }

	// std::string GetRaceKey(const uint32_t a_rawraceid)
	// {
	// 	if (a_rawraceid < RaceKeys.size())
	// 		return RaceKeys[a_rawraceid];

	// 	return ""s;
	// }

}	 // namespace SexLab::RaceKey
