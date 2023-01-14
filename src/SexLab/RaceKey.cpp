#include "SexLab/RaceKey.h"

namespace SexLab::RaceKey
{
	void ValidateRaceKey(std::string& a_racekey)
  {
    ToLower(a_racekey);
    if (a_racekey == "wolves" || a_racekey == "dogs")
      a_racekey = "canines";
  }

	int32_t GetRaceID(const std::string& a_racekey)
	{
		const auto w = std::find(RaceKeys.begin(), RaceKeys.end(), a_racekey);
    if (w == RaceKeys.end())
      return 0;

    return static_cast<int32_t>(w - RaceKeys.begin());
	}

	std::string GetRaceKey(const uint32_t a_rawraceid)
	{
		if (a_rawraceid < RaceKeys.size())
			return RaceKeys[a_rawraceid];

		return ""s;
	}
} // namespace SexLab::RaceKey
