#include "Animation.h"

#include "Registry/Define/RaceKey.h"

namespace Registry
{
	Sex GetSex(RE::Actor* a_actor)
	{
		Sex ret = Sex::None;
		a_actor->VisitFactions([&](auto a_faction, auto a_rank) {
			if (a_faction == GameForms::GenderFaction) {
				switch (a_rank) {
				case 0:
					ret = Sex::Male;
					break;
				case 1:
					ret = Sex::Female;
					break;
				case 2:
					ret = Sex::Futa;
					break;
				default:
					logger::info("Actor {} has invalid gender faction rank ({})", a_actor->GetFormID(), a_rank);
					break;
				}
				return true;
			}
			return false;
		});
		if (ret != Sex::None)
			return ret;

		const auto base = a_actor->GetActorBase();
		if (!base) {
			logger::error("Unable to retrieve actor base for actor {:X}", a_actor->formID);
			return Sex::None;
		}
		switch (base->GetSex()) {
		default:
		case RE::SEXES::kMale:
			return Sex::Male;
		case RE::SEXES::kFemale:
			if (IsNPC(a_actor) && IsFuta(a_actor))
				return Sex::Female;
			else
				return Sex::Futa;
			break;
		}
	}

	
	bool PositionInfo::CanFillPosition(RE::Actor* a_actor)
	{
		if (extra.all(Extra::Vamprie) && !IsVampire(a_actor))
			return false;
		if (extra.all(Extra::Unconscious) && !a_actor->IsDead() && !a_actor->AsActorState()->IsUnconscious())
			return false;
		if (sex.none(GetSex(a_actor)))
			return false;
		if (!RaceHandler::HasRaceKey(a_actor, race))
			return false;

		return true;	
	}

	std::vector<PositionFragment> PositionInfo::MakeFragments()
	{
		using Fragment = stl::enumeration<PositionFragment, FragmentUnderlying>;
		std::vector<Fragment> fragments;
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
		std::vector<PositionFragment> ret;
		for (auto&& fragment : fragments) {
			ret.push_back(fragment.get());
		}
		return ret;
	}


	Stage* Scene::GetStageByKey(std::string_view a_key) const
	{
		for (auto&& [key, dest] : graph) {
			if (key->id == a_key)
				return key;
		}
		return nullptr;
	}

}	 // namespace Registry
