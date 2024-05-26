#include "sslVoiceSlots.h"

#include "Registry/Voice.h"
#include "Registry/Library.h"

namespace Papyrus::VoiceSlots
{
	namespace BaseVoice
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
				const auto rk = Registry::RaceHandler::GetRaceKey(r);
				if (rk == Registry::RaceKey::None)
					continue;
				races.push_back(rk);
			}
			Registry::Voice::GetSingleton()->SetRace(a_id, races);
		}

	}	 // namespace BaseVoice

	RE::BSFixedString SelectVoice(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		return SelectVoiceByTags(nullptr, a_actor, "");
	}

	RE::BSFixedString SelectVoiceByTags(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_tags)
	{
		auto s = Registry::Voice::GetSingleton();
		auto v = a_actor ? s->GetVoice(a_actor, { a_tags }) : s->GetVoice(Registry::TagDetails{ a_tags });
		return v ? v->name : "";
	}

	RE::BSFixedString SelectVoiceByTagsA(RE::StaticFunctionTag*, RE::Actor* a_actor, std::vector<std::string_view> a_tags)
	{
		auto s = Registry::Voice::GetSingleton();
		auto v = a_actor ? s->GetVoice(a_actor, { a_tags }) : s->GetVoice({ a_tags });
		return v ? v->name : "";
	}

	RE::BSFixedString GetSavedVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return "";
		}
		auto v = Registry::Voice::GetSingleton()->GetSavedVoice(a_actor->GetFormID());
		return v ? v->name : "";
	}

	void StoreVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_voice)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Voice::GetSingleton()->SaveVoice(a_actor->GetFormID(), a_voice);
	}

	void DeleteVoice(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Voice::GetSingleton()->ClearVoice(a_actor->GetFormID());
	}

	std::vector<RE::BSFixedString> GetAllVoices(RE::StaticFunctionTag*)
	{
		return Registry::Voice::GetSingleton()->GetAllVoiceNames();
	}

	RE::BSFixedString SelectVoiceByRace(RE::StaticFunctionTag*, RE::BSFixedString a_racekey)
	{
		auto v = Registry::Voice::GetSingleton()->GetVoice(Registry::RaceHandler::GetRaceKey(a_racekey));
		return v ? v->name : "";
	}

}	 // namespace Papyrus::VoiceSlots
