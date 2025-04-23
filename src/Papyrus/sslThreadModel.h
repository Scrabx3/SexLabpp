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

	RE::BSFixedString GetActiveScene(QUESTARGS);
	RE::BSFixedString GetActiveStage(QUESTARGS);
	std::vector<RE::BSFixedString> GetPlayingScenes(QUESTARGS);
	std::vector<RE::Actor*> GetPositions(QUESTARGS);
	std::vector<RE::BSFixedString> AddContextExImpl(RE::TESQuest*, std::vector<RE::BSFixedString> a_oldcontext, std::string a_newcontext);

	bool CreateInstance(QUESTARGS, std::vector<RE::Actor*> a_submissives, std::vector<RE::BSFixedString> a_scenesPrimary, std::vector<RE::BSFixedString> a_scenesLeadIn, std::vector<RE::BSFixedString> a_scenesCustom, int a_furniturepref);
	std::vector<RE::BSFixedString> GetLeadInScenes(QUESTARGS);
	std::vector<RE::BSFixedString> GetPrimaryScenes(QUESTARGS);
	std::vector<RE::BSFixedString> GetCustomScenes(QUESTARGS);
	std::vector<RE::BSFixedString> AdvanceScene(QUESTARGS, std::vector<RE::BSFixedString> a_history, RE::BSFixedString a_nextStage);
	int SelectNextStage(QUESTARGS, std::vector<RE::BSFixedString> a_tags);
	bool SetActiveScene(QUESTARGS, RE::BSFixedString a_sceneid);
	bool ReassignCenter(QUESTARGS, RE::TESObjectREFR* a_centeron);
	void UpdatePlacement(QUESTARGS, RE::Actor* a_position);

	bool IsCollisionRegistered(RE::TESQuest* a_qst);
	void UnregisterCollision(RE::TESQuest* a_qst);
	std::vector<int> GetCollisionActions(QUESTARGS, RE::Actor* a_position, RE::Actor* a_partner);
	bool HasCollisionAction(QUESTARGS, int a_type, RE::Actor* a_position, RE::Actor* a_partner);
	RE::Actor* GetPartnerByAction(QUESTARGS, RE::Actor* a_position, int a_type);
	std::vector<RE::Actor*> GetPartnersByAction(QUESTARGS, RE::Actor* a_position, int a_type);
	RE::Actor* GetPartnerByTypeRev(QUESTARGS, RE::Actor* a_position, int a_type);
	std::vector<RE::Actor*> GetPartnersByTypeRev(QUESTARGS, RE::Actor* a_position, int a_type);
	float GetActionVelocity(QUESTARGS, RE::Actor* a_position, RE::Actor* a_partner, int a_type);

	void AddExperience(QUESTARGS, std::vector<RE::Actor*> a_positions, RE::BSFixedString a_scene, std::vector<RE::BSFixedString> a_playedstages);
	void UpdateStatistics(QUESTARGS, RE::Actor* a_actor, std::vector<RE::Actor*> a_positions, RE::BSFixedString a_scene, std::vector<RE::BSFixedString> a_playedstages, float a_time);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetActiveScene, "sslThreadModel", false);
		REGISTERFUNC(GetActiveStage, "sslThreadModel", false);
		REGISTERFUNC(GetPlayingScenes, "sslThreadModel", false);
		REGISTERFUNC(GetPositions, "sslThreadModel", false);

		REGISTERFUNC(CreateInstance, "sslThreadModel", false);
		REGISTERFUNC(GetLeadInScenes, "sslThreadModel", false);
		REGISTERFUNC(GetPrimaryScenes, "sslThreadModel", false);
		REGISTERFUNC(GetCustomScenes, "sslThreadModel", false);
		REGISTERFUNC(AdvanceScene, "sslThreadModel", false);
		REGISTERFUNC(SelectNextStage, "sslThreadModel", false);
		REGISTERFUNC(SetActiveScene, "sslThreadModel", false);
		REGISTERFUNC(ReassignCenter, "sslThreadModel", false);
		REGISTERFUNC(UpdatePlacement, "sslThreadModel", false);

		REGISTERFUNC(IsCollisionRegistered, "sslThreadModel", true);
		REGISTERFUNC(UnregisterCollision, "sslThreadModel", true);
		REGISTERFUNC(GetCollisionActions, "sslThreadModel", true);
		REGISTERFUNC(HasCollisionAction, "sslThreadModel", true);
		REGISTERFUNC(GetPartnerByAction, "sslThreadModel", true);
		REGISTERFUNC(GetPartnersByAction, "sslThreadModel", true);
		REGISTERFUNC(GetPartnerByTypeRev, "sslThreadModel", true);
		REGISTERFUNC(GetPartnersByTypeRev, "sslThreadModel", true);
		REGISTERFUNC(GetActionVelocity, "sslThreadModel", true);

		REGISTERFUNC(AddContextExImpl, "sslThreadModel", true);
		REGISTERFUNC(AddExperience, "sslThreadModel", true);
		REGISTERFUNC(UpdateStatistics, "sslThreadModel", true);

		return ActorAlias::Register(a_vm);
	}

}	 // namespace Papyrus::ThreadModel
