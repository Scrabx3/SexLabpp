#pragma once

#include "Registry/Library.h"

namespace Thread
{
	class Instance
	{
		constexpr static const char* CENTER_REF_NAME{ "CenterAlias" };

	public:
		enum class FurniturePreference
		{
			Disallow = 0,
			Allow = 1,
			Prefer = 2,
		};

		enum SceneType
		{
			Primary = 0,
			LeadIn,
			Custom,

			Total
		};

		using FurnitureMapping = std::vector<std::pair<RE::TESObjectREFR*, Registry::FurnitureOffset>>;
		using SceneMapping = std::array<std::vector<const Registry::Scene*>, SceneType::Total>;

	public:
		struct Position
		{
			Position(RE::BGSRefAlias* alias, RE::Actor* actor, bool submissive, bool dominant);

			RE::BGSRefAlias* alias;
			Registry::ActorFragment data;
			const Registry::Voice* voice{ nullptr };
			const Registry::Expression* expression{ nullptr };
			std::optional<float> ghostAlpha{ std::nullopt };
			uint8_t uniquePermutations{ 0 };
			uint8_t currentPermutation{ 0 };
		};

		struct Center
		{
			Center(RE::BGSRefAlias* alias) :
				alias(alias), ref(alias ? alias->GetReference() : nullptr) {}
			~Center() = default;

			void SetReference(RE::TESObjectREFR* a_ref, Registry::FurnitureOffset a_offset);

			RE::BGSRefAlias* alias;
			RE::TESObjectREFR* ref{ nullptr };
			Registry::FurnitureOffset offset{};
			const Registry::FurnitureDetails* details{ nullptr };
		};

	public:
		Instance(RE::TESQuest* a_linkedQst, const std::vector<RE::Actor*>& a_submissives, const SceneMapping& a_scenes, FurniturePreference a_furniturePreference);
		~Instance() = default;

		static bool CreateInstance(RE::TESQuest* a_linkedQst, const std::vector<RE::Actor*> a_submissives, const SceneMapping& a_scenes, FurniturePreference a_furniturePreference);
		static Instance* GetInstance(RE::TESQuest* a_linkedQst);

	public:
		void AdvanceScene(const Registry::Stage* a_nextStage);

		bool SetActiveScene(const Registry::Scene* a_scene);
		const Registry::Scene* GetActiveScene() { return activeScene; }
		const Registry::Stage* GetActiveStage() { return activeStage; }
		std::vector<const Registry::Scene*> GetThreadScenes(SceneType a_type);
		std::vector<const Registry::Scene*> GetThreadScenes();

		const std::vector<RE::Actor*>& GetActors();
		Position* GetPosition(RE::Actor* a_actor);
		const Registry::PositionInfo* GetPositionInfo(RE::Actor* a_actor);
		void UpdatePlacement(RE::Actor* a_actor);

		RE::TESObjectREFR* GetCenterRef() { return center.ref; }
		Registry::FurnitureType GetFurnitureType() { return center.offset.type; }
		bool ReplaceCenterRef(RE::TESObjectREFR* a_ref);

		bool GetAutoplayEnabled();
		void SetAutoplayEnabled(bool a_enabled);

		const Registry::Expression* GetExpression(RE::Actor* a_position);
		void SetExpression(RE::Actor* a_position, const Registry::Expression* a_expression);
		const Registry::Voice* GetVoice(RE::Actor* a_position);
		void SetVoice(RE::Actor* a_position, const Registry::Voice* a_voice);
		bool IsGhostMode(RE::Actor* a_position);
		void SetGhostMode(RE::Actor* a_position, bool a_ghostMode);
		int32_t GetUniquePermutations(RE::Actor* a_position);
		int32_t GetCurrentPermutation(RE::Actor* a_position);
		void SetNextPermutation(RE::Actor* a_position);

private:
		RE::TESQuest* linkedQst;

		Center center;
		std::vector<Position> positions;
		Registry::Coordinate baseCoordinates{};
		std::vector<RE::Actor*> activeAssignment{};
		std::vector<std::vector<RE::Actor*>> assignments{};
		const Registry::Scene* activeScene{ nullptr };
		const Registry::Stage* activeStage{ nullptr };
		SceneMapping scenes{};

	private:
		RE::Actor* InitializeReferences(const std::vector<RE::Actor*>& a_submissives);
		std::vector<Registry::ActorFragment> InitializeScenes(const SceneMapping& a_scenes, FurniturePreference a_furniturePreference);
		std::vector<const Registry::Scene*>& InitializeCenter(RE::Actor* centerAct, FurniturePreference furniturePreference);
		bool InitializeFixedCenter(RE::Actor* centerAct, std::vector<const Registry::Scene*>& prioScenes, REX::EnumSet<Registry::FurnitureType::Value> sceneTypes);
		bool IsMenuSelectionEnabled();
		FurnitureMapping::value_type SelectCenterRefMenu(const FurnitureMapping& a_furnitures, RE::Actor* a_tmpCenter);
		FurnitureMapping GetUniqueFurnituesOfTypeInBound(RE::Actor* a_centerAct, REX::EnumSet<Registry::FurnitureType::Value> a_furnitureTypes);

	private:
		static inline std::vector<std::unique_ptr<Instance>> instances{};
	};

}	 // namespace Thread
