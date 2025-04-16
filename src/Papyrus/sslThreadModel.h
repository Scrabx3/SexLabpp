#pragma once

#include "Registry/Define/Animation.h"

namespace Papyrus::ThreadModel
{
	enum FurniStatus
	{
		Disallow = 0,
		Allow = 1,
		Prefer = 2,
	};

	namespace ActorAlias
	{
		enum STATUS05
		{
			Unconscious = -5,
			Dying = -10,
		};

		void LockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias);
		void UnlockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias);

		std::vector<RE::TESForm*> StripByData(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias,
			Registry::Position::StripData a_stripdata, std::vector<uint32_t> a_defaults, std::vector<uint32_t> a_overwrite);
		std::vector<RE::TESForm*> StripByDataEx(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias,
			Registry::Position::StripData a_stripdata, std::vector<uint32_t> a_defaults, std::vector<uint32_t> a_overwrite, std::vector<RE::TESForm*> a_mergewith);

		inline bool Register(VM* a_vm)
		{
			REGISTERFUNC(LockActorImpl, "sslActorAlias", false);
			REGISTERFUNC(UnlockActorImpl, "sslActorAlias", false);

			REGISTERFUNC(StripByData, "sslActorAlias", false);
			REGISTERFUNC(StripByDataEx, "sslActorAlias", false);

			return true;
		}
	}	 // namespace ActorAlias

	RE::TESObjectREFR* FindCenter(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst,
		const std::vector<RE::BSFixedString> a_scenes, RE::reference_array<RE::BSFixedString> a_out_scenes, RE::reference_array<float> a_out_coordinates, FurniStatus a_status);
	bool UpdateBaseCoordinates(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::BSFixedString a_sceneid, RE::reference_array<float> a_out);
	void ApplySceneOffset(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::BSFixedString a_sceneid, RE::reference_array<float> a_out);

	int SelectNextStage(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::BSFixedString a_scene, RE::BSFixedString a_stage, std::vector<RE::BSFixedString> a_tags);
	RE::BSFixedString PlaceAndPlay(VM* a_vm, StackID a_stackID, RE::TESQuest*,
		std::vector<RE::Actor*> a_positions, std::vector<float> a_coordinates, RE::BSFixedString a_scene, RE::BSFixedString a_stage);
	void RePlace(VM* a_vm, StackID a_stackID, RE::TESQuest*,
		RE::Actor* a_position, std::vector<float> a_coordinates, RE::BSFixedString a_scene, RE::BSFixedString a_stage, int32_t n);

	std::vector<RE::BSFixedString> AddContextExImpl(RE::TESQuest*, std::vector<RE::BSFixedString> a_oldcontext, std::string a_newcontext);
	void ShuffleScenes(RE::TESQuest*, RE::reference_array<RE::BSFixedString> a_scenes, RE::BSFixedString a_tofront);

	bool IsCollisionRegistered(RE::TESQuest* a_qst);
	void RegisterCollision(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions, RE::BSFixedString a_activescene);
	void UnregisterCollision(RE::TESQuest* a_qst);
	std::vector<int> GetCollisionActions(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, RE::Actor* a_partner);
	bool HasCollisionAction(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int a_type, RE::Actor* a_position, RE::Actor* a_partner);
	RE::Actor* GetPartnerByAction(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type);
	std::vector<RE::Actor*> GetPartnersByAction(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type);
	RE::Actor* GetPartnerByTypeRev(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type);
	std::vector<RE::Actor*> GetPartnersByTypeRev(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type);
	float GetActionVelocity(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, RE::Actor* a_partner, int a_type);

	void AddExperience(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions,
		RE::BSFixedString a_scene, std::vector<RE::BSFixedString> a_playedstages);
	void UpdateStatistics(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_actor, std::vector<RE::Actor*> a_positions,
		RE::BSFixedString a_scene, std::vector<RE::BSFixedString> a_playedstages, float a_time);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(FindCenter, "sslThreadModel", true);
		REGISTERFUNC(UpdateBaseCoordinates, "sslThreadModel", true);
		REGISTERFUNC(ApplySceneOffset, "sslThreadModel", true);

		REGISTERFUNC(SelectNextStage, "sslThreadModel", false);
		REGISTERFUNC(PlaceAndPlay, "sslThreadModel", false);
		REGISTERFUNC(RePlace, "sslThreadModel", false);

		REGISTERFUNC(AddContextExImpl, "sslThreadModel", true);
		REGISTERFUNC(ShuffleScenes, "sslThreadModel", true);

		REGISTERFUNC(IsCollisionRegistered, "sslThreadModel", true);
		REGISTERFUNC(RegisterCollision, "sslThreadModel", true);
		REGISTERFUNC(UnregisterCollision, "sslThreadModel", true);
		REGISTERFUNC(GetCollisionActions, "sslThreadModel", true);
		REGISTERFUNC(HasCollisionAction, "sslThreadModel", true);
		REGISTERFUNC(GetPartnerByAction, "sslThreadModel", true);
		REGISTERFUNC(GetPartnersByAction, "sslThreadModel", true);
		REGISTERFUNC(GetPartnerByTypeRev, "sslThreadModel", true);
		REGISTERFUNC(GetPartnersByTypeRev, "sslThreadModel", true);
		REGISTERFUNC(GetActionVelocity, "sslThreadModel", true);

		REGISTERFUNC(AddExperience, "sslThreadModel", true);
		REGISTERFUNC(UpdateStatistics, "sslThreadModel", true);

		return ActorAlias::Register(a_vm);
	}

}	 // namespace Papyrus::ThreadModel
