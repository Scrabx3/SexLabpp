#pragma once

namespace Papyrus::ThreadModel
{
	enum FurniStatus
	{
		Disallow = 0,
		Allow = 1,
		Prefer = 2,
	};

	RE::TESObjectREFR* FindCenter(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst,
		const std::vector<RE::BSFixedString> a_scenes, RE::reference_array<RE::BSFixedString> a_out_scenes, RE::reference_array<float> a_out_coordinates, FurniStatus a_status);
	bool UpdateBaseCoordinates(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::BSFixedString a_sceneid, RE::reference_array<float> a_out);
	void ApplySceneOffset(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::BSFixedString a_sceneid, RE::reference_array<float> a_out);

	RE::BSFixedString PlaceAndPlay(VM* a_vm, StackID a_stackID, RE::TESQuest*,
		std::vector<RE::Actor*> a_positions, std::vector<float> a_coordinates, RE::BSFixedString a_scene, RE::BSFixedString a_stage);
	void RePlace(VM* a_vm, StackID a_stackID, RE::TESQuest*, 
		RE::Actor* a_position, std::vector<float> a_coordinates, RE::BSFixedString a_scene, RE::BSFixedString a_stage, int32_t n);

	std::vector<RE::BSFixedString> AddContextExImpl(RE::TESQuest*, std::vector<RE::BSFixedString> a_oldcontext, std::string a_newcontext);
	void ShuffleScenes(RE::TESQuest*, RE::reference_array<RE::BSFixedString> a_scenes, RE::BSFixedString a_tofront);

	bool IsPhysicsRegistered(RE::TESQuest* a_qst);
	void RegisterPhysics(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions, RE::BSFixedString a_activescene);
	void UnregisterPhysics(RE::TESQuest* a_qst);
	std::vector<int> GetPhysicTypes(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, RE::Actor* a_partner);
	bool HasPhysicType(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int a_type, RE::Actor* a_position, RE::Actor* a_partner);
	RE::Actor* GetPhysicPartnerByType(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type);
	std::vector<RE::Actor*> GetPhysicPartnersByType(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type);
	float GetPhysicVelocity(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, RE::Actor* a_partner, int a_type);

	void AddExperience(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions,	
		RE::BSFixedString a_scene, std::vector<RE::BSFixedString> a_playedstages);
	void UpdateStatistics(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_actor, std::vector<RE::Actor*> a_positions,
		RE::BSFixedString a_scene, std::vector<RE::BSFixedString> a_playedstages, float a_time);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(FindCenter, "sslThreadModel", true);
		REGISTERFUNC(UpdateBaseCoordinates, "sslThreadModel", true);
		REGISTERFUNC(ApplySceneOffset, "sslThreadModel", true);

		REGISTERFUNC(PlaceAndPlay, "sslThreadModel", false);
		REGISTERFUNC(RePlace, "sslThreadModel", false);

		REGISTERFUNC(AddContextExImpl, "sslThreadModel", true);
		REGISTERFUNC(ShuffleScenes, "sslThreadModel", true);

		REGISTERFUNC(IsPhysicsRegistered, "sslThreadModel", true);
		REGISTERFUNC(RegisterPhysics, "sslThreadModel", true);
		REGISTERFUNC(UnregisterPhysics, "sslThreadModel", true);
		REGISTERFUNC(GetPhysicTypes, "sslThreadModel", true);
		REGISTERFUNC(HasPhysicType, "sslThreadModel", true);
		REGISTERFUNC(GetPhysicPartnerByType, "sslThreadModel", true);
		REGISTERFUNC(GetPhysicPartnersByType, "sslThreadModel", true);
		REGISTERFUNC(GetPhysicVelocity, "sslThreadModel", true);

		REGISTERFUNC(AddExperience, "sslThreadModel", true);
		REGISTERFUNC(UpdateStatistics, "sslThreadModel", true);

		return true;
	}

}	 // namespace Papyrus::ThreadModel
