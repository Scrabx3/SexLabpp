#pragma once

namespace Registry
{
  struct BedHandler
  {
		enum class BedType
		{
			None = 0,
			BedRoll = 1,
			Single = 2,
			Double = 3,
		};

		_NODISCARD static BedType GetBedType(const RE::TESObjectREFR* a_reference);
		_NODISCARD static bool IsBed(const RE::TESObjectREFR* a_reference);
	};
} // namespace Registry
