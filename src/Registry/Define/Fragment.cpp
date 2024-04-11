#include "Fragment.h"

#include "Registry/Define/Sex.h"

namespace Registry
{
	PositionFragment MakeFragmentFromActor(RE::Actor* a_actor, bool a_submissive)
	{
		stl::enumeration<PositionFragment> ret{};
		switch (GetSex(a_actor)) {
		case Sex::Female:
			ret.set(PositionFragment::Female);
			break;
		case Sex::Male:
			ret.set(PositionFragment::Male);
			break;
		case Sex::Futa:
			ret.set(PositionFragment::Futa);
			break;
		default:
			logger::error("Cannt build fragment from Actor {:X}: Invalid Sex", a_actor->formID);
			return { PositionFragment::None };
		}
		const auto racekey = RaceHandler::GetRaceKey(a_actor);
		switch (racekey) {
		case RaceKey::None:
			logger::error("Cannt build fragment from Actor {:X}: Invalid RaceKey", a_actor->formID);
			break;
		case RaceKey::Human:
			{
				ret.set(PositionFragment::Human);
				if (a_actor->HasKeyword(GameForms::Vampire)) {
					ret.set(PositionFragment::Vampire);
				}
			}
			break;
		default:
			{
				const auto val = RaceKeyAsFragment(racekey);
				ret.set(val);
			}
			break;
		}
		if (a_actor->IsDead() || a_actor->IsUnconscious() || a_actor->GetActorValue(RE::ActorValue::kVariable05) < 0) {
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

	std::vector<std::pair<RE::Actor*, Registry::PositionFragment>> MakeFragmentPair(const std::vector<RE::Actor*>& a_actors, const std::vector<RE::Actor*>& a_submissives)
	{
		std::vector<std::pair<RE::Actor*, Registry::PositionFragment>> ret{};
		for (auto&& actor : a_actors) {
			const auto submissive = std::ranges::contains(a_submissives, actor);
			ret.emplace_back(actor, MakeFragmentFromActor(actor, submissive));
		}
		return ret;
	}

} // namespace Registry
