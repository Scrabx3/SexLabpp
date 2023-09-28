#include "Furniture.h"

#include "Registry/Util/RayCast.h"
#include "Registry/Util/RayCast/ObjectBound.h"

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
		if (a_reference->HasKeyword(GameForms::FurnitureBedRoll)) {
			return BedType::BedRoll;
		}
		if (std::string name{ a_reference->GetName() }; name.empty() || AsLower(name).find("bed") == std::string::npos)
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
			return BedType::Single;
		default:
			return BedType::Double;
		}
	}

	bool BedHandler::IsBed(const RE::TESObjectREFR* a_reference)
	{
		return GetBedType(a_reference) != BedType::None;
	}

	FurnitureType FurnitureHandler::GetFurnitureType(RE::TESObjectREFR* a_ref)
	{
		using BenchType = RE::TESFurniture::WorkBenchData::BenchType;
		if (!a_ref->HasCollision()) {
			return FurnitureType::None;
		}
		const auto base = a_ref->GetBaseObject();
		if (!base)
			return FurnitureType::None;

		switch (base->formID) {
		case 0x0005'2FF5:	 // WallLeanmarker
			return FurnitureType::Wall;
		case 0x0007'0EA1:	 // RailLeanMarker
			return FurnitureType::Railing;
		case 0x00016'B691:
			return FurnitureType::ChairNoble;
		case 0x0007'4EC6:
			return FurnitureType::ChairBar;
		case 0x0003'6340:
			return FurnitureType::ChairWood;
		case 0x0000'CA02:
			return FurnitureType::BenchNoble;
		case 0x0002'67D3:	 // Nordic Throne
		case 0x0009'85C2:	 // Katariah Ship
			return FurnitureType::ThroneNordic;
		case 0x000F'507A:	 // CounterBarLeanMarker
		case 0x0006'CF36:	 // CounterLeanMarker
			return FurnitureType::TableCounter;
		case 0x000C'4EF1:
			return FurnitureType::Table;
		}

		switch (BedHandler::GetBedType(a_ref)) {
		case BedHandler::BedType::BedRoll:
			return FurnitureType::BedRoll;
		case BedHandler::BedType::Double:
			return FurnitureType::BedDouble;
		case BedHandler::BedType::Single:
			return FurnitureType::BedSingle;
		}

		const auto name = AsLower<std::string>({ a_ref->GetName() });
		const auto furniture = base->As<RE::TESFurniture>();
		if (furniture) {
			if (name.find("chair") != std::string::npos) {
				const auto model = AsLower<std::string>({ furniture->model.c_str() });
				if (model.find("dlc2darkelfchair01") != std::string::npos ||
						model.find("dwefurniturechair01") != std::string::npos ||
						model.find("dwefurniturehighchair01") != std::string::npos ||
						model.find("hhfurniturechair02.nif") != std::string::npos) {
					return FurnitureType::Chair;
				}
				if (model.find("commonchair") != std::string::npos ||
						model.find("upperchair01") != std::string::npos ||
						model.find("orcchair01") != std::string::npos) {
					return FurnitureType::ChairCommon;
				}
				FurnitureType::ChairMisc;
			}

			if (name.find("throne")) {
				const auto model = AsLower<std::string>({ furniture->model.c_str() });
				if (model.find("throneriften")) {
					return FurnitureType::ThroneRiften;
				}
				return FurnitureType::Throne;
			}

			if (name.find("bench") != std::string::npos) {
				const auto root = a_ref->Get3D();
				const auto extra = root ? root->GetExtraData("FRN") : nullptr;
				const auto node = extra ? netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra) : nullptr;
				if (node) {
					size_t count = 0;
					for (auto&& marker : node->markers) {
						if (marker.animationType.all(RE::BSFurnitureMarker::AnimationType::kSit)) {
							if (count++) {
								return FurnitureType::Bench;
							}
						}
					}
				}
				// A bench with only 1 sit node or no 3d (fallback to unspecified chair)
				return FurnitureType::ChairMisc;
			}

			switch (furniture->workBenchData.benchType.get()) {
			case BenchType::kSmithingWeapon:
				return FurnitureType::CraftGrindstone;
			case BenchType::kSmithingArmor:
				return FurnitureType::CraftWorkbench;
			case BenchType::kAlchemy:
				return FurnitureType::CraftAlchemy;
			case BenchType::kEnchanting:
				return FurnitureType::CraftEnchanting;
			case BenchType::kCreateObject:
				if (name.find("cooking") != std::string::npos) {
					return FurnitureType::CraftCookingPot;
				} else if (name.find("forge") != std::string::npos) {
					return FurnitureType::CraftSmithing;
				} else if (name.find("anvil") != std::string::npos) {
					return FurnitureType::CraftAnvil;
				}
				break;
			}
		}

		// TODO: consider static tables and chairs
		// TODO: BDSM furniture

		return FurnitureType::None;
	}

	std::vector<std::pair<FurnitureType, Coordinate>> FurnitureDetails::GetClosestCoordinateInBound(
		RE::TESObjectREFR* a_ref,
		const RE::TESObjectREFR* a_center,
		stl::enumeration<FurnitureType> a_filtertypes) const
	{
		const auto niobj = a_ref->Get3D();
		const auto ninode = niobj ? niobj->AsNode() : nullptr;
		if (!ninode)
			return {};
		const auto boundingbox = MakeBoundingBox(ninode);
		if (!boundingbox)
			return {};
		auto centerstart = boundingbox->GetCenterWorld();
		centerstart.z = boundingbox->worldBoundMax.z;

		const glm::vec4 tracestart{
			centerstart.x,
			centerstart.y,
			centerstart.z,
			0.0f
		};
		const auto basecoords = Coordinate(a_ref);
		const auto centercoords = Coordinate(a_center);

		std::vector<std::pair<FurnitureType, Coordinate>> ret{};
		for (auto&& [type, offsetlist] : _data) {
			if (!a_filtertypes.any(type)) {
				continue;
			}

			float distance = std::numeric_limits<float>::max();
			Coordinate distance_coords;

			for (auto&& offset : offsetlist) {
				// Get location for current coordinates
				// Add some units height to it so we hover above ground to avoid hitting a rug or gold coin
				auto coords = basecoords;
				offset.Apply(coords);
				coords.location.z += 16.0f;
				const glm::vec4 traceend_A{ coords.location, 0.0f };
				// Check if path to offset is free, if not we likely hit some static, like a wall, or some actor
				const auto resA = Raycast::hkpCastRay(tracestart, traceend_A, { a_ref, a_center });
				if (glm::distance(resA.hitPos, traceend_A) > 16.0) {
					continue;
				}
				// 2nd cast to see if there is no ceiling above either
				auto traceend_B = traceend_A;
				traceend_B.z += 128.0f;
				const auto resB = Raycast::hkpCastRay(traceend_A, traceend_B, { a_ref, a_center });
				if (resB.hit) {
					continue;
				}
				if (const auto d = glm::distance(coords.location, centercoords.location); d < distance) {
					distance_coords = coords;
					distance = d;
				}
			}
			if (distance != std::numeric_limits<float>::max()) {
				ret.emplace_back(type, distance_coords);
			}
		}
		return ret;
	}

}	 // namespace Registry
