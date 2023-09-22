#include "SexLabRegistry.h"

#include "Registry/Animation.h"
#include "Registry/Define/RaceKey.h"
#include "Registry/Library.h"

namespace Papyrus::SexLabRegistry
{
#define SCENE(argRet)                                 \
	const auto lib = Registry::Library::GetSingleton(); \
	const auto scene = lib->GetSceneByID(a_id);         \
	if (!scene) {                                       \
		a_vm->TraceStack("Invalid scene id", a_stackID);  \
		return argRet;                                    \
	}

#define STAGE(argRet)                                \
	const auto stage = scene->GetStageByKey(a_stage);  \
	if (!stage) {                                      \
		a_vm->TraceStack("Invalid stage id", a_stackID); \
		return argRet;                                   \
	}

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
		} else if (a_furniturepref == FurniturePreference::Prefer) {
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
		RE::BSFixedString a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive)
	{
		auto argSubmissive{ a_submissive ? std::vector<RE::Actor*>{ a_submissive } : std::vector<RE::Actor*>{} };
		return ValidateSceneA(a_vm, a_stackID, nullptr, a_sceneid, a_positions, a_tags, argSubmissive);
	}

	bool ValidateSceneA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		RE::BSFixedString a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives)
	{
		return !ValidateScenesA(a_vm, a_stackID, nullptr, { a_sceneid }, a_positions, a_tags, a_submissives).empty();
	}

	std::vector<RE::BSFixedString> ValidateScenes(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::BSFixedString> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive)
	{
		auto argSubmissive{ a_submissive ? std::vector<RE::Actor*>{ a_submissive } : std::vector<RE::Actor*>{} };
		return ValidateScenesA(a_vm, a_stackID, nullptr, a_sceneids, a_positions, a_tags, argSubmissive);
	}

	std::vector<RE::BSFixedString> ValidateScenesA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::BSFixedString> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives)
	{
		if (a_positions.empty()) {
			a_vm->TraceStack("Cannot validate scenes against an empty position array", a_stackID);
			return {};
		}
		if (std::find(a_positions.begin(), a_positions.end(), nullptr) != a_positions.end()) {
			a_vm->TraceStack("Array contains none", a_stackID);
			return {};
		}
		if (std::find(a_submissives.begin(), a_submissives.end(), nullptr) != a_submissives.end()) {
			a_vm->TraceStack("Array contains none", a_stackID);
			return {};
		}
		if (a_sceneids.empty()) {
			return {};
		}
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
			if (!scene->SortActorsFB(fragments))
				continue;
			ret.push_back(sceneid);
		}
		return ret;
	}

	bool SortByScene(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> a_positions, std::string a_sceneid, bool a_allowfallback)
	{
		const auto lib = Registry::Library::GetSingleton();
		const auto scene = lib->GetSceneByID(a_sceneid);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id ", a_stackID);
			return false;
		}
		std::vector<RE::Actor*> positions{ a_positions.begin(), a_positions.end() };
		const auto ret = scene->SortActors(positions, a_allowfallback);
		if (!ret)
			return false;

		for (size_t i = 0; i < ret->size(); i++) {
			a_positions[i] = ret->at(i);
		}
		return true;
	}

	int32_t SortBySceneEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> a_positions, std::vector<std::string> a_sceneids, bool a_allowfallback)
	{
		const auto lib = Registry::Library::GetSingleton();
		for (size_t i = 0; i < a_sceneids.size(); i++) {
			const auto scene = lib->GetSceneByID(a_sceneids[i]);
			if (!scene) {
				a_vm->TraceStack("Invalid scene id ", a_stackID);
				break;
			}
			std::vector<RE::Actor*> positions{ a_positions.begin(), a_positions.end() };
			const auto result = scene->SortActors(positions, a_allowfallback);
			if (result) {
				for (size_t n = 0; n < result->size(); n++) {
					a_positions[n] = result->at(n);
				}
				return static_cast<int32_t>(i);
			}
		}
		return -1;
	}

	bool SceneExists(RE::StaticFunctionTag*, RE::BSFixedString a_sceneid)
	{
		return Registry::Library::GetSingleton()->GetSceneByID(a_sceneid);
	}

	std::vector<RE::BSFixedString> SceneExistA(RE::StaticFunctionTag*, std::vector<RE::BSFixedString> a_sceneids)
	{
		const auto lib = Registry::Library::GetSingleton();
		std::vector<RE::BSFixedString> ret{};
		ret.reserve(a_sceneids.size());
		for (auto&& id : a_sceneids) {
			if (!lib->GetSceneByID(id))
				continue;
			ret.push_back(id);
		}
		return ret;
	}

	bool IsSceneEnabled(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE(false);
		return scene->enabled;
	}

	void SetSceneEnabled(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, bool a_enabled)
	{
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID_Mutable(a_id);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		}
		scene->enabled = a_enabled;
	}

	RE::BSFixedString GetSceneName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE("");
		return scene->name;
	}

	bool IsSceneTag(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_tag)
	{
		SCENE(false);
		return scene->tags.HasTag(a_tag);
	}

	bool IsSceneTagA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, std::vector<std::string_view> a_tags)
	{
		SCENE(false);
		const auto details = Registry::TagDetails(a_tags);
		return scene->IsCompatibleTags(details);
	}

	bool IsStageTag(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, RE::BSFixedString a_tag)
	{
		SCENE(false);
		STAGE(false);
		return stage->tags.HasTag(a_tag);
	}

	bool IsStageTagA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, std::vector<std::string_view> a_tags)
	{
		SCENE(false);
		STAGE(false);
		const auto details = Registry::TagDetails{ a_tags };
		return details.MatchTags(stage->tags);
	}

	std::vector<RE::BSFixedString> GetSceneTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE({});
		return scene->tags.AsVector();
	}

	std::vector<RE::BSFixedString> GetStageTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		SCENE({});
		STAGE({});
		return stage->tags.AsVector();
	}

	std::vector<RE::BSFixedString> GetCommonTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::BSFixedString> a_ids)
	{
		const auto lib = Registry::Library::GetSingleton();
		Registry::TagData ret{};
		for (auto&& sceneid : a_ids) {
			const auto scene = lib->GetSceneByID(sceneid);
			if (!scene) {
				a_vm->TraceStack("Invalid scene id", a_stackID);
				break;
			}
			ret.AddTag(scene->tags);
		}
		return ret.AsVector();
	}

	RE::BSFixedString GetAnimationEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n)
	{
		SCENE({});
		STAGE({});
		return scene->GetNthAnimationEvent(stage, n);
	}

	std::vector<RE::BSFixedString> GetAnimationEventA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		SCENE({});
		STAGE({});
		return scene->GetAnimationEvents(stage);
	}

	RE::BSFixedString BranchTo(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n)
	{
		SCENE("");
		STAGE("");
		const auto ret = scene->GetNthLinkedStage(stage, n);
		return ret ? ret->id : "";
	}

	int32_t GetNumBranches(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		SCENE(0);
		STAGE(0);
		return static_cast<int32_t>(scene->GetNumLinkedStages(stage));
	}

	int32_t GetNodeType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		SCENE(static_cast<int32_t>(Registry::Scene::NodeType::None));
		STAGE(static_cast<int32_t>(Registry::Scene::NodeType::None));
		return static_cast<int32_t>(scene->GetStageNodeType(stage));
	}

	std::vector<RE::BSFixedString> GetPathMin(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		SCENE({});
		STAGE({});
		const auto path = scene->GetShortestPath(stage);
		std::vector<RE::BSFixedString> ret{};
		for (auto&& p : path) {
			ret.push_back(p->id);
		}
		return ret;
	}

	std::vector<RE::BSFixedString> GetPathMax(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		SCENE({});
		STAGE({});
		const auto path = scene->GetLongestPath(stage);
		std::vector<RE::BSFixedString> ret{};
		for (auto&& p : path) {
			ret.push_back(p->id);
		}
		return ret;
	}

	int32_t GetActorCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE(false);
		return scene->CountPositions();
	}

	bool IsSimilarPosition(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n, int m)
	{
		SCENE(false);
		if (n < 0 || m < 0 || n >= scene->positions.size() || m >= scene->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return false;
		}
		return scene->positions[n].CanFillPosition(scene->positions[m]);
	}

	bool CanFillPosition(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return false;
		}
		SCENE(false);
		if (n < 0 || n >= scene->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return false;
		}
		return scene->positions[n].CanFillPosition(a_actor);
	}

	std::vector<RE::BSFixedString> GetFixedLengthStages(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE({});
		const auto stages = scene->GetFixedLengthStages();
		std::vector<RE::BSFixedString> ret{};
		for (auto&& stage : stages) {
			ret.push_back(stage->id);
		}
		return ret;
	}

	float GetFixedLength(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		SCENE(0);
		STAGE(0);
		return stage->fixedlength;
	}

	std::vector<RE::BSFixedString> GetClimaxStages(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE({});
		const auto stages = scene->GetClimaxStages();
		std::vector<RE::BSFixedString> ret{};
		for (auto&& stage : stages) {
			ret.push_back(stage->id);
		}
		return ret;
	}

	int32_t GetPositionSex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n)
	{
		SCENE(0);
		if (n < 0 || n >= scene->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return 0;
		}
		return static_cast<int32_t>(scene->positions[n].GetSexPapyrus());
	}

	std::vector<int32_t> GetPositionSexA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE({});
		std::vector<int32_t> ret{};
		ret.reserve(scene->positions.size());
		for (auto&& position : scene->positions) {
			const auto sex = static_cast<int32_t>(position.GetSexPapyrus());
			ret.push_back(sex);
		}
		return ret;
	}

	int32_t GetRaceIDPosition(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n)
	{
		SCENE(0);
		if (n < 0 || n >= scene->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return 0;
		}
		return static_cast<int32_t>(scene->positions[n].race);
	}

	std::vector<int32_t> GetRaceIDPositionA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE({});
		std::vector<int32_t> ret{};
		ret.reserve(scene->positions.size());
		for (auto&& position : scene->positions) {
			const auto race = static_cast<int32_t>(position.race);
			ret.push_back(race);
		}
		return ret;
	}

	RE::BSFixedString GetRaceKeyPosition(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n)
	{
		SCENE(0);
		if (n < 0 || n >= scene->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return 0;
		}
		return Registry::RaceHandler::AsString(scene->positions[n].race);
	}

	std::vector<RE::BSFixedString> GetRaceKeyPositionA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		SCENE({});
		std::vector<RE::BSFixedString> ret{};
		ret.reserve(scene->positions.size());
		for (auto&& position : scene->positions) {
			const auto race = Registry::RaceHandler::AsString(position.race);
			ret.push_back(race);
		}
		return ret;
	}

	std::vector<float> GetOffset(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n)
	{
		std::vector<float> argRet{ 0, 0, 0, 0 };
		SCENE(argRet);
		STAGE(argRet);
		if (n < 0 || n >= stage->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return argRet;
		}
		const auto ret = stage->positions[n].offset.GetOffset();
		return { ret.begin(), ret.end() };
	}

	std::vector<float> GetOffsetRaw(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n)
	{
		std::vector<float> argRet{ 0, 0, 0, 0 };
		SCENE(argRet);
		STAGE(argRet);
		if (n < 0 || n >= stage->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return argRet;
		}
		const auto ret = stage->positions[n].offset.GetRawOffset();
		return { ret.begin(), ret.end() };
	}

	void UpdateOffset(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n, float a_value, Registry::Offset a_idx)
	{
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID_Mutable(a_id);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		}
		const auto stage = scene->GetStageByKey_Mutable(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return;
		}
		if (n < 0 || n >= stage->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return;
		}
		if (a_idx < Registry::Offset::X || a_idx >= Registry::Offset::Total) {
			a_vm->TraceStack("Invalid offset idx", a_stackID);
			return;
		}
		stage->positions[n].offset.UpdateOffset(a_value, a_idx);
	}

	void UpdateOffsetA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n, std::vector<float> a_newoffset)
	{
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID_Mutable(a_id);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		}
		const auto stage = scene->GetStageByKey_Mutable(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return;
		}
		if (n < 0 || n >= stage->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return;
		}
		if (a_newoffset.size() < Registry::Offset::Total) {
			a_vm->TraceStack("New offsets are of incorrect size", a_stackID);
			return;
		}
		stage->positions[n].offset.UpdateOffset({
			a_newoffset[Registry::Offset::X],
			a_newoffset[Registry::Offset::Y],
			a_newoffset[Registry::Offset::Z],
			a_newoffset[Registry::Offset::R],
		});
	}

	void ResetOffset(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n)
	{
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID_Mutable(a_id);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		}
		const auto stage = scene->GetStageByKey_Mutable(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return;
		}
		if (n < 0 || n >= stage->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return;
		}
		stage->positions[n].offset.ResetOffset();
	}


	void ResetOffsetA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID_Mutable(a_id);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		}
		const auto stage = scene->GetStageByKey_Mutable(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return;
		}
		for (auto&& pos : stage->positions) {
			pos.offset.ResetOffset();
		}
	}

	int32_t GetStripData(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n)
	{
		SCENE(0);
		STAGE(0);
		if (n < 0 || n >= stage->positions.size()) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return 0;
		}
		return stage->positions[n].strips.underlying();
	}

	std::vector<int32_t> GetStripDataA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage)
	{
		SCENE({});
		STAGE({});
		std::vector<int32_t> ret{};
		ret.reserve(stage->positions.size());
		for (auto&& position : stage->positions) {
			ret.push_back(position.strips.underlying());
		}
		return ret;
	}

}	 // namespace Papyrus::SexLabRegistry
