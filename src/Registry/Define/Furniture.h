#pragma once

#include "Transform.h"

namespace Registry
{
	struct FurnitureType
	{
		enum Value : uint32_t
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

			Bench = 1 << 20,			 // With back
			BenchNoble = 1 << 21,	 // Noble Bench (no back, with arm)
			BenchMisc = 1 << 20,	 // No specification on back or arm

			Throne = 1 << 22,
			ThroneRiften = 1 << 23,
			ThroneNordic = 1 << 24,

			XCross = 1 << 25,
			Pillory = 1 << 26,
		};

	public:
		constexpr FurnitureType() = default;
		constexpr FurnitureType(Value a_value) :
			value(a_value) {}
		FurnitureType(const RE::BSFixedString& a_value);

		RE::BSFixedString ToString() const;

		constexpr bool IsValid() const { return value != Value::None; }
		constexpr bool Is(FurnitureType::Value a_value) const { return value == a_value; }

	public:
		constexpr bool operator==(const FurnitureType& a_rhs) const { return value == a_rhs.value; }
		constexpr bool operator!=(const FurnitureType& a_rhs) const { return value != a_rhs.value; }
		constexpr bool operator<(const FurnitureType& a_rhs) const { return value < a_rhs.value; }

		constexpr operator Value() const { return value; }

	public:
		Value value{ Value::None };
	};

	class FurnitureDetails
	{
	public:
		FurnitureDetails(const YAML::Node& a_node);
		FurnitureDetails(FurnitureType a_type, Coordinate a_coordinate) :
			_data({ std::make_pair(a_type, std::vector{ a_coordinate }) }) {}
		~FurnitureDetails() = default;

		std::vector<std::pair<FurnitureType, std::vector<Coordinate>>> GetCoordinatesInBound(RE::TESObjectREFR* a_ref, REX::EnumSet<FurnitureType::Value> a_filter) const;
		std::vector<std::pair<FurnitureType, Coordinate>> GetClosestCoordinateInBound(
			RE::TESObjectREFR* a_ref, REX::EnumSet<FurnitureType::Value> a_filter, const RE::TESObjectREFR* a_center) const;

		template <typename T, typename S>
		bool HasType(T a_container, S a_projection) const
		{
			return std::ranges::any_of(_data, a_container, [a_projection](auto&& it) {
				FurnitureType type = a_projection(it);
				return HasType(type);
			});
		}
		bool HasType(FurnitureType::Value a_type) const
		{
			return std::ranges::contains(_data, a_type, [](auto&& it) { return it.first.value; });
		}

	private:
		std::vector<std::pair<FurnitureType, std::vector<Coordinate>>> _data;
	};

	struct BedHandler
	{
		/// @brief Get the reference's bed-furnituretype
		/// @param a_reference The reference to get the type from
		/// @return FurnitureType, the bed type of this object, or None if the reference is not a bed
		_NODISCARD static FurnitureType GetBedType(const RE::TESObjectREFR* a_reference);
		_NODISCARD static bool IsBed(const RE::TESObjectREFR* a_reference);
	};

}	 // namespace Registry
