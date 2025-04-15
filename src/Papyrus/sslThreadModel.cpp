#include "sslThreadModel.h"

#include "Registry/Define/Furniture.h"
#include "Registry/Library.h"
#include "Thread/NiNode/NiUpdate.h"
#include "Thread/NiNode/Node.h"
#include "Registry/Stats.h"
#include "Registry/Util/RayCast.h"
#include "Registry/Util/RayCast/ObjectBound.h"
#include "Registry/Util/Scale.h"
#include "UserData/StripData.h"
#include "Util/World.h"

using Offset = Registry::CoordinateType;

namespace Papyrus::ThreadModel
{
	namespace ActorAlias
	{
		void LockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias)
		{
			const auto actor = a_alias->GetActorReference();
			if (!actor) {
				a_vm->TraceStack("Reference is empty or not an actor", a_stackID);
				return;
			}
			if (actor->IsPlayerRef()) {
				RE::PlayerCharacter::GetSingleton()->SetAIDriven(true);
				const auto ui = RE::UI::GetSingleton();
				const auto interfacestr = RE::InterfaceStrings::GetSingleton();
				if (ui->IsMenuOpen(interfacestr->dialogueMenu)) {
					if (auto view = ui->GetMovieView(interfacestr->dialogueMenu)) {
						RE::GFxValue arg{ interfacestr->dialogueMenu };
						view->InvokeNoReturn("_global.skse.CloseMenu", &arg, 1);
					}
				}
				actor->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kAlive;
			} else {
				switch (actor->actorState1.lifeState) {
				case RE::ACTOR_LIFE_STATE::kUnconcious:
					actor->SetActorValue(RE::ActorValue::kVariable05, STATUS05::Unconscious);
					break;
				case RE::ACTOR_LIFE_STATE::kDying:
				case RE::ACTOR_LIFE_STATE::kDead:
					actor->SetActorValue(RE::ActorValue::kVariable05, STATUS05::Dying);
					actor->Resurrect(false, true);
					break;
				}
				actor->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kRestrained;
			}

			if (actor->IsWeaponDrawn()) {
				const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
				if (const auto script = factory ? factory->Create() : nullptr) {
					script->SetCommand("rae weaponsheathe");
					script->CompileAndRun(actor);
					if (!actor->IsPlayerRef() && actor->IsSneaking()) {
						script->SetCommand("setforcesneak 0");
						script->CompileAndRun(actor);
					}
					delete script;
				}
			}

			actor->StopCombat();
			actor->EndDialogue();
			actor->InterruptCast(false);
			actor->StopInteractingQuick(true);
			actor->SetCollision(false);

			if (const auto process = actor->currentProcess) {
				process->ClearMuzzleFlashes();
			}
			actor->StopMoving(1.0f);
		}

