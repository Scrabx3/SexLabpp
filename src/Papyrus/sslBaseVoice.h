#pragma once

namespace Papyrus::BaseVoice
{
	bool GetEnabled(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	void SetEnabled(RE::StaticFunctionTag*, RE::BSFixedString a_id, bool a_enabled);

	std::vector<RE::BSFixedString> GetVoiceTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	int GetCompatibleSex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);
	std::vector<RE::BSFixedString> GetCompatibleRaces(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id);

	RE::TESSound* GetSoundObject(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_strength,
		std::vector<RE::BSFixedString> a_context, RE::BSFixedString a_scene, RE::BSFixedString a_stage, int a_positionidx);
	RE::TESSound* GetSoundObjectLeg(RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_idx);

	bool InitializeVoiceObject(RE::StaticFunctionTag*, RE::BSFixedString a_id);
	void FinalizeVoiceObject(RE::StaticFunctionTag*, RE::BSFixedString a_id);

	void SetSoundObjectLeg(RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_idx, RE::TESSound* a_set);
	void SetVoiceTags(RE::StaticFunctionTag*, RE::BSFixedString a_id, std::vector<RE::BSFixedString> a_newtags);
	void SetCompatibleSex(RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_set);
	void SetCompatibleRaces(RE::StaticFunctionTag*, RE::BSFixedString a_id, std::vector<RE::BSFixedString> a_set);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetEnabled, "sslBaseVoice", true);
		REGISTERFUNC(SetEnabled, "sslBaseVoice", true);
		REGISTERFUNC(GetVoiceTags, "sslBaseVoice", true);
		REGISTERFUNC(GetCompatibleSex, "sslBaseVoice", true);
		REGISTERFUNC(GetCompatibleRaces, "sslBaseVoice", true);
		REGISTERFUNC(GetSoundObject, "sslBaseVoice", true);
		REGISTERFUNC(GetSoundObjectLeg, "sslBaseVoice", true);
		REGISTERFUNC(InitializeVoiceObject, "sslBaseVoice", true);
		REGISTERFUNC(FinalizeVoiceObject, "sslBaseVoice", true);
		REGISTERFUNC(SetSoundObjectLeg, "sslBaseVoice", true);
		REGISTERFUNC(SetVoiceTags, "sslBaseVoice", true);
		REGISTERFUNC(SetCompatibleSex, "sslBaseVoice", true);
		REGISTERFUNC(SetCompatibleRaces, "sslBaseVoice", true);

		return true;
	}

}	 // namespace Papyrus::BaseExpression
