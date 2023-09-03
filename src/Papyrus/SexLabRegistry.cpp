#include "SexLabRegistry.h"

#include "Registry/Animation.h"
#include "Registry/Define/RaceKey.h"
#include "Registry/Library.h"

namespace Papyrus::SexLabRegistry
{
	int32_t GetRaceID(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Cannot get race id of a none ref", a_stackID);
			return 0;
		}
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_actor);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack(fmt::format("Invalid race key for actor {}", a_actor->formID).c_str(), a_stackID);
			return 0;
		}
		return static_cast<int32_t>(racekey);
	}

	int32_t MapRaceKeyToID(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_racekey)
	{
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_racekey);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack(fmt::format("Invalid race key {}", a_racekey).c_str(), a_stackID);
			return 0;
		}
		return static_cast<int32_t>(racekey);
	}

	std::vector<int32_t> GetRaceIDA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		const auto key = GetRaceID(a_vm, a_stackID, nullptr, a_actor);
		if (key == static_cast<int32_t>(Registry::RaceKey::None)) {
			return {};
		}
		std::vector<int32_t> ret{ key };
		const auto alternate = Registry::RaceHandler::GetVariantKey(Registry::RaceKey(key));
		if (alternate != Registry::RaceKey::None) {
			ret.push_back(static_cast<int32_t>(alternate));
		}
		return ret;
	}

	std::vector<int32_t> MapRaceKeyToIDA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_racekey)
	{
		const auto key = MapRaceKeyToID(a_vm, a_stackID, nullptr, a_racekey);
		if (key == static_cast<int32_t>(Registry::RaceKey::None)) {
			return {};
		}
		std::vector<int32_t> ret{ key };
		const auto alternate = Registry::RaceHandler::GetVariantKey(Registry::RaceKey(key));
		if (alternate != Registry::RaceKey::None) {
			ret.push_back(static_cast<int32_t>(alternate));
		}
		return ret;
	}

	RE::BSFixedString GetRaceKey(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Cannot get race key of a none ref", a_stackID);
			return 0;
		}
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_actor);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack(fmt::format("Invalid race key for race {:X}", a_actor->formID).c_str(), a_stackID);
			return 0;
		}
		return Registry::RaceHandler::AsString(racekey);
	}

	RE::BSFixedString GetRaceKeyByRace(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESRace* a_race)
	{
		if (!a_race) {
			a_vm->TraceStack("Cannot get race key of a none race", a_stackID);
			return 0;
		}
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_race);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack(fmt::format("Invalid race key for race {:X}", a_race->formID).c_str(), a_stackID);
			return 0;
		}
		return Registry::RaceHandler::AsString(racekey);
	}

	RE::BSFixedString MapRaceIDToRaceKey(RE::StaticFunctionTag*, int32_t a_raceid)
	{
		return Registry::RaceHandler::AsString(Registry::RaceKey(a_raceid));
	}

	std::vector<RE::BSFixedString> GetRaceKeyA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		const auto ids = GetRaceIDA(a_vm, a_stackID, nullptr, a_actor);
		std::vector<RE::BSFixedString> ret{};
		for (auto&& id : ids) {
			ret.push_back(Registry::RaceHandler::AsString(Registry::RaceKey(id)));
		}
		return ret;
	}

	std::vector<RE::BSFixedString> GetRaceKeyByRaceA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESRace* a_race)
	{
		if (!a_race) {
			a_vm->TraceStack("Cannot get racekeys from none race", a_stackID);
			return {};
		}
		const auto key = Registry::RaceHandler::GetRaceKey(a_race);
		if (key == Registry::RaceKey::None)
			return {};
		const auto variant = Registry::RaceHandler::GetVariantKey(key);
		if (variant == Registry::RaceKey::None)
			return { Registry::RaceHandler::AsString(key) };
		return { Registry::RaceHandler::AsString(key), Registry::RaceHandler::AsString(variant) };
	}

	std::vector<RE::BSFixedString> MapRaceIDToRaceKeyA(RE::StaticFunctionTag*, int32_t a_raceid)
	{
		const auto key1 = Registry::RaceHandler::AsString(Registry::RaceKey(a_raceid));
		if (key1.empty())
			return {};
		const auto variant = Registry::RaceHandler::GetVariantKey(Registry::RaceKey(a_raceid));
		if (variant == Registry::RaceKey::None)
			return { key1 };
		return { key1, Registry::RaceHandler::AsString(variant) };
	}

	std::vector<RE::BSFixedString> GetAllRaceKeys(RE::StaticFunctionTag*, bool a_ignoreambiguous)
	{
		return Registry::RaceHandler::GetAllRaceKeys(a_ignoreambiguous);
	}

	int32_t GetSex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, bool a_ignoreoverwrite)
	{
		if (!a_actor) {
			a_vm->TraceStack("Cannot get sex from none ref", a_stackID);
			return 0;
		}
		auto sex = Registry::GetSex(a_actor, a_ignoreoverwrite);
		switch (sex) {
		case Registry::Sex::Male:
			return Registry::IsNPC(a_actor) ? 0 : 3;
		case Registry::Sex::Female:
			return Registry::IsNPC(a_actor)	 ? 1 :
						 Settings::bCreatureGender ? 4 :
																				 3;
		case Registry::Sex::Futa:
			return Registry::IsNPC(a_actor)	 ? 2 :
						 Settings::bCreatureGender ? 4 :
																				 3;
		default:
			return 0;
		}
	}

	std::vector<RE::BSFixedString> LookupScenes(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive, FurniturePreference a_furniturepref, RE::TESObjectREFR* a_center)
	{
		auto argSubmissive{ a_submissive ? std::vector<RE::Actor*>{ a_submissive } : std::vector<RE::Actor*>{} };
		return LookupScenesA(a_vm, a_stackID, nullptr, a_positions, a_tags, argSubmissive, a_furniturepref, a_center);
	}

	std::vector<RE::BSFixedString> LookupScenesA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives, FurniturePreference a_furniturepref, RE::TESObjectREFR* a_center)
	{
		if (a_positions.empty()) {
			a_vm->TraceStack("Cannot lookup animations without actors", a_stackID);
			return {};
		}
		const auto tags = Registry::StringSplit(a_tags, ',');
		auto scenes = Registry::Library::GetSingleton()->LookupScenes(a_positions, tags, a_submissives);
		const auto pretrim = scenes.size();
		logger::info("Found {} Scenes, trimming by furniture preference and center...", pretrim);
		if (a_center) {
			const auto type = Registry::FurnitureHandler::GetFurnitureType(a_center);
			if (type != Registry::FurnitureType::None) {
				std::erase_if(scenes, [&](Registry::Scene* a_scene) {
					return !a_scene->IsCompatibleFurniture(type);
				});
			}
		} else if (a_furniturepref == FurniturePreference::Prefer ) {
			const auto where = std::remove_if(scenes.begin(), scenes.end(), [&](Registry::Scene* a_scene) {
				return !a_scene->UsesFurniture();
			});
			if (where != scenes.begin()) {
				scenes.erase(where, scenes.end());
			} else {
				logger::info("Furniture preferred but no furniture animations found, ignoring filter");
			}
		} else if (a_furniturepref == FurniturePreference::Disallow) {
			std::erase_if(scenes, [&](Registry::Scene* a_scene) {
				return a_scene->UsesFurniture();
			});
		}
		logger::info("Finished trimming, returning {}/{} scenes", scenes.size(), pretrim);
		std::vector<RE::BSFixedString> ret{};
		ret.reserve(scenes.size());
		for (auto&& scene : scenes)
			ret.push_back(scene->id);
		return ret;
	}

	bool ValidateScene(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::string a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive)
	{
		auto argSubmissive{ a_submissive ? std::vector<RE::Actor*>{ a_submissive } : std::vector<RE::Actor*>{} };
		return ValidateSceneA(a_vm, a_stackID, nullptr, a_sceneid, a_positions, a_tags, argSubmissive);
	}

	bool ValidateSceneA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::string a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives)
	{
		return !ValidateScenesA(a_vm, a_stackID, nullptr, { a_sceneid }, a_positions, a_tags, a_submissives).empty();
	}

	std::vector<RE::BSFixedString> ValidateScenes(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<std::string> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive)
	{
		auto argSubmissive{ a_submissive ? std::vector<RE::Actor*>{ a_submissive } : std::vector<RE::Actor*>{} };
		return ValidateScenesA(a_vm, a_stackID, nullptr, a_sceneids, a_positions, a_tags, argSubmissive);
	}

	std::vector<RE::BSFixedString> ValidateScenesA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<std::string> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives)
	{
		std::vector<RE::BSFixedString> ret{};
		ret.reserve(a_sceneids.size());
		const auto fragments = Registry::MakeFragmentPair(a_positions, a_submissives);
		const auto tagdetail = Registry::TagDetails{ a_tags };
		const auto lib = Registry::Library::GetSingleton();
		for (auto&& sceneid : a_sceneids) {
			const auto scene = lib->GetSceneByID(sceneid);
			if (!scene) {
				a_vm->TraceStack("Invalid scene id ", a_stackID);
				break;
			}
			if (!scene->IsCompatibleTags(tagdetail))
				continue;
			if (!scene->SortActors(fragments))
				continue;
			ret.push_back(sceneid);
		}
		return ret;
	}

	bool SortByScene(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_positions, std::string a_sceneid, bool a_allowfallback)
	{
		const auto lib = Registry::Library::GetSingleton();
		const auto scene = lib->GetSceneByID(a_sceneid);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id ", a_stackID);
			return false;
		}
		const auto ret = scene->SortActors(a_positions, a_allowfallback);
		if (!ret)
			return false;

		for (size_t i = 0; i < ret->size(); i++) {
			a_positions[i] = ret->at(i);
		}
		return true;
	}

	int32_t SortBySceneEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_positions, std::vector<std::string> a_sceneids, bool a_allowfallback)
	{
		const auto lib = Registry::Library::GetSingleton();
		for (size_t i = 0; i < a_sceneids.size(); i++) {
			const auto scene = lib->GetSceneByID(a_sceneids[i]);
			if (!scene) {
				a_vm->TraceStack("Invalid scene id ", a_stackID);
				break;
			}
			const auto result = scene->SortActors(a_positions, a_allowfallback);
			if (result) {
				for (size_t n = 0; n < result->size(); n++) {
					a_positions[n] = result->at(n);
				}
				return static_cast<int32_t>(i);
			}
		}

		return -1;
	}


}	 // namespace Papyrus::SexLabRegistry
