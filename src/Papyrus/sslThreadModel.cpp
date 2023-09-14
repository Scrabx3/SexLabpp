#include "sslThreadModel.h"

#include "Registry/Animation.h"
#include "Registry/Define/Furniture.h"
#include "Registry/Library.h"
#include "Registry/Util/CellCrawler.h"
#include "UserData/ConfigData.h"

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
		RE::reference_array<RE::BSFixedString> a_out,
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
		// get and sort scenes into furniture and non-furniure scenes
		const auto sort_in_array = [&](auto scene) {
			if (scene->furnitures.furnitures.get() != Registry::FurnitureType::None && a_status != FurniStatus::Disallow) {
				scenes_furni.push_back(scene);
			} else {
				if (scene->furnitures.allowbed && a_status != FurniStatus::Disallow) {
					scenes_furni.push_back(scene);
				}
				scenes_none.push_back(scene);
			}
		};
		const auto lib = Registry::Library::GetSingleton();
		const auto preferred_scene = lib->GetSceneByID(a_preferedscene);
		for (auto&& sceneid : a_scenes) {
			const auto scene = lib->GetSceneByID(sceneid);
			if (!scene)
				continue;
			sort_in_array(scene);
		}
		if (scenes_furni.empty() && scenes_none.empty()) {
			a_vm->TraceStack("Cannot set center object without any provided scenes", a_stackID);
			return nullptr;
		}
		// get aliases from quest,
		// if center is given, check for an animation that uses center
		// otherwise if preferred animation is given,
		const auto get_compatible_type = [](const Registry::Scene* a_scene, Registry::FurnitureType a_type) -> bool {
			switch (a_type) {
			case Registry::FurnitureType::BedSingle:
			case Registry::FurnitureType::BedDouble:
				if (a_scene->furnitures.allowbed) {
					return true;
				}
				__fallthrough;
			default:
				if (a_scene->furnitures.furnitures.all(a_type)) {
					return true;
				}
				break;
			}
			return false;
		};
		const auto [center, actor] = GetAliasRefs(a_qst);
		if (!actor) {
			a_vm->TraceStack("Quest should have some actor positions filled", a_stackID);
			return nullptr;
		}
		// is given center valid?
		if (center) {
			const auto centertype = Registry::FurnitureHandler::GetFurnitureType(center);
			const auto scene = [&]() -> const Registry::Scene* {
				if (preferred_scene && preferred_scene->IsCompatibleFurniture(centertype))
					return preferred_scene;

				if (centertype == Registry::FurnitureType::None)
					return scenes_none.empty() ? nullptr : scenes_none[Random::draw<size_t>(0, scenes_none.size() - 1)];

				for (auto&& scene : scenes_furni) {
					if (get_compatible_type(scene, centertype)) {
						return scene;
					}
				}
				return nullptr;
			}();
			if (scene) {
				a_out[0] = scene->id;
				return center;
			}
			return nullptr;
		}
		// non furniture animations
		if (a_status == FurniStatus::Disallow) {
			if (preferred_scene && preferred_scene->furnitures.furnitures == Registry::FurnitureType::None) {
				a_out[0] = a_preferedscene;
			} else {
				if (scenes_none.empty()) {
					return nullptr;
				}
				a_out[0] = scenes_none.at(Random::draw<size_t>(0, scenes_none.size() - 1))->id;
			}
			return actor;
		}
		// check cells for furniture compatible objects
		std::vector<std::pair<RE::TESObjectREFR*, const Registry::Scene*>> potentials{};
		const auto fromref = center ? center : actor;
		RE::TESObjectREFR* retref = nullptr;
		CellCrawler::ForEachObjectInRange(fromref, Settings::fScanRadius, [&](RE::TESObjectREFR& a_ref) {
			// TODO: Check if the object is actually reachable from current location (or if theres a wall between them)
			if (std::fabs(fromref->GetPositionZ() - a_ref.GetPosition().z) > 256)
				return RE::BSContainer::ForEachResult::kContinue;	 // not same floor

			// Skip, using actor for non furni animations
			const auto type = Registry::FurnitureHandler::GetFurnitureType(&a_ref);
			if (type == Registry::FurnitureType::None)
				return RE::BSContainer::ForEachResult::kContinue;

			// is pref compatible?
			if (preferred_scene && preferred_scene->furnitures.furnitures.all(type)) {
				a_out[0] = a_preferedscene;
				retref = &a_ref;
				return RE::BSContainer::ForEachResult::kStop;
			}

			// potentials
			for (auto&& scene : scenes_furni) {
				if (!scene || !get_compatible_type(scene, type)) {
					continue;
				}
				potentials.emplace_back(&a_ref, scene);
				scene = nullptr;
			}
			return RE::BSContainer::ForEachResult::kContinue;
		});
		if (retref) {	 // valid furniture for preferred scene
			return retref;
		} else if (potentials.size() && (a_status == FurniStatus::Prefer || Random::draw<int>(0, Settings::iFurniturePrefWeight))) {
			const auto frompos = fromref->GetPosition();
			std::sort(potentials.begin(), potentials.end(), [&](auto& a, auto& b) {
				return a.first->GetPosition().GetDistance(frompos) < b.first->GetPosition().GetDistance(frompos);
			});
			a_out[0] = potentials[0].second->id;
			return potentials[0].first;
		} else {
			if (scenes_none.empty()) {
				return nullptr;
			}
			a_out[0] = scenes_none.at(Random::draw<size_t>(0, scenes_none.size() - 1))->id;
			return actor;
		}
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
				UserData::ConfigData::GetSingleton()->AdjustOffsetByModel(model, ret);
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
			UserData::ConfigData::GetSingleton()->AdjustOffsetByStage(scene->id, stage->id, i, offsets);

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

	bool GetIsCompatiblecenter(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::BSFixedString a_sceneid, RE::TESObjectREFR* a_center)
	{
		if (!a_center) {
			a_vm->TraceStack("Cannot validate a none reference center", a_stackID);
			return false;
		}
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_sceneid);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return false;
		}
		return scene->IsCompatibleFurniture(a_center);
	}

}	 // namespace Papyrus::ThreadModel
