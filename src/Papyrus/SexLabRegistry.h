#pragma once

#include "Registry/Define/Transform.h"

namespace Papyrus::SexLabRegistry
{
	enum class FurniturePreference
	{
		Disallow = 0,
		Default = 1,
		Prefer = 2,
	};

	int32_t GetRaceID(STATICARGS, RE::Actor* a_actor);
	int32_t MapRaceKeyToID(STATICARGS, RE::BSFixedString a_racekey);
	std::vector<int32_t> GetRaceIDA(STATICARGS, RE::Actor* a_actor);
	std::vector<int32_t> MapRaceKeyToIDA(STATICARGS, RE::BSFixedString a_racekey);
	RE::BSFixedString GetRaceKey(STATICARGS, RE::Actor* a_actor);
	RE::BSFixedString GetRaceKeyByRace(STATICARGS, RE::TESRace* a_race);
	RE::BSFixedString MapRaceIDToRaceKey(RE::StaticFunctionTag*, int32_t a_raceid);
	std::vector<RE::BSFixedString> GetRaceKeyA(STATICARGS, RE::Actor* a_actor);
	std::vector<RE::BSFixedString> GetRaceKeyByRaceA(STATICARGS, RE::TESRace* a_race);
	std::vector<RE::BSFixedString> MapRaceIDToRaceKeyA(RE::StaticFunctionTag*, int32_t a_raceid);
	std::vector<RE::BSFixedString> GetAllRaceKeys(RE::StaticFunctionTag*, bool a_ignoreambiguous);
	int32_t GetSex(STATICARGS, RE::Actor* a_actor, bool a_ignoreoverwrite);

	std::vector<RE::BSFixedString> LookupScenes(STATICARGS,
		std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissives, FurniturePreference a_furniturepref, RE::TESObjectREFR* a_center);
	std::vector<RE::BSFixedString> LookupScenesA(STATICARGS, 
		std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives, FurniturePreference a_furniturepref, RE::TESObjectREFR* a_center);
	bool ValidateScene(STATICARGS, RE::BSFixedString a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive);
	bool ValidateSceneA(STATICARGS, RE::BSFixedString a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives);
	std::vector<RE::BSFixedString> ValidateScenes(STATICARGS, 
		std::vector<RE::BSFixedString> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive);
	std::vector<RE::BSFixedString> ValidateScenesA(STATICARGS,
		std::vector<RE::BSFixedString> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives);
	RE::BSFixedString GetSceneByName(RE::StaticFunctionTag*, RE::BSFixedString a_name);

	bool SortByScene(STATICARGS, RE::reference_array<RE::Actor*> a_positions, RE::Actor* a_victim, std::string a_sceneid);
	bool SortBySceneA(STATICARGS, RE::reference_array<RE::Actor*> a_positions, std::vector<RE::Actor*> a_victims, std::string _sceneid);
	int32_t SortBySceneEx(STATICARGS, RE::reference_array<RE::Actor*> a_positions, RE::Actor* a_victim, std::vector<std::string> a_sceneids);
	int32_t SortBySceneExA(STATICARGS, RE::reference_array<RE::Actor*> a_positions, std::vector<RE::Actor*> a_victims, std::vector<std::string> a_sceneids);

	bool SceneExists(RE::StaticFunctionTag*, RE::BSFixedString a_sceneid);
	std::vector<RE::BSFixedString> SceneExistA(RE::StaticFunctionTag*, std::vector<RE::BSFixedString> a_sceneids);
	bool StageExists(STATICARGS, RE::BSFixedString a_sceneid, RE::BSFixedString a_stage);
	bool IsSceneEnabled(STATICARGS, RE::BSFixedString a_sceneid);
	void SetSceneEnabled(STATICARGS, RE::BSFixedString a_sceneid, bool a_enabled);
	RE::BSFixedString GetSceneName(STATICARGS, RE::BSFixedString a_sceneid);

