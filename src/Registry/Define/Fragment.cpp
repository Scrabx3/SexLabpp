#include "Fragment.h"

namespace Registry
{
	PositionFragment MakeFragmentFromActor(RE::Actor* a_actor, bool a_submissive)
	{
		auto base = a_actor->GetActorBase();
		if (!base) {
			logger::error("Invalid Actor {:X} (0): Missing base object", a_actor->formID);
			return { PositionFragment::None };
		}

		auto sex = base ? base->GetSex() : RE::SEXES::kNone;
		stl::enumeration<PositionFragment> ret{};
		switch (sex) {
		case RE::SEXES::kFemale:
			ret.set(Registry::IsFuta(a_actor) ? PositionFragment::Futa : PositionFragment::Female);
			break;
		case RE::SEXES::kMale:
			ret.set(PositionFragment::Male);
			break;
		default:
			logger::error("Invalid Actor {:X} ({:X}): Missing sex", a_actor->formID, (base ? base->formID : 0));
			return { PositionFragment::None };
		}

		const auto racekey = RaceHandler::GetRaceKey(a_actor);
		switch (racekey) {
		case RaceKey::None:
			logger::error("Invalid Actor {:X} ({:X}): Missing Racekey", a_actor->formID, base->formID);
			break;
		case RaceKey::Human:
			{
				ret.set(PositionFragment::Human);
				if (a_actor->HasKeyword(GameForms::Vampire)) {
					ret.set(PositionFragment::Vampire);
				}
				// COMEBACK: bound extra
			}
			break;
		default:
			{
				const auto val = RaceKeyAsFragment(racekey);
				ret.set(val);
			}
			break;
		}

		if (a_actor->IsDead() || a_actor->IsUnconscious()) {
			ret.set(PositionFragment::Unconscious);
		} else if (a_submissive) {
			ret.set(PositionFragment::Submissive);
		}

		return ret.get();
	}

	FragmentHash CombineFragments(const std::vector<PositionFragment>& a_fragments)
  {
		assert(a_fragments.size() <= MAX_ACTOR_COUNT);
		FragmentHash ret{};
		ret |= static_cast<std::underlying_type<PositionFragment>::type>(a_fragments[0]);
		size_t i = 1;
		for (; i < a_fragments.size(); i++) {
			ret <<= PositionFragmentSize;
			ret |= static_cast<std::underlying_type<PositionFragment>::type>(a_fragments[i]);
		}
		for (size_t n = i; n < 5; n++) {
			ret <<= PositionFragmentSize;
		}
		// ret <<= HeaderFragmentSize;
		// ret |= static_cast<std::underlying_type<HeaderFragment>::type>(a_header);
		return ret;
  }

	RaceKey FragmentAsRaceKey(PositionFragment a_fragment)
	{
		using underlying = std::underlying_type<PositionFragment>::type;
		const auto fragment = static_cast<underlying>(a_fragment);
		if (fragment & static_cast<underlying>(PositionFragment::Human)) {
			return RaceKey::Human;
		}
		return RaceKey((fragment >> 3) & 0b1111'11);
	}

  PositionFragment RaceKeyAsFragment(RaceKey a_racekey)
  {
		using underlying = std::underlying_type<PositionFragment>::type;
		if (a_racekey == RaceKey::Human) {
			return PositionFragment::Human;
		}

		return PositionFragment(static_cast<underlying>(a_racekey) << 3);
	}

} // namespace Registry
