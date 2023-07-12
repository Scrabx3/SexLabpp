#include "sslThreadModel.h"

#include "Registry/Animation.h"
#include "Registry/Define/Furniture.h"
#include "Registry/Library.h"

namespace Papyrus::ThreadModel
{
	RE::TESObjectREFR* FindCenter(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst,
		const std::vector<RE::BSFixedString> a_scenes,
		const RE::BSFixedString a_preferedscene,
		std::vector<RE::BSFixedString> a_out,
    FurniStatus a_status)
	{
		if (!a_qst) {
			a_vm->TraceStack("Cannot call GetSetCenterObject on a none object", a_stackID);
			return nullptr;
		} else if (a_out.empty()) {
			a_vm->TraceStack("Out array should have at least 1 place allocated", a_stackID);
			return nullptr;
		}
		std::vector<const Registry::Scene*> scenes_furni{};
		std::vector<const Registry::Scene*> scenes_none{};
		{ // get scene objects from argument ids
			const auto lib = Registry::Library::GetSingleton();
			auto get = [&](const RE::BSFixedString& a_id) mutable {
				if (a_id.empty())
					return;
				if (const auto scene = lib->GetSceneByID(a_id)) {
					if (scene->furnitures.furnitures.get() != Registry::FurnitureType::None && a_status != FurniStatus::Disallow) {
						scenes_furni.push_back(scene);
					} else {
						if (scene->furnitures.allowbed && a_status != FurniStatus::Disallow) {
							scenes_furni.push_back(scene);
						}
						scenes_none.push_back(scene);
					}
				}
			};
			get(a_preferedscene);
			for (auto&& sceneid : a_scenes) {
				get(sceneid);
			}
			if (scenes_furni.empty() && scenes_none.empty()) {
				a_vm->TraceStack("Cannot set center object without any provided scenes", a_stackID);
				return nullptr;
			}
		}
		RE::BGSRefAlias* center_alias = nullptr;
		RE::TESObjectREFR* center = nullptr;
		RE::TESObjectREFR* actor = nullptr;
		{ // get aliases and references from quest
			a_qst->aliasAccessLock.LockForRead();
			for (auto&& alias : a_qst->aliases) {
				if (!alias)
					continue;
				if (alias->aliasName == "CenterAlias") {
					center_alias = static_cast<RE::BGSRefAlias*>(alias);
					center = center_alias->GetReference();
				} else {
					const auto ref = static_cast<RE::BGSRefAlias*>(alias)->GetReference();
					actor = ref && ref->IsPlayerRef() ? ref : actor;
				}
			}
			a_qst->aliasAccessLock.UnlockForRead();
			if (!actor) {
				a_vm->TraceStack("Missing actor in some alias", a_stackID);
				return nullptr;
			} else if (!center_alias) {
				a_vm->TraceStack("Missing center alias", a_stackID);
				return nullptr;
			}
		}

		const auto getScene = [&](Registry::FurnitureType a_checktype) -> const Registry::Scene* {
			if (a_checktype == Registry::FurnitureType::None) {
				if (!scenes_none.empty()) {
					return scenes_none[0];
				}
			} else {
				for (auto&& scene : scenes_furni) {
					switch (a_checktype) {
					case Registry::FurnitureType::BedSingle:
					case Registry::FurnitureType::BedDouble:
						if (scene->furnitures.allowbed) {
							return scene;
						}
						__fallthrough;
					default:
						if (scene->furnitures.furnitures.all(a_checktype)) {
							return scene;
						}
						break;
					}
				}
			}
			return nullptr;
		};
		if (const auto scene = center ? getScene(Registry::FurnitureHandler::GetFurnitureType(center)) : nullptr; scene) {
			a_out[0] = scene->id;
      return center;
		}
    // default search with unknown center
    bool found_primary = false; // if some center matches the preferred scene, ignore all non preferred ones
		std::vector<std::pair<RE::TESObjectREFR*, const Registry::Scene*>> potentials{};
    // non furniture animations
		for (auto&& scene : scenes_none) {
			if (a_preferedscene == scene->id.c_str()) {
				potentials.clear();
				potentials.emplace_back(actor, scene);
				found_primary = true;
				break;
			} else {
				potentials.emplace_back(actor, scene);
			}
		}
		if (found_primary && !potentials[0].second->furnitures.allowbed) {
			// if the preferred scene is valid here and doesnt allow furniture interaction..
			a_out[0] = potentials[0].second->id;
			return potentials[0].first;
		}
    const size_t loose_potentials = potentials.size();
		// furniture animations
		if (a_status != FurniStatus::Disallow) {
			const auto center_coords = center ? center->GetPosition() : actor->GetPosition();
			const auto callback = [&](RE::TESObjectREFR& a_reference) mutable {
				if (std::fabs(center_coords.z - a_reference.GetPosition().z) > 256)
					return RE::BSContainer::ForEachResult::kContinue; // not same floor

				const auto type = Registry::FurnitureHandler::GetFurnitureType(&a_reference);
				if (type == Registry::FurnitureType::None)
					return RE::BSContainer::ForEachResult::kContinue;

				if (found_primary) {
					assert(!potentials.empty());
					const auto scene = potentials[0].second;
					if (scene->furnitures.furnitures.all(type)) {
						if (potentials[0].first == actor) {
							// Animation is loose (non furniture) but theres a compatible furniture closeby
							potentials.clear();
						}
						potentials.push_back({ &a_reference, scene });
					}
				} else if (const auto scene = getScene(type)) {
					if (a_preferedscene == scene->id.c_str()) {
						if (!found_primary) {
							found_primary = true;
							potentials.clear();
						}
					}
					potentials.emplace_back(&a_reference, scene);
				}
				return RE::BSContainer::ForEachResult::kContinue;
			};
			const auto TES = RE::TES::GetSingleton();
			if (const auto interior = TES->interiorCell; interior) {
				interior->ForEachReferenceInRange(center_coords, Settings::fScanRadius, callback);
			} else if (const auto grids = TES->gridCells; grids) {
				auto gridLength = grids->length;
				if (gridLength > 0) {
					float yPlus = center_coords.y + Settings::fScanRadius;
					float yMinus = center_coords.y - Settings::fScanRadius;
					float xPlus = center_coords.x + Settings::fScanRadius;
					float xMinus = center_coords.x - Settings::fScanRadius;
					for (uint32_t x = 0, y = 0; (x < gridLength && y < gridLength); x++, y++) {
						const auto gridcell = grids->GetCell(x, y);
						if (gridcell && gridcell->IsAttached()) {
							auto cellCoords = gridcell->GetCoordinates();
							if (!cellCoords)
								continue;
							float worldX = cellCoords->worldX;
							float worldY = cellCoords->worldY;
							if (worldX < xPlus && (worldX + 4096.0) > xMinus && worldY < yPlus && (worldY + 4096.0) > yMinus) {
								gridcell->ForEachReferenceInRange(center_coords, Settings::fScanRadius, callback);
							}
						}
					}
				}
			}
		}
		// pick and ret some valid
		const auto chosen_idx = Random::draw<size_t>(
			a_status == FurniStatus::Prefer && loose_potentials < potentials.size() ? loose_potentials : 0,
			potentials.size() - 1);
		const auto chosen_center = potentials.at(chosen_idx).first;
		size_t i = 0;
		for (auto&& [centerRef, scene] : potentials) {
			if (centerRef != chosen_center)
				continue;

			a_out[i++] = scene->id;
			if (i >= a_out.size()) {
				break;
			}
		}
		return chosen_center;
	}

}	 // namespace Papyrus::ThreadModel
