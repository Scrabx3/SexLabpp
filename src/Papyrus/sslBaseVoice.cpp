#include "sslBaseVoice.h"

#include "Registry/Library.h"
#include "Registry/Voice.h"

namespace Papyrus::BaseVoice
{
	bool GetEnabled(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		const auto vs = Registry::Voice::GetSingleton();
		auto it = vs->GetVoice(a_id);
		if (!it) {
			a_vm->TraceStack("Invalid voice form", a_stackID);
			return false;
		}
		return it->enabled;
	}

	void SetEnabled(RE::StaticFunctionTag*, RE::BSFixedString a_id, bool a_enabled)
	{
		const auto vs = Registry::Voice::GetSingleton();
		vs->SetVoiceObjectEnabled(a_id, a_enabled);
	}

	std::vector<RE::BSFixedString> GetVoiceTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		const auto vs = Registry::Voice::GetSingleton();
		auto it = vs->GetVoice(a_id);
		if (!it) {
			a_vm->TraceStack("Invalid voice form", a_stackID);
			return {};
		}
		return it->tags.AsVector();
	}

	int GetCompatibleSex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		const auto vs = Registry::Voice::GetSingleton();
		auto it = vs->GetVoice(a_id);
		if (!it) {
			a_vm->TraceStack("Invalid voice form", a_stackID);
			return {};
		}
		return it->sex == RE::SEXES::kNone ? -1 : it->sex;
	}

	std::vector<RE::BSFixedString> GetCompatibleRaces(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		const auto vs = Registry::Voice::GetSingleton();
		auto it = vs->GetVoice(a_id);
		if (!it) {
			a_vm->TraceStack("Invalid voice form", a_stackID);
			return {};
		}
		std::vector<RE::BSFixedString> ret{};
		for (auto&& r : it->races) {
			ret.push_back(Registry::RaceHandler::AsString(r));
		}
		return ret;
	}

	RE::TESSound* GetSoundObject(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_strength,
		std::vector<RE::BSFixedString> a_context, RE::BSFixedString a_scene, RE::BSFixedString a_stage, int a_positionidx)
	{
		const auto lib = Registry::Library::GetSingleton();
		auto scene = lib->GetSceneByID(a_scene);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return nullptr;
		}
		if (scene->CountPositions() <= static_cast<uint32_t>(a_positionidx)) {
			a_vm->TraceStack("Invalid position idx", a_stackID);
			return nullptr;
		}
		auto stage = scene->GetStageByKey(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return nullptr;
		}
		auto p = scene->GetNthPosition(a_positionidx);
		const auto vs = Registry::Voice::GetSingleton();
		return vs->PickSound(a_id, static_cast<uint32_t>(a_strength), stage, p, a_context);
	}

	RE::TESSound* GetSoundObjectLeg(RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_idx)
	{
		return Registry::Voice::GetSingleton()->PickSound(a_id, Registry::LegacyVoice(a_idx));
	}

	bool InitializeVoiceObject(RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		return Registry::Voice::GetSingleton()->InitializeNew(a_id);
	}

	void FinalizeVoiceObject(RE::StaticFunctionTag*, RE::BSFixedString a_id)
	{
		Registry::Voice::GetSingleton()->SaveYAML(a_id);
	}

	void SetSoundObjectLeg(RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_idx, RE::TESSound* a_set)
	{
		Registry::Voice::GetSingleton()->SetSound(a_id, Registry::LegacyVoice(a_idx), a_set);
	}

	void SetVoiceTags(RE::StaticFunctionTag*, RE::BSFixedString a_id, std::vector<RE::BSFixedString> a_newtags)
	{
		Registry::Voice::GetSingleton()->SetTags(a_id, a_newtags);
	}

	void SetCompatibleSex(RE::StaticFunctionTag*, RE::BSFixedString a_id, int a_set)
	{
		Registry::Voice::GetSingleton()->SetSex(a_id, a_set == -1 ? RE::SEXES::kNone : RE::SEXES::SEX(a_set));
	}

	void SetCompatibleRaces(RE::StaticFunctionTag*, RE::BSFixedString a_id, std::vector<RE::BSFixedString> a_set)
	{
		std::vector<Registry::RaceKey> races{};
		races.reserve(a_set.size());
		for (auto&& r : a_set) {
			races.push_back(Registry::RaceHandler::GetRaceKey(r));
		}
		Registry::Voice::GetSingleton()->SetRace(a_id, races);
	}


}	 // namespace Papyrus::BaseVoice
