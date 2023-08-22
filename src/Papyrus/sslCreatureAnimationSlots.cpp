#include "sslCreatureAnimationSlots.h"

#include "Registry/Library.h"
#include "Registry/Define/RaceKey.h"

namespace Papyrus::CreatureAnimationSlots
{
	std::vector<RE::BSFixedString> GetByRaceKeyTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest*,
		int32_t a_actorcount, 
		RE::BSFixedString a_racekey, 
		std::vector<std::string_view> a_tags)
	{
		if (a_actorcount <= 0 || a_actorcount > Registry::MAX_ACTOR_COUNT) {
			a_vm->TraceStack(fmt::format("Actorcount should be between 0 and {}", Registry::MAX_ACTOR_COUNT).c_str(), a_stackID);
			return {};
		}
    const auto racekey = Registry::RaceHandler::GetRaceKey(a_racekey);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack("Invalid racekey", a_stackID);
			return {};
		}
		std::vector<RE::BSFixedString> ret{};
		ret.reserve(256);
		Registry::Library::GetSingleton()->ForEachScene([&](const Registry::Scene* a_scene) {
			if (!a_scene->IsEnabled())
				return false;
			if (a_scene->positions.size() != a_actorcount)
				return false;
			if (!a_scene->tags.MatchTags(a_tags))
				return false;
			for (auto&& position : a_scene->positions) {
				if (Registry::RaceHandler::IsCompatibleRaceKey(position.race, racekey)) {
					ret.push_back(a_scene->id);
					break;
				}
			}
			return ret.size() == ret.capacity();
		});
		return ret;
	}

	std::vector<RE::BSFixedString> GetByCreatureActorsTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest*,
		int32_t a_actorcount, std::vector<RE::Actor*> a_creatures, std::vector<std::string_view> a_tags)
	{
		if (a_actorcount <= 0 || a_actorcount > Registry::MAX_ACTOR_COUNT) {
			a_vm->TraceStack(fmt::format("Actorcount should be between 0 and {}", Registry::MAX_ACTOR_COUNT).c_str(), a_stackID);
			return {};
		}
		if (a_creatures.empty() || std::find(a_creatures.begin(), a_creatures.end(), nullptr) != a_creatures.end()) {
			a_vm->TraceStack("Array contained none values", a_stackID);
			return {};
		}
		if (a_creatures.size() > a_actorcount) {
			a_vm->TraceStack("Actor array should be less or equal than actor count but was greater", a_stackID);
			return {};
		}

		std::vector<RE::BSFixedString> ret{};
		ret.reserve(256);
		Registry::Library::GetSingleton()->ForEachScene([&](const Registry::Scene* a_scene) {
			if (!a_scene->IsEnabled())
				return false;
			if (a_scene->positions.size() != a_actorcount)
				return false;
			if (!a_scene->tags.MatchTags(a_tags))
				return false;

			int32_t reqtrue = static_cast<int32_t>(a_creatures.size());
			std::vector<bool> control(a_actorcount, false);
			for (auto&& creature : a_creatures) {
				const auto racekey = Registry::RaceHandler::GetRaceKey(creature);
				if (racekey == Registry::RaceKey::None) {
					a_vm->TraceStack("Invalid racekey", a_stackID);
					ret.clear();
					return true;
				}
				if (racekey == Registry::RaceKey::Human) {
					reqtrue -= 1;
					continue;
				}
				for (size_t i = 0; i < a_scene->positions.size(); i++) {
					if (control[i])
						continue;
					const auto& position = a_scene->positions[i];
					if (Registry::RaceHandler::IsCompatibleRaceKey(position.race, racekey)) {
						control[i] = true;
						break;
					}
				}
			}
			if (reqtrue != std::count(control.begin(), control.end(), true)) {
				return false;
			}
			ret.push_back(a_scene->id);
			return ret.size() == ret.capacity();
		});
		return ret;
	}

	std::vector<RE::BSFixedString> GetByRaceGendersTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest*,
		int32_t a_actorcount, 
		RE::BSFixedString a_racekey, 
		int32_t a_malecrt, 
		int32_t a_femalecrt, 
		std::vector<std::string_view> a_tags)
	{
		if (a_actorcount <= 0 || a_actorcount > Registry::MAX_ACTOR_COUNT) {
			a_vm->TraceStack(fmt::format("Actorcount should be between 0 and {}", Registry::MAX_ACTOR_COUNT).c_str(), a_stackID);
			return {};
		}
		if (a_malecrt + a_femalecrt > a_actorcount) {
			a_vm->TraceStack("Expecting more creatures than total positions", a_stackID);
			return {};
		}
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_racekey);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack("Invalid racekey", a_stackID);
			return {};
		}
		std::vector<RE::BSFixedString> ret;
		ret.reserve(256);
		Registry::Library::GetSingleton()->ForEachScene([&](const Registry::Scene* a_scene) {
			if (!a_scene->IsEnabled())
				return false;
			if (a_scene->positions.size() != a_actorcount)
				return false;
			if (!a_scene->tags.MatchTags(a_tags))
				return false;
			bool has_race = false;
			for (auto&& position : a_scene->positions) {
				if (Registry::RaceHandler::IsCompatibleRaceKey(position.race, racekey)) {
					has_race = true;
					break;
				}
			}
			if (!has_race) {
				return false;
			}
			bool match_gender = false;
			for (auto&& tag : a_scene->tags.extra) {
				std::string_view view{ tag.data() };
				if (view.find_first_not_of("MFC") != std::string_view::npos) {
					continue;
				}
				const auto crt_total = std::count(view.begin(), view.end(), 'C');
				if (crt_total != a_femalecrt + a_malecrt)
					break;

				enum
				{
					Male = 0,
					Female = 1,
					Either = 2,
				};
				std::vector<int> count;
				for (auto&& position : a_scene->positions) {
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
				if (count[Male] <= a_malecrt && count[Male] + count[Either] >= a_malecrt) {
					count[Either] -= a_malecrt - count[Male];
					match_gender = count[Female] + count[Either] == a_femalecrt;
				}
				break;
			}
			if (!match_gender) {
				return false;
			}
			ret.push_back(a_scene->id);
			return ret.size() == ret.capacity();
		});
		return ret;
	}

} // namespace Papyrus::CreatureAnimationSlots