		void UnlockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias)
		{
			const auto actor = a_alias->GetActorReference();
			if (!actor) {
				a_vm->TraceStack("Reference is empty or not an actor", a_stackID);
				return;
			}
			Registry::Scale::GetSingleton()->RemoveScale(actor);
			switch (static_cast<int32_t>(actor->GetActorValue(RE::ActorValue::kVariable05))) {
			case STATUS05::Unconscious:
				actor->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kUnconcious;
				break;
			// case STATUS05::Dying:
			// 	{
			// 		const float hp = actor->GetActorValue(RE::ActorValue::kHealth);
			// 		const auto killer = actor->myKiller.get().get();
			// 		actor->KillImpl(killer, hp + 1, false, true);
			// 	}
			// 	break;
			default:
				actor->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kAlive;
				break;
			}
			if (actor->IsPlayerRef()) {
				RE::PlayerCharacter::GetSingleton()->SetAIDriven(false);
			} else {
				actor->SetActorValue(RE::ActorValue::kVariable05, 0.0f);
			}
			actor->SetCollision(true);
		}

		std::vector<RE::TESForm*> StripByData(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias,
			Registry::Position::StripData a_stripdata, std::vector<uint32_t> a_defaults, std::vector<uint32_t> a_overwrite)
		{
			if (!a_alias) {
				a_vm->TraceStack("Cannot call StripByData on a none alias", a_stackID);
				return {};
			}
			return StripByDataEx(a_vm, a_stackID, a_alias, a_stripdata, a_defaults, a_overwrite, {});
		}

		std::vector<RE::TESForm*> StripByDataEx(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias,
			Registry::Position::StripData a_stripdata,
			std::vector<uint32_t> a_defaults,				// use if a_stripData == default
			std::vector<uint32_t> a_overwrite,			// use if exists
			std::vector<RE::TESForm*> a_mergewith)	// [HighHeelSpell, WeaponRight, WeaponLeft, Armor...]
		{
			using Strip = Registry::Position::StripData;
			using SlotMask = RE::BIPED_MODEL::BipedObjectSlot;

			enum MergeIDX
			{
				Spell = 0,
				Right = 1,
				Left = 2,
			};

			if (!a_alias) {
				a_vm->TraceStack("Cannot call StripByDataEx on a none alias", a_stackID);
				return a_mergewith;
			}
			const auto actor = a_alias->GetActorReference();
			if (!actor) {
				a_vm->TraceStack("ReferenceAlias must be filled with an actor reference", a_stackID);
				return a_mergewith;
			}
			if (a_mergewith.size() < 3) {
				a_mergewith.resize(3, nullptr);
			}
			if (a_stripdata == Strip::None) {
				logger::info("Stripping, Policy: NONE");
				return a_mergewith;
			}
			uint32_t slots;
			bool weapon;
			if (a_overwrite.size() >= 2) {
				slots = a_overwrite[0];
				weapon = a_overwrite[1];
			} else {
				stl::enumeration<Strip> stripnum(a_stripdata);
				if (stripnum.all(Strip::All)) {
					slots = static_cast<uint32_t>(-1);
					weapon = true;
				} else {
					if (stripnum.all(Strip::Default) && a_defaults.size() >= 2) {
						slots = a_defaults[0];
						weapon = a_defaults[1];
					} else {
						logger::error("Strip Policy uses faulty default settings");
						slots = 0;
						weapon = 0;
					}
					if (stripnum.all(Strip::Boots)) {
						slots |= static_cast<uint32_t>(SlotMask::kFeet);
					}
					if (stripnum.all(Strip::Gloves)) {
						slots |= static_cast<uint32_t>(SlotMask::kHands);
					}
					if (stripnum.all(Strip::Helmet)) {
						slots |= static_cast<uint32_t>(SlotMask::kHead);
					}
				}
			}
			const auto stripconfig = UserData::StripData::GetSingleton();
			const auto manager = RE::ActorEquipManager::GetSingleton();
			for (const auto& [form, data] : actor->GetInventory()) {
				if (!data.second->IsWorn()) {
					continue;
				}
				switch (stripconfig->CheckStrip(form)) {
				case UserData::Strip::NoStrip:
					continue;
				case UserData::Strip::Always:
					break;
				case UserData::Strip::None:
					if (form->IsWeapon() && !weapon) {
						continue;
					} else if (const auto biped = form->As<RE::BGSBipedObjectForm>()) {
						const auto biped_slots = static_cast<uint32_t>(biped->GetSlotMask());
						if ((biped_slots & slots) == 0) {
							continue;
						}
					}
					break;
				}
				if (form->IsWeapon() && actor->currentProcess) {
					if (actor->currentProcess->GetEquippedRightHand() == form)
						a_mergewith[Right] = form;
					else
						a_mergewith[Left] = form;
				} else {
					a_mergewith.push_back(form);
				}
				manager->UnequipObject(actor, form);
			}
			std::vector<RE::FormID> ids{};
			ids.reserve(a_mergewith.size());
			for (auto&& it : a_mergewith)
				ids.push_back(it ? it->formID : 0);
			logger::info("Stripping, Policy: [{:X}, {}], Stripped Equipment: [{}]", weapon, slots, [&] {
				if (ids.empty()) {
					return std::string("");
				}
				return std::accumulate(std::next(ids.begin()), ids.end(), std::format("{:X}", ids[0]), [](std::string a, auto b) {
					return std::move(a) + ", " + std::format("{:X}", b);
				});
			}());
			actor->Update3DModel();
			return a_mergewith;
		}

	}	 // namespace ActorAlias

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
				if (ref && (!ret.second || ref->IsPlayerRef())) {
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
		const auto& [center, actor] = GetAliasRefs(a_qst);
		if (!actor) {
			a_vm->TraceStack("Quest must have some actor positions filled", a_stackID);
			return nullptr;
		}
		const auto library = Registry::Library::GetSingleton();
		std::unordered_map<Registry::FurnitureType, std::vector<const Registry::Scene*>> scene_map{};
		stl::enumeration<Registry::FurnitureType> filled_types;
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
				filled_types.set(scene->furnitures.furnitures.get());
				const auto types = Registry::FlagToComponents(scene->furnitures.furnitures.get());
				for (auto&& flag : types) {
					auto& it = scene_map[flag];
					if (std::ranges::find(it, scene) == it.end()) {
						it.push_back(scene);
					}
				}
			} else {
				if (scene->furnitures.allowbed && a_status != FurniStatus::Disallow) {
					std::array bedtypes{
						Registry::FurnitureType::BedSingle,
						Registry::FurnitureType::BedDouble,
						Registry::FurnitureType::BedRoll
					};
					for (auto&& type : bedtypes) {
						auto& it = scene_map[type];
						if (std::ranges::find(it, scene) == it.end()) {
							filled_types.set(type);
							it.push_back(scene);
						}
					}
				}
				auto& it = scene_map[Registry::FurnitureType::None];
				if (std::ranges::find(it, scene) == it.end()) {
					it.push_back(scene);
				}
			}
		}
		assert(!scene_map.empty() && std::ranges::find_if(scene_map, [](auto& typevec) { return typevec.second.empty(); }) == scene_map.end());
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
			const auto coords = details->GetClosestCoordinateInBound(center, filled_types, actor);
			if (coords.empty()) {
				return nullptr;
			}
			const auto i = Random::draw<size_t>(0, coords.size() - 1);
			return ReturnData(coords[i].first, coords[i].second) ? center : nullptr;
		}
		if (a_status == FurniStatus::Disallow) {
			Registry::Coordinate coord{ actor };
			return ReturnData(Registry::FurnitureType::None, coord) ? actor : nullptr;
		} else if (a_status != FurniStatus::Prefer && !Random::draw<int>(0, Settings::iFurniturePrefWeight)) {
			Registry::Coordinate coord{ actor };
			if (ReturnData(Registry::FurnitureType::None, coord)) {
				return actor;
			}
		}

		std::vector<std::pair<RE::TESObjectREFR*, glm::vec4>> thread_heads{};
		a_qst->aliasAccessLock.LockForRead();
		for (auto&& alias : a_qst->aliases) {
			if (!alias)
				continue;
			const auto aliasref = skyrim_cast<RE::BGSRefAlias*>(alias);
			if (!aliasref)
				continue;
			const auto ref = aliasref->GetReference();
			if (!ref || ref == center)
				continue;
			auto head = ref->GetNodeByName(Thread::NiNode::Node::HEAD);
			if (!head)
				continue;
			auto& t = head->world.translate;
			thread_heads.emplace_back(ref, glm::vec4{ t.x, t.y, t.z, 0.0f });
		}
		a_qst->aliasAccessLock.UnlockForRead();

		std::vector<RE::TESObjectREFR*> used_furnitures{};
		const auto processlist = RE::ProcessLists::GetSingleton();
		for (auto&& ithandle : processlist->highActorHandles) {
			const auto it = ithandle.get();
			if (!it || it.get() == actor || it.get() == center)
				continue;
			const auto furni = it->GetOccupiedFurniture().get();
			if (!furni)
				continue;
			used_furnitures.push_back(furni.get());
		}

		std::vector<std::pair<RE::TESObjectREFR*, const Registry::FurnitureDetails*>> found_objects;
		Util::ForEachObjectInRange(actor, Settings::fScanRadius, [&](RE::TESObjectREFR* a_ref) {
			if (!a_ref || std::ranges::contains(used_furnitures, a_ref)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			const auto details = library->GetFurnitureDetails(a_ref);
			if (!details || !details->HasType(scene_map, [](auto& it) { return it.first; })) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			auto obj = a_ref->Get3D();
			auto node = obj ? obj->AsNode() : nullptr;
			auto box = node ? ObjectBound::MakeBoundingBox(node) : std::nullopt;
			if (!box) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			auto center = box->GetCenterWorld();
			auto end = glm::vec4(center, 0.0f);
			for (auto&& it : thread_heads) {
				auto startref = it.first;
				auto start = it.second;
				do {
					auto res = Raycast::hkpCastRay(start, end, { a_ref, startref });
					if (!res.hit) {
						found_objects.emplace_back(a_ref, details);
						goto __CONTINUE_NEXT;
					}
					auto hitref = res.hitObject ? res.hitObject->GetUserData() : nullptr;
					auto base = hitref ? hitref->GetBaseObject() : nullptr;
					if (!base || base->Is(RE::FormType::Static, RE::FormType::MovableStatic, RE::FormType::Furniture))
						break;
					if (base->Is(RE::FormType::Door) && hitref->IsLocked())
						break;
					startref = hitref;
					start = res.hitPos;
				} while (true);
			}
__CONTINUE_NEXT:
			return RE::BSContainer::ForEachResult::kContinue;
		});
		std::vector<std::tuple<Registry::FurnitureType, Registry::Coordinate, RE::TESObjectREFR*>> coords{};
		for (auto&& [ref, details] : found_objects) {
			const auto res = details->GetClosestCoordinateInBound(ref, filled_types, actor);
			for (auto&& pair : res) {
				coords.push_back(std::make_tuple(pair.first, pair.second, ref));
			}
		}
		if (!coords.empty()) {
			// IDEA: Give player an alphabetical list of all found options here
			std::sort(coords.begin(), coords.end(), [&](auto& a, auto& b) {
				return std::get<1>(a).GetDistance(actor) < std::get<1>(b).GetDistance(actor);
			});
			const auto& res = coords[0];
			if (ReturnData(std::get<0>(res), std::get<1>(res))) {
				return std::get<2>(res);
			}
		}
		Registry::Coordinate coord{ actor };
		return ReturnData(Registry::FurnitureType::None, coord) ? actor : nullptr;
	}

	bool UpdateBaseCoordinates(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::BSFixedString a_sceneid, RE::reference_array<float> a_out)
	{
		const auto& [center, actor] = GetAliasRefs(a_qst);
		if (!actor || !center) {
			a_vm->TraceStack("Invalid aliases", a_stackID);
			return false;
		}
		const auto library = Registry::Library::GetSingleton();
		const auto scene = library->GetSceneByID(a_sceneid);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return false;
		}
		const auto details = library->GetFurnitureDetails(center);
		if (!details)	 // nothing to do
			return true;
		auto res = details->GetClosestCoordinateInBound(center, scene->furnitures.furnitures, actor);
		if (res.empty())
			return false;
		scene->furnitures.offset.Apply(res[0].second);
		res[0].second.ToContainer(a_out);
		return true;
	}

	void ApplySceneOffset(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::BSFixedString a_sceneid, RE::reference_array<float> a_out)
	{
		const auto library = Registry::Library::GetSingleton();
		const auto scene = library->GetSceneByID(a_sceneid);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		}
		Registry::Coordinate ret{ a_out };
		scene->furnitures.offset.Apply(ret);
		ret.ToContainer(a_out);
	}

	int SelectNextStage(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::BSFixedString a_scene, RE::BSFixedString a_stage, std::vector<RE::BSFixedString> a_tags)
	{
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_scene);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return 0;
		}
		const auto stage = scene->GetStageByID(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return 0;
		}
		auto adj = scene->GetAdjacentStages(stage);
		if (!adj || adj->empty()) {
			return 0;
		}
		Registry::TagData tags{ a_tags };
		std::vector<int> weights{};
		int n = 0;
		for (auto&& i : *adj) {
			auto c = i->tags.CountTags(tags);
			weights.resize(weights.size() + c + 1, n++);
		}
		return weights[Random::draw<size_t>(0, weights.size() - 1)];
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

		const auto stage = scene->GetStageByID(a_stage);
		if (!stage) {
			a_vm->TraceStack("Invalid stage id", a_stackID);
			return "";
		}

		for (size_t i = 0; i < a_positions.size(); i++) {
			const auto& actor = a_positions[i];
			Registry::Coordinate coordinate{ a_coordinates };
			stage->positions[i].offset.Apply(coordinate);

			actor->data.angle.z = coordinate.rotation;
			actor->data.angle.x = actor->data.angle.y = 0.0f;
			actor->SetPosition(coordinate.AsNiPoint(), true);
			actor->Update3DPosition(true);
			Registry::Scale::GetSingleton()->SetScale(actor, scene->positions[i].data.GetScale());

			const auto event = scene->GetNthAnimationEvent(stage, i);
			actor->NotifyAnimationGraph(event);
			const auto schlong = std::format("SOSBend{}", stage->positions[i].schlong);
			actor->NotifyAnimationGraph(schlong);
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
		const auto stage = scene->GetStageByID(a_stage);
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
		const auto list = Util::StringSplit(a_newcontext, ",");
		for (auto&& tag : list) {
			if (!tag.starts_with('!'))
				continue;
			const auto context = RE::BSFixedString(std::string(tag.substr(1)));
			if (std::find(a_oldcontext.begin(), a_oldcontext.end(), context) != a_oldcontext.end())
				continue;
			a_oldcontext.push_back(context);
		}
		return a_oldcontext;
	}

	void ShuffleScenes(RE::TESQuest*, RE::reference_array<RE::BSFixedString> a_scenes, RE::BSFixedString a_tofront)
	{
		if (a_scenes.empty()) {
			return;
		}
		auto start = a_scenes.begin();
		if (!a_tofront.empty() && Registry::Library::GetSingleton()->GetSceneByID(a_tofront)) {
			auto where = std::ranges::find(a_scenes, a_tofront);
			if (where == a_scenes.end()) {
				a_scenes[0] = a_tofront;
			} else {
				std::iter_swap(a_scenes.begin(), where);
			}
			start++;
		}
		std::random_device rd;
		std::mt19937 gen{ rd() };
		std::ranges::shuffle(start, a_scenes.end(), gen);
	}

	bool IsCollisionRegistered(RE::TESQuest* a_qst)
	{
		return Thread::NiNode::NiUpdate::IsRegistered(a_qst->formID);
	}

	void RegisterCollision(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions, RE::BSFixedString a_activescene)
	{
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_activescene);
		if (!scene || scene->CountPositions() != a_positions.size()) {
			a_vm->TraceStack("Invalid scene", a_stackID);
			return;
		}
		Thread::NiNode::NiUpdate::Register(a_qst->formID, a_positions, scene);
	}

	void UnregisterCollision(RE::TESQuest* a_qst)
	{
		Thread::NiNode::NiUpdate::Unregister(a_qst->formID);
	}

	std::vector<int> GetCollisionActions(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, RE::Actor* a_partner)
	{
		auto process = Thread::NiNode::NiUpdate::GetProcess(a_qst->formID);
		if (!process) {
			a_vm->TraceStack("Not registered", a_stackID);
			return {};
		}
		std::vector<int> ret{};
		process->VisitPositions([&](auto& p) {
			if (a_position && p.actor->formID != a_position->formID)
				return false;
			for (auto&& type : p.interactions) {
				if (a_partner && type.partner->formID != a_partner->formID)
					continue;
				ret.push_back(static_cast<int>(type.action));
			}
			return false;
		});
		return ret;
	}

	bool HasCollisionAction(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int a_type, RE::Actor* a_position, RE::Actor* a_partner)
	{
		auto process = Thread::NiNode::NiUpdate::GetProcess(a_qst->formID);
		if (!process) {
			a_vm->TraceStack("Not registered", a_stackID);
			return false;
		}
		return process->VisitPositions([&](auto& p) {
			if (a_position && p.actor->formID != a_position->formID)
				return false;
			for (auto&& type : p.interactions) {
				if (a_partner && type.partner->formID != a_partner->formID)
					continue;
				if (a_type != -1 && a_type != static_cast<int>(type.action))
					continue;
				return true;
			}
			return false;
		});
	}

	RE::Actor* GetPartnerByAction(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type)
	{
		if (!a_position) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return nullptr;
		}
		auto process = Thread::NiNode::NiUpdate::GetProcess(a_qst->formID);
		if (!process) {
			a_vm->TraceStack("Not registered", a_stackID);
			return nullptr;
		}
		RE::Actor* ret = nullptr;
		process->VisitPositions([&](auto& p) {
			if (p.actor->formID != a_position->formID)
				return false;
			for (auto&& type : p.interactions) {
				if (a_type != -1 && a_type != static_cast<int>(type.action))
					continue;
				ret = type.partner.get();
				return true;
			}
			return false;
		});
		return ret;
	}

	std::vector<RE::Actor*> GetPartnersByAction(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type)
	{
		auto process = Thread::NiNode::NiUpdate::GetProcess(a_qst->formID);
		if (!process) {
			a_vm->TraceStack("Not registered", a_stackID);
			return {};
		}
		std::vector<RE::Actor*> ret{};
		process->VisitPositions([&](auto& p) {
			if (a_position && p.actor->formID != a_position->formID)
				return false;
			for (auto&& type : p.interactions) {
				if (a_type != -1 && a_type != static_cast<int>(type.action))
					continue;
				ret.push_back(type.partner.get());
			}
			return false;
		});
		return ret;
	}

	RE::Actor* GetPartnerByTypeRev(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type)
	{
		if (!a_position) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		auto process = Thread::NiNode::NiUpdate::GetProcess(a_qst->formID);
		if (!process) {
			a_vm->TraceStack("Not registered", a_stackID);
			return {};
		}
		RE::Actor* ret = nullptr;
		process->VisitPositions([&](auto& p) {
			for (auto&& type : p.interactions) {
				if (a_position->formID == type.partner->formID) {
					if (a_type == -1 || a_type == static_cast<int>(type.action)) {
						ret = p.actor.get();
						return true;
					}
					break;
				}
			}
			return false;
		});
		return ret;
	}

	std::vector<RE::Actor*> GetPartnersByTypeRev(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, int a_type)
	{
		auto process = Thread::NiNode::NiUpdate::GetProcess(a_qst->formID);
		if (!process) {
			a_vm->TraceStack("Not registered", a_stackID);
			return {};
		}
		std::vector<RE::Actor*> ret{};
		process->VisitPositions([&](auto& p) {
			for (auto&& type : p.interactions) {
				if (!a_position || a_position->formID == type.partner->formID) {
					if (a_type == -1 || a_type == static_cast<int>(type.action))
						ret.push_back(p.actor.get());
					break;
				}
			}
			return false;
		});
		return ret;
	}

	float GetActionVelocity(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::Actor* a_position, RE::Actor* a_partner, int a_type)
	{
		if (!a_position) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0.0f;
		}
		if (a_type == -1) {
			a_vm->TraceStack("Type cant be 'any'", a_stackID);
			return 0.0f;
		}
		auto process = Thread::NiNode::NiUpdate::GetProcess(a_qst->formID);
		if (!process) {
			a_vm->TraceStack("Not registered", a_stackID);
			return 0.0f;
		}
		float ret = 0.0f;
		process->VisitPositions([&](auto& p) {
			if (p.actor->formID != a_position->formID)
				return false;
			for (auto&& type : p.interactions) {
				if (a_partner && a_partner->formID != type.partner->formID)
					continue;
				if (a_type != static_cast<int>(type.action))
					continue;
				ret = type.velocity;
				return true;
			}
			return false;
		});
		return ret;
	}

	void AddExperience(VM* a_vm, StackID a_stackID, RE::TESQuest*, std::vector<RE::Actor*> a_positions,
		RE::BSFixedString a_scene, std::vector<RE::BSFixedString> a_playedstages)
	{
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_scene);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		} else if (scene->CountPositions() != a_positions.size()) {
			a_vm->TraceStack("Position cound does not match scene position count", a_stackID);
			return;
		}
		int vaginal = 0, anal = 0, oral = 0;
		for (auto&& it : a_playedstages) {
			auto stage = scene->GetStageByID(it);
			if (!stage)
				continue;

			vaginal += stage->tags.HasTag(Registry::Tag::Vaginal);
			anal += stage->tags.HasTag(Registry::Tag::Anal);
			oral += stage->tags.HasTag(Registry::Tag::Oral);
		}
		const auto statdata = Registry::Statistics::StatisticsData::GetSingleton();
		for (auto&& p : a_positions) {
			if (!p)
				continue;

			auto& stats = statdata->GetStatistics(p);
			stats.AddStatistic(stats.XP_Vaginal, vaginal * 1.25f);
			stats.AddStatistic(stats.XP_Anal, anal * 1.25f);
			stats.AddStatistic(stats.XP_Oral, oral * 1.25f);
		}
	}


	void UpdateStatistics(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::Actor* a_actor, std::vector<RE::Actor*> a_positions,
		RE::BSFixedString a_scene, std::vector<RE::BSFixedString> a_playedstages, float a_time)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		const auto scene = Registry::Library::GetSingleton()->GetSceneByID(a_scene);
		if (!scene) {
			a_vm->TraceStack("Invalid scene id", a_stackID);
			return;
		} else if (scene->CountPositions() != a_positions.size()) {
			a_vm->TraceStack("Position cound does not match scene position count", a_stackID);
			return;
		}
		auto& stats = Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor);
		stats.SetStatistic(stats.LastUpdate_GameTime, RE::Calendar::GetSingleton()->GetCurrentGameTime());
		stats.AddStatistic(stats.SecondsInScene, a_time);
		stats.AddStatistic(stats.TimesTotal, 1);
		if (scene->CountPositions() == 1) {
			stats.AddStatistic(stats.TimesMasturbated, 1);
			if (scene->CountSubmissives() == 1) {
				stats.AddStatistic(stats.TimesSubmissive, 1);
			}
		} else {
			int sub = 0;
			for (size_t i = 0; i < a_positions.size(); i++) {
				if (!a_positions[i])
					continue;
				if (a_positions[i] == a_actor) {
					const auto& p = scene->GetNthPosition(i);
					sub = p->IsSubmissive();
					continue;
				}
				if (sub != 1 && scene->GetNthPosition(i)->IsSubmissive()) {
					sub = -1;
				}
				if (a_positions[i]->IsHumanoid()) {
					switch (Registry::GetSex(a_positions[i])) {
					case Registry::Sex::Male:
						stats.AddStatistic(stats.PartnersMale, 1);
						break;
					case Registry::Sex::Female:
						stats.AddStatistic(stats.PartnersFemale, 1);
						break;
					case Registry::Sex::Futa:
						stats.AddStatistic(stats.PartnersFuta, 1);
						break;
					}
				} else {
					stats.AddStatistic(stats.PartnersCreature, 1);
				}
			}
			switch (sub) {
			case -1:
				stats.AddStatistic(stats.TimesDominant, 1);
				break;
			case 1:
				stats.AddStatistic(stats.TimesSubmissive, 1);
				break;
			}
		}
		int vaginal = 0, anal = 0, oral = 0;
		for (auto&& it : a_playedstages) {
			auto stage = scene->GetStageByID(it);
			if (!stage)
				continue;

			vaginal += stage->tags.HasTag(Registry::Tag::Vaginal);
			anal += stage->tags.HasTag(Registry::Tag::Anal);
			oral += stage->tags.HasTag(Registry::Tag::Oral);
		}
		if (vaginal) {
			stats.AddStatistic(stats.TimesVaginal, 1);
			stats.AddStatistic(stats.XP_Vaginal, vaginal * 1.25f);
		}
		if (anal) {
			stats.AddStatistic(stats.TimesAnal, 1);
			stats.AddStatistic(stats.XP_Anal, anal * 1.25f);
		}
		if (oral) {
			stats.AddStatistic(stats.TimesOral, 1);
			stats.AddStatistic(stats.XP_Oral, oral * 1.25f);
		}
	}

}	 // namespace Papyrus::ThreadModel
