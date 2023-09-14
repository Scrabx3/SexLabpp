#pragma once

namespace Papyrus::SexLabRegistry
{
	enum class FurniturePreference
	{
		Disallow = 0,
		Default = 1,
		Prefer = 2,
	};

	int32_t GetRaceID(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
  int32_t MapRaceKeyToID(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_racekey);
  std::vector<int32_t> GetRaceIDA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
	std::vector<int32_t> MapRaceKeyToIDA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_racekey);
	RE::BSFixedString GetRaceKey(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
  RE::BSFixedString GetRaceKeyByRace(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESRace* a_race);
  RE::BSFixedString MapRaceIDToRaceKey(RE::StaticFunctionTag*, int32_t a_raceid);
  std::vector<RE::BSFixedString> GetRaceKeyA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);
  std::vector<RE::BSFixedString> GetRaceKeyByRaceA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESRace* a_race);
  std::vector<RE::BSFixedString> MapRaceIDToRaceKeyA(RE::StaticFunctionTag*, int32_t a_raceid);
  std::vector<RE::BSFixedString> GetAllRaceKeys(RE::StaticFunctionTag*, bool a_ignoreambiguous);
  int32_t GetSex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, bool a_ignoreoverwrite);

	std::vector<RE::BSFixedString> LookupScenes(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissives, FurniturePreference a_furniturepref, RE::TESObjectREFR* a_center);
	std::vector<RE::BSFixedString> LookupScenesA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives, FurniturePreference a_furniturepref, RE::TESObjectREFR* a_center);
	bool ValidateScene(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		RE::BSFixedString a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive);
	bool ValidateSceneA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		RE::BSFixedString a_sceneid, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives);
	std::vector<RE::BSFixedString> ValidateScenes(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::BSFixedString> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, RE::Actor* a_submissive);
	std::vector<RE::BSFixedString> ValidateScenesA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*,
		std::vector<RE::BSFixedString> a_sceneids, std::vector<RE::Actor*> a_positions, std::string a_tags, std::vector<RE::Actor*> a_submissives);

	bool SortByScene(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> a_positions, std::string a_sceneid, bool a_allowfallback);
	int32_t SortBySceneEx(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> a_positions, std::vector<std::string> a_sceneids, bool a_allowfallback);

	bool SceneExists(RE::StaticFunctionTag*, RE::BSFixedString a_sceneid);
	std::vector<RE::BSFixedString> SceneExistA(RE::StaticFunctionTag*, std::vector<RE::BSFixedString> a_sceneids);
	bool IsSceneEnabled(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_sceneid);
	void SetSceneEnabled(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_sceneid, bool a_enabled);
	RE::BSFixedString GetSceneName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_sceneid);

	bool IsSceneTag(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_tag);
	bool IsSceneTagA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, std::vector<std::string_view> a_tags);
	bool IsStageTag(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stageid, RE::BSFixedString a_tag);
	bool IsStageTagA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stageid, std::vector<std::string_view> a_tags);
	std::vector<RE::BSFixedString> GetSceneTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	std::vector<RE::BSFixedString> GetStageTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage);
	std::vector<RE::BSFixedString> GetCommonTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::BSFixedString> a_ids);

	RE::BSFixedString GetAnimationEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	std::vector<RE::BSFixedString> GetAnimationEventA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	RE::BSFixedString BranchTo(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	int32_t GetNumBranches(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage);
	int32_t GetNodeType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	std::vector<RE::BSFixedString> GetPathMin(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage);
	std::vector<RE::BSFixedString> GetPathMax(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage);

	int32_t GetActorCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	int32_t GetOptionalActorCount(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	
	bool IsSimilarPosition(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n, int m);
	bool CanFillPosition(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n, RE::Actor* a_actor);

	std::vector<RE::BSFixedString> GetFixedLengthStages(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	float GetFixedLength(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage);
	std::vector<RE::BSFixedString> GetClimaxStages(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);

	int32_t GetPositionSex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n);
	std::vector<int32_t> GetPositionSexA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	int32_t GetRaceIDPosition(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n);
	std::vector<int32_t> GetRaceIDPositionA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	RE::BSFixedString GetRaceKeyPosition(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int n);
	std::vector<RE::BSFixedString> GetRaceKeyPositionA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);

	std::vector<float> GetOffset(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	std::vector<float> GetOffsetRaw(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);

	int32_t GetStripData(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage, int n);
	std::vector<int32_t> GetStripDataA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, RE::BSFixedString a_stage);

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

		REGISTERFUNC(SortByScene, "SexLabRegistry", true);
		REGISTERFUNC(SortBySceneEx, "SexLabRegistry", true);

		REGISTERFUNC(SceneExists, "SexLabRegistry", true);
		REGISTERFUNC(SceneExistA, "SexLabRegistry", true);
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

		REGISTERFUNC(BranchTo, "SexLabRegistry", true);
		REGISTERFUNC(GetNumBranches, "SexLabRegistry", true);
		REGISTERFUNC(GetNodeType, "SexLabRegistry", true);

		REGISTERFUNC(GetPathMin, "SexLabRegistry", true);
		REGISTERFUNC(GetPathMax, "SexLabRegistry", true);

		REGISTERFUNC(GetActorCount, "SexLabRegistry", true);
		REGISTERFUNC(GetOptionalActorCount, "SexLabRegistry", true);

		REGISTERFUNC(IsSimilarPosition, "SexLabRegistry", true);
		REGISTERFUNC(CanFillPosition, "SexLabRegistry", true);

		REGISTERFUNC(GetFixedLengthStages, "SexLabRegistry", true);
		REGISTERFUNC(GetFixedLength, "SexLabRegistry", true);
		REGISTERFUNC(GetClimaxStages, "SexLabRegistry", true);

		REGISTERFUNC(GetPositionSex, "SexLabRegistry", true);
		REGISTERFUNC(GetPositionSexA, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceIDPosition, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceIDPositionA, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyPosition, "SexLabRegistry", true);
		REGISTERFUNC(GetRaceKeyPositionA, "SexLabRegistry", true);

		REGISTERFUNC(GetOffset, "SexLabRegistry", true);
		REGISTERFUNC(GetOffsetRaw, "SexLabRegistry", true);

		REGISTERFUNC(GetStripData, "SexLabRegistry", true);
		REGISTERFUNC(GetStripDataA, "SexLabRegistry", true);

		return true;
	}
} // namespace Papyrus::SexLabRegistry
