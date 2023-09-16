#pragma once

#include "Define/Fragment.h"
#include "Define/Furniture.h"
#include "Define/RaceKey.h"
#include "Define/Sex.h"
#include "Define/Tags.h"
#include "Define/Transform.h"

namespace Registry
{
	struct Position
	{
		enum class StripData : uint8_t
		{
			None = 0,
			Helmet = 1 << 0,
			Gloves = 1 << 1,
			Boots = 1 << 2,
			// Unused = 1 << 3,
			// Unused = 1 << 4,
			// Unused = 1 << 5,
			// Unused = 1 << 6,
			Default = 1 << 7,

			All = static_cast<std::underlying_type_t<StripData>>(-1),
		};

	public:
		Position(std::ifstream& a_stream);
		~Position() = default;

	public:
		RE::BSFixedString event;

		bool climax;
		Transform offset;
		stl::enumeration<StripData, uint8_t> strips;
	};

	struct Stage
	{
	public:
		Stage(std::ifstream& a_stream);
		~Stage() = default;

	public:
		std::string id;
		std::vector<Position> positions;

		float fixedlength;
		std::string navtext;
		TagData tags;
	};

	struct PositionInfo
	{
		enum class Extra : uint8_t
		{
			Submissive = 1 << 0,
			Optional = 1 << 1,
			Vamprie = 1 << 2,
			Unconscious = 1 << 3,

			Yoke = 1 << 4,
			Armbinder = 1 << 5,
			Legbinder = 1 << 6,
		};

	public:
		PositionInfo(std::ifstream& a_stream);
		~PositionInfo() = default;

		_NODISCARD bool IsHuman() const { return race == RaceKey::Human; }
		_NODISCARD bool IsMale() const { return sex.all(Sex::Male); }
		_NODISCARD bool IsFemale() const { return sex.all(Sex::Female); }
		_NODISCARD bool IsFuta() const { return sex.all(Sex::Futa); }
		_NODISCARD PapyrusSex GetSexPapyrus() const;

		_NODISCARD bool IsSubmissive() const { return extra.all(Extra::Submissive); }
		_NODISCARD bool IsOptional() const { return extra.all(Extra::Optional); }

		_NODISCARD bool CanFillPosition(RE::Actor* a_actor) const;
		_NODISCARD bool CanFillPosition(const PositionInfo& a_other) const;
		_NODISCARD bool CanFillPosition(PositionFragment a_fragment) const;
		_NODISCARD std::vector<PositionFragment> MakeFragments() const;

	public:
		RaceKey race;
		stl::enumeration<Sex> sex;
		stl::enumeration<Extra> extra;

		float scale;
	};

	class Scene
	{
	public:
		enum class NodeType
		{
			None = -1,

			Root = 0,
			Default = 1,
			Sink = 2,
		};

		struct FurnitureData
		{
			stl::enumeration<FurnitureType> furnitures{ FurnitureType::None };
			bool allowbed{ false };
			std::array<float, Offset::Total> offset{ 0, 0, 0, 0 };
		};

	public:
		Scene(std::ifstream& a_stream, std::string_view a_hash);
		~Scene() = default;

		_NODISCARD bool IsEnabled() const;
		_NODISCARD bool IsPrivate() const;
		_NODISCARD bool HasCreatures() const;
		_NODISCARD bool UsesFurniture() const;

		_NODISCARD bool IsCompatibleTags(const TagData& a_tags) const;
		_NODISCARD bool IsCompatibleTags(const TagDetails& a_details) const;
		_NODISCARD bool IsCompatibleFurniture(FurnitureType a_furniture) const;
		_NODISCARD bool IsCompatibleFurniture(RE::TESObjectREFR* a_reference) const;

		_NODISCARD uint32_t CountPositions() const;
		_NODISCARD uint32_t CountOptionalPositions() const;
		_NODISCARD uint32_t CountSubmissives() const;

		_NODISCARD std::vector<std::vector<PositionFragment>> MakeFragments() const;
		_NODISCARD std::optional<std::vector<RE::Actor*>> SortActors(const std::vector<RE::Actor*>& a_positions, bool a_withfallback) const;
		_NODISCARD std::optional<std::vector<RE::Actor*>> SortActors(const std::vector<std::pair<RE::Actor*, Registry::PositionFragment>>& a_positions) const;
		_NODISCARD std::optional<std::vector<RE::Actor*>> SortActorsFB(std::vector<std::pair<RE::Actor*, Registry::PositionFragment>> a_positions) const;

		_NODISCARD const Stage* GetStageByKey(const RE::BSFixedString& a_stage) const;
		_NODISCARD std::vector<const Stage*> GetLongestPath(const Stage* a_src) const;
		_NODISCARD std::vector<const Stage*> GetShortestPath(const Stage* a_src) const;

		_NODISCARD NodeType GetStageNodeType(const Stage* a_stage) const;
		_NODISCARD std::vector<const Stage*> GetClimaxStages() const;
		_NODISCARD std::vector<const Stage*> GetFixedLengthStages() const;
		_NODISCARD size_t GetNumLinkedStages(const Stage* a_stage) const;
		_NODISCARD const Stage* GetNthLinkedStage(const Stage* a_stage, size_t n) const;
		_NODISCARD RE::BSFixedString GetNthAnimationEvent(const Stage* a_stage, size_t n) const;
		_NODISCARD std::vector<RE::BSFixedString> GetAnimationEvents(const Stage* a_stage) const;

	public:
		// If the animation only includes humans, with specified amount of males and females
		_NODISCARD bool Legacy_IsCompatibleSexCount(int32_t a_males, int32_t a_females) const;
		_NODISCARD bool Legacy_IsCompatibleSexCountCrt(int32_t a_males, int32_t a_females) const;

	public:
		std::string id;
		std::string name;

		std::vector<PositionInfo> positions;
		FurnitureData furnitures;
		TagData tags;

		bool enabled;

	private:
		std::string_view hash;
		bool is_private;

		std::vector<std::unique_ptr<Stage>> stages;
		std::map<const Stage*, std::vector<const Stage*>> graph;
		const Stage* start_animation;
	};

	class AnimPackage
	{
	public:
		AnimPackage(const fs::path a_file);
		~AnimPackage() = default;

		RE::BSFixedString GetName() const { return name; }
		RE::BSFixedString GetAuthor() const { return author; }
		std::string_view GetHash() const { return hash; }

	public:
		std::vector<std::unique_ptr<Scene>> scenes;

	private:
		RE::BSFixedString name;
		RE::BSFixedString author;
		std::string hash;
	};

}	 // namespace Registry
