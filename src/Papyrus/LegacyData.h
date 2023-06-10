#pragma once

#include "Registry/Animation.h"

namespace Papyrus
{
  // Information of pre V2 definitions used in some papyrus functions

  enum LegacySex
  {
    Male = 0,
    Female,
    CrtMale,
    CrtFemale,

    None = -1,
  };

  LegacySex GetLegacySex(RE::Actor* a_actor)
	{
		auto creature = !Registry::IsNPC(a_actor);
		auto sex = Registry::GetSex(a_actor);
		switch (sex) {
		case Registry::Sex::None:
		case Registry::Sex::Male:
      return creature ? CrtMale : Male;
		case Registry::Sex::Female:
		case Registry::Sex::Futa:
			return creature ? (Settings::bCreatureGender ? CrtFemale : CrtMale) : Female;
		}
	}

	std::array<uint32_t, 4> GetLegacySex(std::vector<RE::Actor*> a_positions)
	{
		std::array<uint32_t, 4> ret{ 0, 0, 0, 0 };
		for (auto&& actor : a_positions) {
			auto sex = GetLegacySex(actor);
      ret[sex]++;
		}
    return ret;
	}

} // namespace Papyrus
