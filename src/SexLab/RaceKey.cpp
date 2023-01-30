#include "SexLab/RaceKey.h"

namespace SexLab::RaceKey
{
	std::pair<int, ExtraRace> GetRaceID(std::string a_racekey)
	{
		ToLower(a_racekey);
		ExtraRace extra;
		if (a_racekey == "wolves") {
			a_racekey = "canines";
			extra = ExtraRace::Wolf;
		} else if (a_racekey == "dogs") {
			a_racekey = "canines";
			extra = ExtraRace::Dog;
		} else {
			extra = ExtraRace::None;
		}
		const auto w = std::find(RaceKeys.begin(), RaceKeys.end(), a_racekey);
		if (w == RaceKeys.end())
			return { 0, ExtraRace::None };

		return { static_cast<int32_t>(w - RaceKeys.begin()), extra };
	}

	std::string GetRaceKey(std::pair<int, ExtraRace> a_raceid)
	{
		switch (a_raceid.second) {
		case ExtraRace::Dog:
			return "dogs"s;
		case ExtraRace::Wolf:
			return "wolves"s;
		default:
			return GetRaceKey(a_raceid.first);
		}
	}

	std::string GetRaceKey(const uint32_t a_rawraceid)
	{
		if (a_rawraceid < RaceKeys.size())
			return RaceKeys[a_rawraceid];

		return ""s;
	}

}	 // namespace SexLab::RaceKey
