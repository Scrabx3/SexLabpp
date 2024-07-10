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
		Position(std::ifstream& a_stream, uint8_t a_version);
		~Position() = default;

		void Save(YAML::Node& a_node) const;
		void Load(const YAML::Node& a_node);

	public:
		RE::BSFixedString event;

		bool climax;
		Transform offset;
		stl::enumeration<StripData> strips;
		int8_t schlong;
	};

	struct Stage
	{
	public:
		Stage(std::ifstream& a_stream, uint8_t a_version);
		~Stage() = default;

		void Save(YAML::Node& a_node) const;
		void Load(const YAML::Node& a_node);

	public:
		std::string id;
		std::vector<Position> positions;

		float fixedlength;
		std::string navtext;
		TagData tags;
	};

	struct PositionInfo
	{
		enum class MatchStrictness
		{
			Light,
			Standard,
			Strict,

			Total
		};

		enum class Extra : uint8_t
		{
			Submissive = 1 << 0,
			Vamprie = 1 << 1,
			Unconscious = 1 << 2
		};

	public:
		PositionInfo(std::ifstream& a_stream, uint8_t a_version);
		~PositionInfo() = default;

		_NODISCARD bool IsHuman() const { return race == RaceKey::Human; }
		_NODISCARD bool IsMale() const { return sex.all(Sex::Male); }
		_NODISCARD bool IsFemale() const { return sex.all(Sex::Female); }
		_NODISCARD bool IsFuta() const { return sex.all(Sex::Futa); }
		_NODISCARD PapyrusSex GetSexPapyrus() const;

		_NODISCARD bool IsSubmissive() const { return extra.all(Extra::Submissive); }

		_NODISCARD bool CanFillPosition(RE::Actor* a_actor) const;
		_NODISCARD bool CanFillPosition(const PositionInfo& a_other) const;
		_NODISCARD bool CanFillPosition(stl::enumeration<PositionFragment> a_fragment, MatchStrictness a_strictness) const;
		_NODISCARD std::vector<PositionFragment> MakeFragments() const;

		_NODISCARD bool HasExtraCstm(const RE::BSFixedString& a_extra) const;
		_NODISCARD std::string ConcatExtraCstm() const;

	public:
		RaceKey race;
		stl::enumeration<Sex> sex;
		stl::enumeration<Extra> extra;
		std::vector<RE::BSFixedString> custom;

		float scale;
	};

	class Scene
	{
	public:
		using FragmentPair = std::vector<std::pair<RE::Actor*, Registry::PositionFragment>>;

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
			Coordinate offset{};

		public:
			stl::enumeration<FurnitureType> GetCompatibleFurnitures() const;
		};

	public:
		Scene(std::ifstream& a_stream, std::string_view a_hash, uint8_t a_version);
		~Scene() = default;

		_NODISCARD bool IsEnabled() const;
		_NODISCARD bool IsPrivate() const;
		_NODISCARD bool HasCreatures() const;
		_NODISCARD bool UsesFurniture() const;
		_NODISCARD RE::BSFixedString GetPackageHash() const;

		_NODISCARD bool IsCompatibleTags(const TagData& a_tags) const;
		_NODISCARD bool IsCompatibleTags(const TagDetails& a_details) const;
		_NODISCARD bool IsCompatibleFurniture(FurnitureType a_furniture) const;
		_NODISCARD bool IsCompatibleFurniture(const FurnitureDetails* a_details) const;
		_NODISCARD bool IsCompatibleFurniture(const RE::TESObjectREFR* a_reference) const;

		_NODISCARD uint32_t CountPositions() const;
		_NODISCARD uint32_t CountSubmissives() const;
		_NODISCARD const PositionInfo* GetNthPosition(size_t n) const;

		_NODISCARD std::vector<std::vector<PositionFragment>> MakeFragments() const;
		_NODISCARD std::optional<std::vector<RE::Actor*>> SortActors(const FragmentPair& a_positions, PositionInfo::MatchStrictness a_strictness) const;

		_NODISCARD size_t GetNumStages() const;
		_NODISCARD const std::vector<const Stage*> GetAllStages() const;
		_NODISCARD Stage* GetStageByKey(const RE::BSFixedString& a_stage);
		_NODISCARD const Stage* GetStageByKey(const RE::BSFixedString& a_stage) const;
		_NODISCARD std::vector<const Stage*> GetLongestPath(const Stage* a_src) const;
		_NODISCARD std::vector<const Stage*> GetShortestPath(const Stage* a_src) const;
		void ForEachStage(std::function<bool(Stage*)> a_visitor);

		_NODISCARD NodeType GetStageNodeType(const Stage* a_stage) const;
		_NODISCARD std::vector<const Stage*> GetEndingStages() const;
		_NODISCARD std::vector<const Stage*> GetClimaxStages() const;
		_NODISCARD std::vector<const Stage*> GetFixedLengthStages() const;
		_NODISCARD size_t GetNumAdjacentStages(const Stage* a_stage) const;
		_NODISCARD const Stage* GetNthAdjacentStage(const Stage* a_stage, size_t n) const;
		_NODISCARD const std::vector<const Stage*>* GetAdjacentStages(const Stage* a_stage) const;
		_NODISCARD RE::BSFixedString GetNthAnimationEvent(const Stage* a_stage, size_t n) const;
		_NODISCARD std::vector<RE::BSFixedString> GetAnimationEvents(const Stage* a_stage) const;

		void Save(YAML::Node& a_node) const;
		void Load(const YAML::Node& a_node);

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
		Stage* start_animation;
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
