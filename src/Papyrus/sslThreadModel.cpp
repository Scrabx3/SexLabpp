#include "sslThreadModel.h"

#include "Registry/Animation.h"
#include "Registry/Define/Furniture.h"
#include "Registry/Library.h"
#include "Registry/Util/CellCrawler.h"
#include "Registry/Util/Scale.h"

using Offset = Registry::CoordinateType;

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
		RE::reference_array<RE::BSFixedString> a_out_scenes,
		RE::reference_array<float> a_out_coordinates,
		FurniStatus a_status)
	{
		if (a_scenes.empty()) {
			a_vm->TraceStack("Missing scenes", a_stackID);
			return nullptr;
		}
		if (a_out_scenes.empty()) {
			a_vm->TraceStack("Result scene array must be allocated", a_stackID);
			return nullptr;
		}
		if (a_out_coordinates.size() != 4) {
			a_vm->TraceStack("Result coordinates must have length 4", a_stackID);
			return nullptr;
		}
		const auto [center, actor] = GetAliasRefs(a_qst);
		if (!center && !actor) {
			a_vm->TraceStack("Quest must have some actor positions filled", a_stackID);
			return nullptr;
		}
		const auto library = Registry::Library::GetSingleton();
		std::unordered_map<Registry::FurnitureType, std::vector<const Registry::Scene*>> scene_map{};
		for (auto&& sceneid : a_scenes) {
			const auto scene = library->GetSceneByID(sceneid);
			if (!scene) {
				a_vm->TraceStack("Invalid scene id in array", a_stackID);
				return nullptr;
			}
			if (scene->UsesFurniture()) {
				if (a_status == FurniStatus::Disallow) {
					continue;
				}
			} else if (a_status != FurniStatus::Disallow) {
				if (scene->furnitures.allowbed) {
					std::array bedtypes{
						Registry::FurnitureType::BedSingle,
						Registry::FurnitureType::BedDouble,
						Registry::FurnitureType::BedRoll
					};
					for (auto&& type : bedtypes) {
						auto& it = scene_map[type];
						if (std::ranges::find(it, scene) == it.end()) {
							it.push_back(scene);
						}
					}
				}
			}
			auto& it = scene_map[scene->furnitures.furnitures.get()];
			if (std::ranges::find(it, scene) == it.end()) {
				it.push_back(scene);
			}
		}
		assert(!scene_map.empty() && std::ranges::find_if(scene_map, [](auto& vec) { return vec.empty() } == scene_map.end()));
		const auto ReturnData = [&](Registry::FurnitureType a_where, const Registry::Coordinate& a_coordinate) -> bool {
			if (!scene_map.contains(a_where)) {
				return false;
			}
			const auto& scenes = scene_map[a_where];
			const auto count = std::min<size_t>(a_out_scenes.size(), scenes.size());
			for (size_t i = 0; i < count; i++) {
				a_out_scenes[i] = scenes[i]->id;
			}
			a_coordinate.ToContainer(a_out_coordinates);
			return true;
		};

		if (center) {
			const auto details = library->GetFurnitureDetails(center);
			if (!details) {
				Registry::Coordinate coord{ center };
				return ReturnData(Registry::FurnitureType::None, coord) ? center : nullptr;
			}
			if (!details->HasType(scene_map, [](auto& it) { return it.first; })) {
				return nullptr;
			}
			const auto coords = details->GetClosestCoordinateInBound(center, actor);
			if (coords.empty()) {
				return nullptr;
			}
			const auto i = Random::draw<size_t>(0, coords.size() - 1);
			return ReturnData(coords[i].first, coords[i].second) ? center : nullptr;
		}
		if (a_status == FurniStatus::Disallow || (a_status != FurniStatus::Prefer && !Random::draw<int>(0, Settings::iFurniturePrefWeight))) {
			// no need to look for furniture if furnitures are disallowed or 1/PrefWight rng decides we dont use furniture
			Registry::Coordinate coord{ actor };
			return ReturnData(Registry::FurnitureType::None, coord) ? actor : nullptr;
		}

		std::vector<std::pair<RE::TESObjectREFR*, const Registry::FurnitureDetails*>> found_objects;
		CellCrawler::ForEachObjectInRange(actor, Settings::fScanRadius, [&](RE::TESObjectREFR& a_ref) {
			const auto details = library->GetFurnitureDetails(&a_ref);
			if (!details || !details->HasType(scene_map, [](auto& it) { return it.first; })) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			found_objects.push_back(std::make_pair(&a_ref, details));
			return RE::BSContainer::ForEachResult::kContinue;
		});
		std::vector<std::tuple<Registry::FurnitureType, Registry::Coordinate, RE::TESObjectREFR*>> coords{};
		for (auto&& [ref, details] : found_objects) {
			const auto res = details->GetClosestCoordinateInBound(ref, actor);
			for (auto&& pair : res) {
				coords.push_back(std::make_tuple(pair.first, pair.second, ref));
			}
		}
		if (!coords.empty()) {
			std::sort(coords.begin(), coords.end(), [&](auto& a, auto& b) {
				return std::get<1>(a).GetDistance(actor) < std::get<1>(b).GetDistance(actor);
			});
			const auto& res = coords[0];
			return ReturnData(std::get<0>(res), std::get<1>(res)) ? std::get<2>(res) : nullptr;
		}
		Registry::Coordinate coord{ actor };
		return ReturnData(Registry::FurnitureType::None, coord) ? actor : nullptr;
	}

	std::vector<float> GetBaseCoordinates(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::BSFixedString a_scene)
	{
		using FT = Registry::FurnitureType;
		const auto [center, actor] = GetAliasRefs(a_qst);
		if (!center) {
			a_vm->TraceStack("Cannot get base coordinates from a none center", a_stackID);
			return { 0, 0, 0, 0 };
		}

		std::vector<float> ret{
			center->GetPositionX(),
			center->GetPositionY(),
			center->GetPositionZ(),
			center->GetAngleZ()
		};

		// const auto model = skyrim_cast<RE::TESModelTextureSwap*>(center);
		// const auto transform = model ? UserData::FurnitureData::GetSingleton()->GetFurnitureTransform(model) : nullptr;
		// if (transform) {


		// 	return ret;
		// }

		// TODO: This here is a complete mess and nukes performance, w/e I was trying to do here aint worth it
		switch (Registry::FurnitureHandler::GetFurnitureType(center)) {
		case FT::None:
			break;
		case FT::BedDouble:
		case FT::BedSingle:
			// ret[Registry::CoordinateType::Z] = 37.0
			ret[Registry::CoordinateType::Z] += 45;
			break;
		case FT::BedRoll:
			ret[Registry::CoordinateType::Z] += 7.5;
			ret[Registry::CoordinateType::R] += static_cast<float>(std::_Pi / 2);
			break;
			// default:
			// 	const auto obj = center->Get3D();
			// 	const auto extra = obj ? obj->GetExtraData("FRN") : nullptr;
			// 	const auto markernode = extra ? netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra) : nullptr;
			// 	if (!markernode || markernode->markers.size() <= 1)
			// 		break;

			// 	const auto actorpos = actor->GetPosition();
			// 	const RE::BSFurnitureMarker* closest = nullptr;
			// 	float closest_distance = 0;
			// 	for (const auto& marker : markernode->markers) {
			// 		const auto d = (marker.offset + RE::NiPoint3{ ret[0], ret[1], ret[2] }).GetDistance(actorpos);
			// 		if (d <= closest_distance) {
			// 			closest_distance = d;
			// 			closest = &marker;
			// 		}
			// 	}
			// 	assert(closest);
			// 	// getting middle marker from marker-set closest to the player
			// 	std::vector<decltype(closest)> neighbours{ closest };
			// 	for (const auto& marker : markernode->markers) {
			// 		if (&marker != closest && std::abs(marker.heading - closest->heading) <= 3) {
			// 			neighbours.push_back(&marker);
			// 		}
			// 	}

			// 	RE::NiPoint3 tmp{};
			// 	for (auto&& neighbour : neighbours) {
			// 		tmp.x += neighbour->offset.x;
			// 		tmp.y += neighbour->offset.y;
			// 		tmp.z += neighbour->offset.z;
			// 	}
			// 	tmp.x /= 3;
			// 	tmp.y /= 3;
			// 	tmp.z /= 3;

			// 	std::sort(neighbours.begin(), neighbours.end(), [&](auto a, auto b) {
			// 		return a->offset.GetDistance(tmp) < b->offset.GetDistance(tmp);
			// 	});
			// 	ret[Registry::CoordinateType::X] += neighbours[0]->offset[Registry::CoordinateType::X];
			// 	ret[Registry::CoordinateType::Y] += neighbours[0]->offset[Registry::CoordinateType::Y];
			// 	ret[Registry::CoordinateType::Z] += neighbours[0]->offset[Registry::CoordinateType::Z];
			// 	ret[Registry::CoordinateType::R] += neighbours[0]->heading;

			// 	if (neighbours[0]->entryProperties.all(RE::BSFurnitureMarker::EntryProperties::kBehind)) {
			// 		ret[Registry::CoordinateType::R] += static_cast<float>(std::_Pi / 2);
			// 	}
		}

		// const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_scene);
		// if (!scene) {
		// 	a_vm->TraceStack("Invalid scene id. Scene specific offset will NOT be applied", a_stackID);
		// } else {
		// 	ret[Registry::CoordinateType::X] += scene->furnitures.offset[Registry::CoordinateType::X];
		// 	ret[Registry::CoordinateType::Y] += scene->furnitures.offset[Registry::CoordinateType::Y];
		// 	ret[Registry::CoordinateType::Z] += scene->furnitures.offset[Registry::CoordinateType::Z];
		// 	ret[Registry::CoordinateType::R] += scene->furnitures.offset[Registry::CoordinateType::R];
		// }

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
		} else if (std::find(a_positions.begin(), a_positions.end(), nullptr) != a_positions.end()) {
			a_vm->TraceStack("Array contains a none reference", a_stackID);
			return "";
		}

		const auto stage = scene->GetStageByKey(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return "";
		}

		for (size_t i = 0; i < a_positions.size(); i++) {
			const auto& actor = a_positions[i];
			Registry::Coordinate coordinate{ a_coordinates };
			stage->positions[i].offset.Apply(coordinate);

			actor->data.angle.z = coordinate.rotation;
			actor->SetPosition(coordinate.AsNiPoint(), true);
			Registry::Scale::GetSingleton()->SetScale(actor, scene->positions[i].scale);

			// SKSE::GetTaskInterface()->AddTask([=]() {
			const auto event = scene->GetNthAnimationEvent(stage, i);
			actor->NotifyAnimationGraph(event);
			// });
		}

		return stage->id;
	}

	void RePlace(VM* a_vm, StackID a_stackID, RE::TESQuest*,
		RE::Actor* a_position,
		std::vector<float> a_coordinates,
		RE::BSFixedString a_scene,
		RE::BSFixedString a_stage,
		int32_t n)
	{
		if (!a_position) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_scene);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		}
		const auto stage = scene->GetStageByKey(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return;
		}
		if (n < 0 || n >= stage->positions.size()) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return;
		}
		Registry::Coordinate coordinate{ a_coordinates };
		stage->positions[n].offset.Apply(coordinate);
		a_position->data.angle.z = coordinate.rotation;
		a_position->SetPosition(coordinate.AsNiPoint(), true);
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

	std::vector<RE::BSFixedString> AddContextExImpl(RE::TESQuest*, std::vector<RE::BSFixedString> a_oldcontext, std::string a_newcontext)
	{
		const auto list = Registry::StringSplit(a_newcontext, ',');
		for (auto&& context : list) {
			if (!context.starts_with('!'))
				continue;
			const auto context_ = context.substr(1);
			if (std::find(a_oldcontext.begin(), a_oldcontext.end(), context_) != a_oldcontext.end())
				continue;
			a_oldcontext.push_back(context_);
		}
		return a_oldcontext;
	}

}	 // namespace Papyrus::ThreadModel
