#pragma once

#include "Transform.h"

namespace Registry
{
	enum class FurnitureType : uint32_t
	{
		None = 0,

		BedRoll = 1 << 0,
		BedSingle = 1 << 1,
		BedDouble = 1 << 2,

		Wall = 1 << 3,
		Railing = 1 << 4,

		CraftCookingPot = 1 << 5,
		CraftAlchemy = 1 << 6,
		CraftEnchanting = 1 << 7,
		CraftSmithing = 1 << 8,
		CraftAnvil = 1 << 9,
		CraftWorkbench = 1 << 10,
		CraftGrindstone = 1 << 11,

		Table = 1 << 12,
		TableCounter = 1 << 13,

		Chair = 1 << 14,				// No arm, high back
		ChairCommon = 1 << 15,	// Common chair
		ChairWood = 1 << 16,		// Wooden Chair
		ChairBar = 1 << 17,			// Bar stool
		ChairNoble = 1 << 18,		// Noble Chair
		ChairMisc = 1 << 19,		// Unspecified

		Bench = 1 << 20,				// With back
		BenchNoble = 1 << 21,		// Noble Bench (no back, with arm)
		BenchMisc = 1 << 20,		// No specification on back or arm

		Throne = 1 << 22,
		ThroneRiften = 1 << 23,
		ThroneNordic = 1 << 24,

		XCross = 1 << 25,
		Pillory = 1 << 26,

		// Unused = 1 << 27,
		// Unused = 1 << 28,
		// Unused = 1 << 29,
		// Unused = 1 << 30,
		// Unused = 1 << 31,
	};

	class FurnitureDetails
	{
	public:
		/// @brief Find the closest (in bound) location for each furniture type associated with this model
		/// @param a_ref The reference to find the locations from
		/// @param a_center Some reference to derive distance from
		/// @return A list of pairs stating the furniture type and its associated closest in-bounds point
		std::vector<std::pair<FurnitureType, Coordinate>> GetClosestCoordinateInBound(RE::TESObjectREFR* a_ref, const RE::TESObjectREFR* a_center) const;

		template <typename T>
		bool HasType(T a_container) const
		{
			return HasType(T, std::identity);
		}
		template <typename T, typename S>
		bool HasType(T a_container, S a_projection) const
		{
			for (auto&& it : a_container) {
				FurnitureType type = a_projection(it);
				if (std::ranges::find_if(_data, [&](const auto& arg) { return arg.first == type; }) != _data.end())
					return true;
			}
			return false;
		}

	private:
		RE::BSFixedString _modelpath;
		std::vector<std::pair<FurnitureType, std::vector<Transform>>> _data;
	};

	struct BedHandler
  {
		enum class BedType
		{
			None = 0,
			BedRoll = 1,
			Single = 2,
			Double = 3,
		};

		_NODISCARD static std::vector<RE::TESObjectREFR*> GetBedsInArea(RE::TESObjectREFR* a_center, float a_radius, float a_radiusz);
		_NODISCARD static BedType GetBedType(const RE::TESObjectREFR* a_reference);
		_NODISCARD static bool IsBed(const RE::TESObjectREFR* a_reference);
	};

	struct FurnitureHandler
	{
		_NODISCARD static FurnitureType GetFurnitureType(RE::TESObjectREFR* a_ref);
	};

} // namespace Registry
