#include "Furniture.h"

namespace Registry
{
	std::vector<RE::TESObjectREFR*> BedHandler::GetBedsInArea(RE::TESObjectREFR* a_center, float a_radius, float a_radiusz)
	{
		const auto center = a_center->GetPosition();
		std::vector<RE::TESObjectREFR*> ret{};
		const auto add = [&](RE::TESObjectREFR& ref) {
			if (!ref.GetBaseObject()->Is(RE::FormType::Furniture) && a_radiusz > 0.0f ? (std::fabs(center.z - ref.GetPosition().z) <= a_radiusz) : true)
				if (Registry::BedHandler::IsBed(&ref))
					ret.push_back(&ref);
			return RE::BSContainer::ForEachResult::kContinue;
		};
		const auto TES = RE::TES::GetSingleton();
		if (const auto interior = TES->interiorCell; interior) {
			interior->ForEachReferenceInRange(center, a_radius, add);
		} else if (const auto grids = TES->gridCells; grids) {
			// Derived from: https://github.com/powerof3/PapyrusExtenderSSE
			auto gridLength = grids->length;
			if (gridLength > 0) {
				float yPlus = center.y + a_radius;
				float yMinus = center.y - a_radius;
				float xPlus = center.x + a_radius;
				float xMinus = center.x - a_radius;
				for (uint32_t x = 0, y = 0; (x < gridLength && y < gridLength); x++, y++) {
					const auto gridcell = grids->GetCell(x, y);
					if (gridcell && gridcell->IsAttached()) {
						auto cellCoords = gridcell->GetCoordinates();
						if (!cellCoords)
							continue;
						float worldX = cellCoords->worldX;
						float worldY = cellCoords->worldY;
						if (worldX < xPlus && (worldX + 4096.0) > xMinus && worldY < yPlus && (worldY + 4096.0) > yMinus) {
							gridcell->ForEachReferenceInRange(center, a_radius, add);
						}
					}
				}
			}
			if (!ret.empty()) {
				std::sort(ret.begin(), ret.end(), [&](RE::TESObjectREFR* a_refA, RE::TESObjectREFR* a_refB) {
					return center.GetDistance(a_refA->GetPosition()) < center.GetDistance(a_refB->GetPosition());
				});
			}
		}
		return ret;
	}

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
