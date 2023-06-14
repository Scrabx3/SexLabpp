#pragma once

#include "Define/RaceKey.h"
#include "Define/Tags.h"


namespace Registry
{
	enum Offset : uint8_t
	{
		X = 0,
		Y,
		Z,
		R,

		Total
	};

	enum class Sex : uint8_t
	{
		Male = 1 << 0,
		Female = 1 << 1,
		Futa = 1 << 2,

		None = static_cast<std::underlying_type_t<Sex>>(-1),
	};
	Sex GetSex(RE::Actor* a_actor);

	enum class PositionFragment
	{
		None = 0,

		Male = 1 << 0,
		Female = 1 << 1,
		Futa = (Male | Female),

		Human = 1 << 2,
		Vampire = 1 << 3,
		Yoke = 1 << 4,
		Arminder = 1 << 5,
		HandShackle = (Yoke | Arminder),
		LegsBound = 1 << 6,
		Petsuit = 1 << 7,
		// Unused = 1 << 8,

		CrtBit0 = 1 << 3,
		CrtBit1 = 1 << 4,
		CrtBit2 = 1 << 5,
		CrtBit3 = 1 << 6,
		CrtBit4 = 1 << 7,
		CrtBit5 = 1 << 8,

		Submissive = 1 << 9,
		Unconscious = 1 << 10,
	};
	static inline constexpr size_t PositionFragmentSize = 11;
	using FragmentUnderlying = std::underlying_type<PositionFragment>::type;
	using PositionFragmentation = stl::enumeration<PositionFragment, FragmentUnderlying>;
	stl::enumeration<PositionFragment, FragmentUnderlying> MakePositionFragment(RE::Actor* a_actor, bool a_submissive);

	enum class PositionHeader
	{
		None = 0,

		AllowBed = 1ULL << 0,
		// Unused = 1ULL << 61,
		// Unused = 1ULL << 62,
		// Unused = 1ULL << 63,
	};
	static inline constexpr size_t PositionHeaderSize = 1;
	using FragmentHeaderUnderlying = std::underlying_type<PositionHeader>::type;

	struct Position
	{
		std::string event;

		bool climax;
		float offset[Offset::Total];
	};

	struct Stage
	{
		std::string id;
		std::vector<Position> positions;

		float fixedlength;
		std::string navtext;
		stl::enumeration<Tag, uint64_t> tags;
		std::vector<std::string> extratags;
	};

	struct PositionInfo
	{
		enum class Extra : uint8_t
		{
			Submissive = 1 << 0,
			Optional = 1 << 1,
			Vamprie = 1 << 2,
			Unconscious = 1 << 3,
		};

	public:
		_NODISCARD bool CanFillPosition(RE::Actor* a_actor) const;
		_NODISCARD bool CanFillPosition(PositionFragmentation a_fragment) const;
		_NODISCARD std::vector<PositionFragmentation> MakeFragments() const;

	public:
		RaceKey race;
		stl::enumeration<Sex, uint8_t> sex;
		stl::enumeration<Extra, uint8_t> extra;

		float scale;
	};

	struct FurnitureInfo
	{
		bool allowbed;
		float offset[Offset::Total];
	};

	class Scene
	{
		friend class Decoder;

	public:
		Scene(const std::string_view a_author, const std::string_view a_hash) :
			author(a_author), hash(a_hash), start_animation(nullptr), furnituredata({}) {}
		~Scene() = default;

		_NODISCARD Stage* GetStageByKey(std::string_view a_key) const;
		_NODISCARD std::vector<std::vector<PositionFragmentation>> GetFragmentations() const;

	public:
		std::string id;
		std::string name;
		std::string_view author;
		std::string_view hash;

		std::vector<PositionInfo> positions;
		std::vector<std::pair<Stage*, std::forward_list<Stage*>>> graph;
		Stage* start_animation;

		FurnitureInfo furnituredata;
		stl::enumeration<Tag, uint64_t> tags;
		std::vector<std::string_view> extratags;

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
