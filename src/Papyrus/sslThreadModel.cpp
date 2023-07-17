#include "sslThreadModel.h"

#include "Registry/Animation.h"
#include "Registry/Define/Furniture.h"
#include "Registry/Library.h"
#include "UserData/Offset.h"

using Offset = Registry::Offset;

namespace Papyrus::ThreadModel
{
	static inline std::pair<RE::TESObjectREFR*, RE::TESObjectREFR*> GetAliasRefs(RE::TESQuest* a_qst)
	{
		std::pair<RE::TESObjectREFR*, RE::TESObjectREFR*> ret{ nullptr, nullptr };
		a_qst->aliasAccessLock.LockForRead();
		for (auto&& alias : a_qst->aliases) {
			if (!alias)
				continue;
			if (alias->aliasName == "CenterAlias") {
				const auto aliasref = skyrim_cast<RE::BGSRefAlias*>(alias);
				if (aliasref) {
					ret.first = aliasref->GetReference();
				}
			} else {
				const auto aliasref = skyrim_cast<RE::BGSRefAlias*>(alias);
				if (!aliasref)
					continue;
				const auto ref = aliasref->GetReference();
				if (!ret.second || ref->IsPlayerRef()) {
					ret.second = ref;
				}
			}
		}
		a_qst->aliasAccessLock.UnlockForRead();
		return ret;
	}

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
		{	 // get scene objects from argument ids
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
		const auto [center, actor] = GetAliasRefs(a_qst);
		if (!center) {
			a_vm->TraceStack("Missing actor in some alias", a_stackID);
			return nullptr;
		} else if (!actor) {
			a_vm->TraceStack("Missing center alias", a_stackID);
			return nullptr;
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
		bool found_primary = false;	 // if some center matches the preferred scene, ignore all non preferred ones
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
				// TODO: Check if the object is actually reachable from current location (or if theres a wall between them)

				if (std::fabs(center_coords.z - a_reference.GetPosition().z) > 256)
					return RE::BSContainer::ForEachResult::kContinue;	 // not same floor

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
		// pick and return some valid
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

	std::vector<float> GetBaseCoordinates(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::BSFixedString a_scene)
	{
		using FT = Registry::FurnitureType;

		if (!a_qst) {
			a_vm->TraceStack("Cannot call GetBaseCoordinates on a none object", a_stackID);
			return { 0, 0, 0, 0 };
		}

		const auto [center, actor] = GetAliasRefs(a_qst);
		if (!center) {
			a_vm->TraceStack("Cannot get base coordinates from a none center", a_stackID);
			return { 0, 0, 0, 0 };
		}
		if (!actor) {
			a_vm->TraceStack("Missing some filled alias in quest", a_stackID);
			return { 0, 0, 0, 0 };
		}

		std::vector<float> ret{
			center->GetPositionX(),
			center->GetPositionY(),
			center->GetPositionZ(),
			center->GetAngleZ()
		};

		switch (Registry::FurnitureHandler::GetFurnitureType(center)) {
		case FT::BedDouble:
		case FT::BedSingle:
			ret[Offset::Z] += 45;
			break;
		case FT::BedRoll:
			ret[Offset::Z] += 7.5;
			ret[Offset::R] += static_cast<float>(std::_Pi / 2);
			break;
		default:
			const auto obj = center->Get3D();
			const auto extra = obj ? obj->GetExtraData("FRN") : nullptr;
			const auto markernode = extra ? netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra) : nullptr;
			if (!markernode || markernode->markers.size() <= 1)
				break;

			const auto actorpos = actor->GetPosition();
			const RE::BSFurnitureMarker* closest = nullptr;
			float closest_distance = 0;
			for (const auto& marker : markernode->markers) {
				const auto d = (marker.offset + RE::NiPoint3{ ret[0], ret[1], ret[2] }).GetDistance(actorpos);
				if (d <= closest_distance) {
					closest_distance = d;
					closest = &marker;
				}
			}
			assert(closest);
			// getting middle marker from marker-set closest to the player
			std::vector<decltype(closest)> neighbours{ closest };
			for (const auto& marker : markernode->markers) {
				if (&marker != closest && std::abs(marker.heading - closest->heading) <= 3) {
					neighbours.push_back(&marker);
				}
			}

			RE::NiPoint3 tmp{};
			for (auto&& neighbour : neighbours) {
				tmp.x += neighbour->offset.x;
				tmp.y += neighbour->offset.y;
				tmp.z += neighbour->offset.z;
			}
			tmp.x /= 3;
			tmp.y /= 3;
			tmp.z /= 3;

			std::sort(neighbours.begin(), neighbours.end(), [&](auto a, auto b) {
				return a->offset.GetDistance(tmp) < b->offset.GetDistance(tmp);
			});
			ret[Offset::X] += neighbours[0]->offset[Offset::X];
			ret[Offset::Y] += neighbours[0]->offset[Offset::Y];
			ret[Offset::Z] += neighbours[0]->offset[Offset::Z];
			ret[Offset::R] += neighbours[0]->heading;

			if (neighbours[0]->entryProperties.all(RE::BSFurnitureMarker::EntryProperties::kBehind)) {
				ret[Offset::R] += static_cast<float>(std::_Pi / 2);
			}
		}

		if (const auto base = center->GetBaseObject()) {
			if (const auto model = skyrim_cast<RE::TESModelTextureSwap*>(center)) {
				UserData::Offset::AdjustModelOffset(ret, model);
			}
		}

		const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_scene);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id. Scene specific offset will NOT be applied", a_stackID);
		} else {
			ret[Offset::X] += scene->furnitures.offset[Offset::X];
			ret[Offset::Y] += scene->furnitures.offset[Offset::Y];
			ret[Offset::Z] += scene->furnitures.offset[Offset::Z];
			ret[Offset::R] += scene->furnitures.offset[Offset::R];
		}

		return ret;
	}

	RE::BSFixedString PlaceAndPlay(VM* a_vm, StackID a_stackID, RE::TESQuest*, 
		std::vector<RE::Actor*> a_positions, 
		std::vector<float> a_coordinates,
		RE::BSFixedString a_scene, 
		RE::BSFixedString a_stage)
	{
		if (a_coordinates.size() != Offset::Total) {
			a_vm->TraceStack("Invalid offset", a_stackID);
			return "";
		}

		const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_scene);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return "";
		} else if (a_positions.size() != scene->positions.size()) {
			a_vm->TraceStack("Number positions do not match number of scene positions", a_stackID);
			return "";
		}

		const auto stage = scene->GetStageByKey(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return "";
		}

		for (size_t i = 0; i < a_positions.size(); i++) {
			std::vector<float> offsets{
				a_coordinates[Offset::X] + stage->positions[i].offset[Offset::X],
				a_coordinates[Offset::Y] + stage->positions[i].offset[Offset::Y],
				a_coordinates[Offset::Z] + stage->positions[i].offset[Offset::Z],
				a_coordinates[Offset::R] + stage->positions[i].offset[Offset::R]
			};
			UserData::Offset::AdjustStageOffset(offsets, i, scene->id, stage->id);

			RE::NiPoint3 position{
				offsets[Offset::X],
				offsets[Offset::Y],
				offsets[Offset::Z]
			};
			a_positions[i]->data.angle.z = offsets[Offset::R];
			a_positions[i]->SetPosition(position, false);
			a_positions[i]->NotifyAnimationGraph(stage->positions[i].event);
		}

		return stage->id;
	}

}	 // namespace Papyrus::ThreadModel
