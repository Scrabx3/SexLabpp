#pragma once

#include "Define/Fragment.h"
#include "Define/Furniture.h"
#include "Define/RaceKey.h"
#include "Define/Sex.h"
#include "Define/Tags.h"

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

		RE::BSFixedString event;
		stl::enumeration<StripData, uint8_t> strips;
		bool climax;

		float offset[Offset::Total];
	};

	struct Stage
	{
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
		_NODISCARD bool CanFillPosition(RE::Actor* a_actor) const;
		_NODISCARD bool CanFillPosition(PositionFragment a_fragment) const;
		_NODISCARD std::vector<PositionFragment> MakeFragments() const;

	public:
		RaceKey race;
		stl::enumeration<Sex, uint8_t> sex;
		stl::enumeration<Extra, uint8_t> extra;

		float scale;
	};

	struct FurnitureData
	{
		stl::enumeration<FurnitureType, uint32_t> furnitures;

		bool allowbed;
		float offset[Offset::Total];
	};

	class Scene
	{
		friend class Decoder;

	public:
		Scene(const std::string_view a_author, const std::string_view a_hash) :
			author(a_author), hash(a_hash), start_animation(nullptr), furnitures({}), tags({}) {}
		~Scene() = default;

		_NODISCARD bool IsEnabled() const;
		_NODISCARD bool HasCreatures() const;
		_NODISCARD uint32_t GetSubmissiveCount() const;

		_NODISCARD std::vector<HeaderFragment> MakeHeaders() const;
		_NODISCARD std::vector<std::vector<PositionFragment>> MakeFragments() const;

		_NODISCARD const Stage* GetStageByKey(const RE::BSFixedString& a_key) const;

	public:
		std::string id;
		std::string name;
		std::string_view author;
		std::string_view hash;

		std::vector<PositionInfo> positions;
		std::vector<std::pair<Stage*, std::forward_list<Stage*>>> graph;
		Stage* start_animation;

		FurnitureData furnitures;
		TagData tags;

		bool is_private;
		bool enabled;

	private:
		std::vector<std::unique_ptr<Stage>> stages;
	};

	class AnimPackage
	{
	public:
		std::string name;
		std::string author;
		std::string hash;

		std::vector<std::unique_ptr<Scene>> scenes;
	};

}	 // namespace Registry
