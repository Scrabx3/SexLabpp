#include "Animation.h"

#include "Registry/Define/RaceKey.h"

namespace Registry
{
	bool PositionInfo::CanFillPosition(RE::Actor* a_actor) const
	{
		auto fragment = stl::enumeration(MakeFragmentFromActor(a_actor, false));
		if (CanFillPosition(fragment.get()))
			return true;
		
		fragment.set(PositionFragment::Submissive);
		return CanFillPosition(fragment.get());
	}

	bool PositionInfo::CanFillPosition(PositionFragment a_fragment) const
	{
		const auto fragment = stl::enumeration(a_fragment);
		if (fragment.all(PositionFragment::Futa) && !this->sex.all(Sex::Futa))
			return false;
		if (fragment.all(PositionFragment::Male) && !this->sex.all(Sex::Male))
			return false;
		if (fragment.all(PositionFragment::Female) && !this->sex.all(Sex::Female))
			return false;

		if (fragment.all(PositionFragment::Unconscious) != this->extra.all(Extra::Unconscious))
			return false;
		if (fragment.all(PositionFragment::Submissive) != this->extra.all(Extra::Submissive))
			return false;

		if (fragment.all(PositionFragment::Human)) {
			if (this->extra.all(Extra::Vamprie) && !fragment.all(PositionFragment::Vampire)) {
				return false;
			}
			// COMEBACK: Bindings check
		} else {
			const auto thisrace = static_cast<uint64_t>(race);
			if ((fragment.underlying() & thisrace) != thisrace)
				return false;
		}
		return true;
	}

	std::vector<PositionFragment> PositionInfo::MakeFragments() const
	{
		std::vector<stl::enumeration<PositionFragment>> fragments{};
		const auto addVariance = [&](PositionFragment a_variancebit) {
			const auto count = fragments.size();
			for (size_t i = 0; i < count; i++) {
				auto copy = fragments[i];
				copy.set(a_variancebit);
				fragments.push_back(copy);
			}
		};
		const auto setFragmentBit = [&](PositionFragment a_bit) {
			for (auto&& fragment : fragments) {
				fragment.set(a_bit);
			}
		};
		const auto addRaceVariance = [&](RaceKey a_racekey) {
			const auto val = RaceKeyAsFragment(a_racekey);
			const auto count = fragments.size();
			for (size_t i = 0; i < count; i++) {
				auto copy = fragments[i];
				copy.reset(PositionFragment::CrtBit0,
					PositionFragment::CrtBit1,
					PositionFragment::CrtBit2,
					PositionFragment::CrtBit3,
					PositionFragment::CrtBit4,
					PositionFragment::CrtBit5);
				copy.set(val);
				fragments.push_back(copy);
			}
		};
		const auto setRaceBit = [&](RaceKey a_racekey) {
			const auto val = RaceKeyAsFragment(a_racekey);
			setFragmentBit(val);
		};

		if (this->sex.all(Sex::Male))
			fragments.emplace_back(PositionFragment::Male);
		if (this->sex.all(Sex::Female))
			fragments.emplace_back(PositionFragment::Female);
		if (this->sex.all(Sex::Futa))
			fragments.emplace_back(PositionFragment::Futa);

		if (this->extra.all(Extra::Unconscious))
			setFragmentBit(PositionFragment::Unconscious);
		else if (this->extra.all(Extra::Submissive))
			setFragmentBit(PositionFragment::Submissive);

		switch (this->race) {
		case RaceKey::Human:
			{
				setFragmentBit(PositionFragment::Human);
				if (this->extra.all(Extra::Vamprie)) {
					setFragmentBit(PositionFragment::Vampire);
				} else {
					addVariance(PositionFragment::Vampire);
				}
				if (this->extra.all(Extra::Armbinder)) {
					setFragmentBit(PositionFragment::Arminder);
				} else if (this->extra.all(Extra::Yoke)) {
					setFragmentBit(PositionFragment::Yoke);
				}
				if (this->extra.all(Extra::Legbinder)) {
					setFragmentBit(PositionFragment::LegsBound);
				}
			}
			break;
		case RaceKey::Boar:
			setRaceBit(RaceKey::BoarMounted);
			addRaceVariance(RaceKey::BoarSingle);
			break;
		case RaceKey::Canine:
			setRaceBit(RaceKey::Dog);
			addRaceVariance(RaceKey::Wolf);
		default:
			setRaceBit(this->race);
			break;
		}
		if (this->extra.all(Extra::Optional)) {
			fragments.push_back(PositionFragment::None);
		}

		std::vector<PositionFragment> ret{};
		for (auto&& it : fragments)
			ret.push_back(it.get());

		return ret;
	}

	const Stage* Scene::GetStageByKey(const RE::BSFixedString& a_key) const
	{
		if (a_key.empty()) {
			return start_animation;
		}

		for (auto&& [key, dest] : graph) {
			if (a_key == key->id.c_str())
				return key;
		}
		return nullptr;
	}

	uint32_t Scene::GetSubmissiveCount() const
	{
		uint32_t ret = 0;
		for (auto&& info : positions) {
			if (info.extra.all(PositionInfo::Extra::Submissive)) {
				ret++;
			}
		}
		return ret;
	}

	bool Scene::HasCreatures() const
	{
		for (auto&& info : positions) {
			if (info.race != Registry::RaceKey::Human)
				return true;
		}
		return false;
	}

	bool Scene::IsEnabled() const
	{
		return !is_private && enabled;
	}

	std::vector<std::vector<PositionFragment>> Scene::MakeFragments() const
	{
		std::vector<std::vector<PositionFragment>> ret;
		ret.reserve(this->positions.size());
		for (auto&& pinfo : this->positions) {
			ret.push_back(pinfo.MakeFragments());
		}
		return ret;
	}

}	 // namespace Registry
