#pragma once

#include "Registry/Animation.h"
#include "Registry/Expression.h"
#include "Registry/Voice.h"

namespace Thread
{
	class Instance
	{
	public:
		Instance(RE::TESQuest* a_linkedQst);
		~Instance() = default;

		void SetActiveScene(Registry::Scene* a_scene);
		Registry::Scene* GetActiveScene() const;
		Registry::Stage* GetActiveStage() const;
		std::vector<Registry::Scene*> GetThreadScenes() const;

		std::vector<RE::Actor*> GetActors() const;
		Registry::PositionInfo* GetPositionInfo(RE::Actor* a_actor) const;

		RE::TESObjectREFR* GetCenterRef() const;
		Registry::FurnitureType GetFurnitureType() const;

		bool GetAutoplayEnabled() const;
		void SetAutoplayEnabled(bool a_enabled);

		bool GetAnimationPaused() const;
		int32_t GetAnimationSpeed() const;
		void SetAnimationPaused(bool a_paused);
		void SetAnimationSpeed(int32_t a_speed);

		const Registry::Expression::Profile* GetExpression(RE::Actor* a_position) const;
		void SetExpression(RE::Actor* a_position, const Registry::Expression::Profile* a_expression);
		const Registry::Voice::VoiceObject* GetVoice(RE::Actor* a_position) const;
		void SetVoice(RE::Actor* a_position, const Registry::Voice::VoiceObject* a_voice);
		bool IsGhostMode(RE::Actor* a_position) const;
		void SetGhostMode(RE::Actor* a_position, bool a_ghostMode);
		int32_t GetUniquePermutations(RE::Actor* a_position) const;
		int32_t GetCurrentPermutation(RE::Actor* a_position) const;
		void SetNextPermutation(RE::Actor* a_position);

		void RequstMoveScene();
		void RequestEndScene();

	private:
		RE::TESQuest* linkedQst{ nullptr };
	};

}	 // namespace Thread
