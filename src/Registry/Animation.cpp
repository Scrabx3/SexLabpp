#include "Animation.h"

#include "Registry/Define/RaceKey.h"
#include "Util/Combinatorics.h"

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

	bool Scene::HasCreatures() const
	{
		for (auto&& info : positions) {
			if (!info.IsHuman())
				return true;
		}
		return false;
	}

	uint32_t Scene::CountSubmissives() const
	{
		uint32_t ret = 0;
		for (auto&& info : positions) {
			if (info.IsSubmissive()) {
				ret++;
			}
		}
		return ret;
	}

	uint32_t Scene::CountPositions() const
	{
		return static_cast<uint32_t>(positions.size());
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

	bool Scene::IsCompatibleTags(const TagData& a_tags) const
	{
		return this->tags.HasTags(a_tags, true);
	}
	bool Scene::IsCompatibleTags(const TagDetails& a_details) const
	{
		return a_details.MatchTags(tags);
	}

	bool Scene::UsesFurniture() const
	{
		return this->furnitures.furnitures != FurnitureType::None;
	}

	bool Scene::IsCompatibleFurniture(RE::TESObjectREFR* a_reference) const
	{
		const auto type = FurnitureHandler::GetFurnitureType(a_reference);
		return IsCompatibleFurniture(type);
	}

	bool Scene::IsCompatibleFurniture(FurnitureType a_furniture) const
	{
		switch (a_furniture) {
		case FurnitureType::None:
			return !UsesFurniture();
		case FurnitureType::BedDouble:
		case FurnitureType::BedSingle:
			if (this->furnitures.allowbed)
				return true;
			break;
		}
		return this->furnitures.furnitures.any(a_furniture);
	}

	bool Scene::Legacy_IsCompatibleSexCount(int32_t a_males, int32_t a_females) const
	{
		if (a_males < 0 && a_females < 0) {
			return true;
		}

		bool ret = false;
		tags.ForEachExtra([&](const std::string_view a_tag) {
			if (a_tag.find_first_not_of("MFC") != std::string_view::npos) {
				return false;
			}
			if (a_males == -1 || std::count(a_tag.begin(), a_tag.end(), 'M') == a_males) {
				if (a_females == -1 || std::count(a_tag.begin(), a_tag.end(), 'F') == a_females) {
					ret = true;
					return true;
				}
			}
			return false;
		});
		return ret;
	}

	bool Scene::Legacy_IsCompatibleSexCountCrt(int32_t a_males, int32_t a_females) const
	{
		enum
		{
			Male = 0,
			Female = 1,
			Either = 2,
		};

		bool ret = false;
		tags.ForEachExtra([&](const std::string_view a_tag) {
			if (a_tag.find_first_not_of("MFC") != std::string_view::npos) {
				return false;
			}
			const auto crt_total = std::count(a_tag.begin(), a_tag.end(), 'C');
			if (crt_total != a_males + a_females) {
				return true;
			}

			std::vector<int> count;
			for (auto&& position : positions) {
				if (position.race == Registry::RaceKey::Human)
					continue;
				if (position.sex.none(Registry::Sex::Female)) {
					count[Male]++;
				} else if (position.sex.none(Registry::Sex::Male)) {
					count[Female]++;
				} else {
					count[Either]++;
				}
			}
			if (count[Male] <= a_males && count[Male] + count[Either] >= a_males) {
				count[Either] -= a_males - count[Male];
				ret = count[Female] + count[Either] == a_females;
			}
			return true;
		});
		return ret;
	}

	std::optional<std::vector<RE::Actor*>> Scene::SortActors(const std::vector<RE::Actor*>& a_positions, bool a_withfallback) const
	{
		if (a_positions.size() < positions.size())
			return std::nullopt;

		std::vector<std::pair<RE::Actor*, Registry::PositionFragment>> argActor{};
		for (size_t i = 0; i < positions.size(); i++) {
			argActor.emplace_back(
				a_positions[i],
				MakeFragmentFromActor(a_positions[i], positions[i].IsSubmissive())
			);
		}
		return a_withfallback ? SortActors(argActor) : SortActorsFB(argActor);
	}

	std::optional<std::vector<RE::Actor*>> Scene::SortActors(const std::vector<std::pair<RE::Actor*, Registry::PositionFragment>>& a_positions) const
	{
		if (a_positions.size() < positions.size())
			return std::nullopt;
		// Mark every position that every actor can be placed in
		std::vector<std::vector<std::pair<size_t, RE::Actor*>>> compatibles{};
		compatibles.reserve(positions.size());
		for (size_t i = 0; i < a_positions.size(); i++) {
			for (size_t n = 0; n < positions.size(); n++) {
				if (positions[i].CanFillPosition(a_positions[i].second)) {
					compatibles[i].emplace_back(n, a_positions[i].first);
				}
			}
		}
		// Then find a combination of compatibles that consists exclusively of unique elements
		std::vector<RE::Actor*> ret{};
		Combinatorics::ForEachCombination(compatibles, [&](auto it) {
			// Iteration always use the same nth actor + some idx of a compatible position
			std::vector<RE::Actor*> result{ it.size(), nullptr };
			for (auto&& current : it) {
				const auto [scene_idx, actor] = *current;
				if (result[scene_idx] != nullptr) {
					return Combinatorics::CResult::Next;
				}
				result[scene_idx] = actor;
			}
			assert(std::find(result.begin(), result.end(), nullptr) == result.end());
			ret = result;
			return Combinatorics::CResult::Stop;
		});
		if (ret.empty()) {
			return std::nullopt;
		}
		return ret;
	}

	std::optional<std::vector<RE::Actor*>> Scene::SortActorsFB(std::vector<std::pair<RE::Actor*, Registry::PositionFragment>> a_positions) const
	{
		auto ret = SortActors(a_positions);
		if (ret)
			return ret;

		for (auto&& [actor, fragment] : a_positions) {
			auto e = stl::enumeration(fragment);
			if (e.all(PositionFragment::Human, PositionFragment::Female) && e.none(PositionFragment::Male)) {
				e.reset(PositionFragment::Female);
				e.set(PositionFragment::Male);
			}
			fragment = e.get();
			ret = SortActors(a_positions);
			if (ret)
				return ret;
		}
		return std::nullopt;
	}

	}	 // namespace Registry
