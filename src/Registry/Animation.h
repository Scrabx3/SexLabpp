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
	};

	/// @brief A position within a single stage
	struct Position
	{
		std::string event;

		bool climax;
		float offset[Offset::Total];
	};

	/// @brief A node within a scene graph
	struct Stage
	{
		std::string id;
		std::vector<Position> positions;

		float fixedlength;
		std::string navtext;
		stl::enumeration<Tag, uint64_t> tags;
		std::vector<std::string> extratags;
	};

	/// @brief Shared position info among all stages over a scene
	struct PositionInfo
	{
		enum class Extra : uint8_t
		{
			Submissive = 1 << 0,
			Optional = 1 << 1,
			Vamprie = 1 << 2,
			Unconscious = 1 << 3,
		};

		RaceKey race;
		stl::enumeration<Sex, uint8_t> sex;
		stl::enumeration<Extra, uint8_t> extra;

		float scale;
	};

	/// @brief Furniture related data for invis furniture & bed
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
			author(a_author), hash(a_hash) {}
		~Scene() = default;

		Stage* GetStageByKey(std::string_view a_key) const;

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