	bool IsSceneTag(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_tag);
	bool IsSceneTagA(STATICARGS, RE::BSFixedString a_id, std::vector<std::string_view> a_tags);
	bool IsStageTag(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stageid, RE::BSFixedString a_tag);
	bool IsStageTagA(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stageid, std::vector<std::string_view> a_tags);
	std::vector<RE::BSFixedString> GetSceneTags(STATICARGS, RE::BSFixedString a_id);
	std::vector<RE::BSFixedString> GetStageTags(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);
	std::vector<RE::BSFixedString> GetCommonTags(STATICARGS, std::vector<RE::BSFixedString> a_ids);

	RE::BSFixedString GetAnimationEvent(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	std::vector<RE::BSFixedString> GetAnimationEventA(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	RE::BSFixedString GetStartAnimation(STATICARGS, RE::BSFixedString a_id);
	int32_t GetNumStages(STATICARGS, RE::BSFixedString a_id);
	std::vector<RE::BSFixedString> GetAllstages(STATICARGS, RE::BSFixedString a_id);
	RE::BSFixedString BranchTo(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	int32_t GetNumBranches(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);
	int32_t GetNodeType(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	std::vector<RE::BSFixedString> GetPathMin(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);
	std::vector<RE::BSFixedString> GetPathMax(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	int32_t GetActorCount(STATICARGS, RE::BSFixedString a_id);

	bool IsSimilarPosition(STATICARGS, RE::BSFixedString a_id, int n, int m);
	bool CanFillPosition(STATICARGS, RE::BSFixedString a_id, int n, RE::Actor* a_actor);

	std::vector<RE::BSFixedString> GetEndingStages(STATICARGS, RE::BSFixedString a_id);
	std::vector<RE::BSFixedString> GetFixedLengthStages(STATICARGS, RE::BSFixedString a_id);
	float GetFixedLength(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);
	std::vector<RE::BSFixedString> GetClimaxStages(STATICARGS, RE::BSFixedString a_id, int32_t n);
	std::vector<int32_t> GetClimaxingActors(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	int32_t GetPositionSex(STATICARGS, RE::BSFixedString a_id, int n);
	std::vector<int32_t> GetPositionSexA(STATICARGS, RE::BSFixedString a_id);
	int32_t GetRaceIDPosition(STATICARGS, RE::BSFixedString a_id, int n);
	std::vector<int32_t> GetRaceIDPositionA(STATICARGS, RE::BSFixedString a_id);
	RE::BSFixedString GetRaceKeyPosition(STATICARGS, RE::BSFixedString a_id, int n);
	std::vector<RE::BSFixedString> GetRaceKeyPositionA(STATICARGS, RE::BSFixedString a_id);

	std::vector<float> GetOffset(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	std::vector<float> GetOffsetRaw(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	void UpdateOffset(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n, float a_value, Registry::CoordinateType a_idx);
	void UpdateOffsetA(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n, std::vector<float> a_newoffset);
	void ResetOffset(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	void ResetOffsetA(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	int32_t GetStripData(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	std::vector<int32_t> GetStripDataA(STATICARGS, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	bool HasExtraCustom(STATICARGS, RE::BSFixedString a_id, int n, RE::BSFixedString a_extra);
	std::vector<RE::BSFixedString> GetExtraCustom(STATICARGS, RE::BSFixedString a_id, int n);
	std::vector<RE::BSFixedString> GetExtraCustomA(STATICARGS, RE::BSFixedString a_id);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetRaceID, "SexLabRegistry", true);
		REGISTERFUNC(MapRaceKeyToID, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceIDA, "SexLabRegistry", true);
		REGISTERFUNC(MapRaceKeyToIDA, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKey, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyByRace, "SexLabRegistry", true);
		REGISTERFUNC(MapRaceIDToRaceKey, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyA, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyByRaceA, "SexLabRegistry", true);
		REGISTERFUNC(MapRaceIDToRaceKeyA, "SexLabRegistry", true);
		REGISTERFUNC(GetAllRaceKeys, "SexLabRegistry", true);
		REGISTERFUNC(GetSex, "SexLabRegistry", true);

		REGISTERFUNC(LookupScenes, "SexLabRegistry", true);
		REGISTERFUNC(LookupScenesA, "SexLabRegistry", true);
		REGISTERFUNC(ValidateScene, "SexLabRegistry", true);
		REGISTERFUNC(ValidateSceneA, "SexLabRegistry", true);
		REGISTERFUNC(ValidateScenes, "SexLabRegistry", true);
		REGISTERFUNC(ValidateScenesA, "SexLabRegistry", true);
		REGISTERFUNC(GetSceneByName, "SexLabRegistry", true);

		REGISTERFUNC(SortByScene, "SexLabRegistry", true);
		REGISTERFUNC(SortBySceneA, "SexLabRegistry", true);
		REGISTERFUNC(SortBySceneEx, "SexLabRegistry", true);
		REGISTERFUNC(SortBySceneExA, "SexLabRegistry", true);

		REGISTERFUNC(SceneExists, "SexLabRegistry", true);
		REGISTERFUNC(SceneExistA, "SexLabRegistry", true);
		REGISTERFUNC(StageExists, "SexLabRegistry", true);
		REGISTERFUNC(IsSceneEnabled, "SexLabRegistry", true);
		REGISTERFUNC(SetSceneEnabled, "SexLabRegistry", true);
		REGISTERFUNC(GetSceneName, "SexLabRegistry", true);

		REGISTERFUNC(IsSceneTag, "SexLabRegistry", true);
		REGISTERFUNC(IsSceneTagA, "SexLabRegistry", true);
		REGISTERFUNC(IsStageTag, "SexLabRegistry", true);
		REGISTERFUNC(IsStageTagA, "SexLabRegistry", true);
		REGISTERFUNC(GetSceneTags, "SexLabRegistry", true);
		REGISTERFUNC(GetStageTags, "SexLabRegistry", true);
		REGISTERFUNC(GetCommonTags, "SexLabRegistry", true);

		REGISTERFUNC(GetAnimationEvent, "SexLabRegistry", true);
		REGISTERFUNC(GetAnimationEventA, "SexLabRegistry", true);

		REGISTERFUNC(GetStartAnimation, "SexLabRegistry", true);
		REGISTERFUNC(GetNumStages, "SexLabRegistry", true);
		REGISTERFUNC(GetAllstages, "SexLabRegistry", true);
		REGISTERFUNC(BranchTo, "SexLabRegistry", true);
		REGISTERFUNC(GetNumBranches, "SexLabRegistry", true);
		REGISTERFUNC(GetNodeType, "SexLabRegistry", true);

		REGISTERFUNC(GetPathMin, "SexLabRegistry", true);
		REGISTERFUNC(GetPathMax, "SexLabRegistry", true);

		REGISTERFUNC(GetActorCount, "SexLabRegistry", true);

		REGISTERFUNC(IsSimilarPosition, "SexLabRegistry", true);
		REGISTERFUNC(CanFillPosition, "SexLabRegistry", true);

		REGISTERFUNC(GetEndingStages, "SexLabRegistry", true);
		REGISTERFUNC(GetFixedLengthStages, "SexLabRegistry", true);
		REGISTERFUNC(GetFixedLength, "SexLabRegistry", true);
		REGISTERFUNC(GetClimaxStages, "SexLabRegistry", true);
		REGISTERFUNC(GetClimaxingActors, "SexLabRegistry", true);

		REGISTERFUNC(GetPositionSex, "SexLabRegistry", true);
		REGISTERFUNC(GetPositionSexA, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceIDPosition, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceIDPositionA, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyPosition, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyPositionA, "SexLabRegistry", true);

		REGISTERFUNC(GetOffset, "SexLabRegistry", true);
		REGISTERFUNC(GetOffsetRaw, "SexLabRegistry", true);
		REGISTERFUNC(UpdateOffset, "SexLabRegistry", true);
		REGISTERFUNC(UpdateOffsetA, "SexLabRegistry", true);
		REGISTERFUNC(ResetOffset, "SexLabRegistry", true);
		REGISTERFUNC(ResetOffsetA, "SexLabRegistry", true);

		REGISTERFUNC(GetStripData, "SexLabRegistry", true);
		REGISTERFUNC(GetStripDataA, "SexLabRegistry", true);

		return true;
	}
}	 // namespace Papyrus::SexLabRegistry
