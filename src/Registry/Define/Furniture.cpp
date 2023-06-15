#include "Furniture.h"

namespace Registry
{
	BedHandler::BedType BedHandler::GetBedType(const RE::TESObjectREFR* a_reference)
	{
		if (a_reference->GetName()[0] == '\0')
			return BedType::None;
		const auto root = a_reference->Get3D();
		const auto extra = root ? root->GetExtraData("FRN") : nullptr;
		const auto node = extra ? netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra) : nullptr;
    if (!node)
      return BedType::None;

		size_t sleepmarkers = 0;
		for (auto&& marker : node->markers) {
			if (marker.animationType.all(RE::BSFurnitureMarker::AnimationType::kSleep)) {
				sleepmarkers += 1;
			}
		}
		switch (sleepmarkers) {
		case 0:
      return BedType::None;
		case 1:
			return a_reference->HasKeyword(GameForms::FurnitureBedRoll) ?
							 BedType::BedRoll :
							 BedType::Single;
		default:
      return BedType::Double;
		}
	}

	bool BedHandler::IsBed(const RE::TESObjectREFR* a_reference)
  {
		return GetBedType(a_reference) != BedType::None;
	}

} // namespace Registry
