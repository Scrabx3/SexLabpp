#include "Animation.h"

#include "Registry/Define/RaceKey.h"

namespace Registry
{
	stl::enumeration<PositionFragment, FragmentUnderlying> MakePositionFragment(RE::Actor* a_actor, bool a_submissive)
	{
		auto base = a_actor->GetActorBase();
		if (!base) {
			logger::error("Invalid Actor {:X} (0): Missing base object", a_actor->formID);
			return { PositionFragment::None };
		}

		auto sex = base ? base->GetSex() : RE::SEXES::kNone;
		stl::enumeration<PositionFragment, FragmentUnderlying> ret{};
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
				const auto val = PositionFragment(static_cast<FragmentUnderlying>(racekey) << 3);
				ret.set(val);
			}
			break;
		}

		if (a_actor->IsDead() || a_actor->IsUnconscious()) {
			ret.set(PositionFragment::Unconscious);
		} else if (a_submissive) {
			ret.set(PositionFragment::Submissive);
		}

		return ret;
	}

	bool PositionInfo::CanFillPosition(RE::Actor* a_actor) const
	{
		if (extra.all(Extra::Vamprie) && !IsVampire(a_actor))
			return false;
		if (extra.all(Extra::Unconscious) && !a_actor->IsDead() && !a_actor->IsUnconscious())
			return false;
		if (sex.none(GetSex(a_actor)))
			return false;
		if (!RaceHandler::HasRaceKey(a_actor, race))
			return false;

		return true;	
	}

	bool PositionInfo::CanFillPosition(PositionFragmentation a_fragment) const
	{
		if (a_fragment.all(PositionFragment::Futa) && !this->sex.all(Sex::Futa)) {
			return false;
		} else if (a_fragment.all(PositionFragment::Male) && !this->sex.all(Sex::Male)) {
			return false;
		} else if (a_fragment.all(PositionFragment::Female) && !this->sex.all(Sex::Female)) {
			return false;
		}

		if (a_fragment.all(PositionFragment::Unconscious) != this->extra.all(Extra::Unconscious)) {
			return false;
		}
		if (a_fragment.all(PositionFragment::Submissive) != this->extra.all(Extra::Submissive)) {
			return false;
		}

		if (a_fragment.all(PositionFragment::Human)) {
			if (this->extra.all(Extra::Vamprie) && !a_fragment.all(PositionFragment::Vampire)) {
				return false;
			}
			// COMEBACK: Bindings check
		} else {
			const auto thisrace = static_cast<uint64_t>(race);
			if ((a_fragment.underlying() & thisrace) != thisrace)
				return false;
		}
		return true;
	}

	std::vector<PositionFragmentation> PositionInfo::MakeFragments() const
	{
		std::vector<PositionFragmentation> fragments;
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
		const auto addRaceVariance = [&](RaceKey a_type) {
			const auto val = PositionFragment(static_cast<FragmentUnderlying>(a_type) << 3);
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
		const auto setRaceBit = [&](RaceKey a_type) {
			const auto val = PositionFragment(static_cast<FragmentUnderlying>(a_type) << 3);
			setFragmentBit(val);
		};

		if (this->sex.all(Sex::Male)) {
			fragments.emplace_back(PositionFragment::Male);
		}
		if (this->sex.all(Sex::Female)) {
			fragments.emplace_back(PositionFragment::Female);
		}
		if (this->sex.all(Sex::Futa)) {
			fragments.emplace_back(PositionFragment::Futa);
		}

		if (this->extra.all(PositionInfo::Extra::Unconscious)) {
			setFragmentBit(PositionFragment::Unconscious);
		} else if (this->extra.all(PositionInfo::Extra::Submissive)) {
			setFragmentBit(PositionFragment::Submissive);
		}

		switch (this->race) {
		case RaceKey::Human:
			{
				setFragmentBit(PositionFragment::Human);
				if (this->extra.all(PositionInfo::Extra::Vamprie)) {
					setFragmentBit(PositionFragment::Vampire);
				} else {
					addVariance(PositionFragment::Vampire);
				}
				// COMEBACK: bound extra
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
		return fragments;
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

	std::vector<std::vector<PositionFragmentation>> Scene::GetFragmentations() const
	{
		std::vector<std::vector<PositionFragmentation>> ret{};
		ret.reserve(positions.size());
		for (auto&& info : positions) {
			ret.push_back(info.MakeFragments());
		}
		return ret;
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

}	 // namespace Registry
