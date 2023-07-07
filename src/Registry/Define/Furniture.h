#pragma once

namespace Registry
{
	enum class FurnitureType
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
		CraftWorkbench = 1 << 9,

		Table = 1 << 10,
		TableCounter = 1 << 11,

		Chair = 1 << 12,			 // No arm, high back (Common Wooden chair)
		ChairBar = 1 << 13,		 // No Arm, no back
		ChairArm = 1 << 14,		 // Arm, low back
		ChairWing = 1 << 15,	 // Arm, high back
		ChairNoble = 1 << 16,	 // Noble Chair

		Bench = 1 << 17,
		BenchNoble = 1 << 18,

		Throne = 1 << 19,
		ThroneRiften = 1 << 20,
		ThroneNordic = 1 << 21,

		XCross = 1 << 22,
		Pillory = 1 << 23,
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
