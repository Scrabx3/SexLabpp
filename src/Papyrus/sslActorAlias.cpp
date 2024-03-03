#include "sslActorAlias.h"

#include "UserData/StripData.h"
#include "Registry/Util/Scale.h"

namespace Papyrus::ActorAlias
{
	void LockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias)
	{
		if (!a_alias) {
			a_vm->TraceStack("Cannot call LockActorImpl on a none reference", a_stackID);
			return;
		}
		const auto actor = a_alias->GetActorReference();
		if (!actor) {
			a_vm->TraceStack("LockActorImpl requires the filled reference to be an actor", a_stackID);
			return;
		}
		if (actor->IsPlayerRef()) {
			RE::PlayerCharacter::GetSingleton()->SetAIDriven(true);

			if (const auto queue = RE::UIMessageQueue::GetSingleton()) {
				// force hide dialogue menu
				queue->AddMessage(RE::DialogueMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kForceHide, nullptr);

				// hide crosshair and activate prompt
				auto msg = queue->CreateUIMessageData(RE::InterfaceStrings::GetSingleton()->hudData);
				if (const auto data = static_cast<RE::HUDData*>(msg)) {
					data->text = "";
					data->type = RE::HUDData::Type::kActivateNoLabel;
					queue->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kUpdate, data);

				}
			}

			actor->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kAlive;
		} else {
			actor->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kRestrained;
		}

		actor->StopCombat();
		// actor->PauseCurrentDialogue();
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
		if (!a_alias) {
			a_vm->TraceStack("Cannot call LockActorImpl on a none reference", a_stackID);
			return;
		}
		const auto actor = a_alias->GetActorReference();
		if (!actor) {
			a_vm->TraceStack("LockActorImpl requires the filled reference to be an actor", a_stackID);
			return;
		}
		Registry::Scale::GetSingleton()->RemoveScale(actor);
		actor->actorState1.lifeState = actor->GetActorValue(RE::ActorValue::kHealth) <= 0 ? RE::ACTOR_LIFE_STATE::kDying : RE::ACTOR_LIFE_STATE::kAlive;
		if (actor->IsPlayerRef()) {
			RE::PlayerCharacter::GetSingleton()->SetAIDriven(false);
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
			return a_mergewith;
		}
		const auto join = [&]() {
			std::string ret{ "[" };
			for (size_t i = 0;; i++) {
				if (a_mergewith[i]) {
					ret += std::to_string(a_mergewith[i]->formID);
				}
				if (i == a_mergewith.size() - 1) {
					ret += "]";
					break;
				} else {
					ret += ", ";
				}
			}
			return ret;
		};
		logger::info("Stripping By Data {}, Initial Equipment: {}", a_mergewith.size(), join());
		uint32_t slots;
		bool weapon;
		if (a_overwrite.size() >= 2 && a_overwrite[0] != 0) {
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
		if (weapon && actor->currentProcess) {
			a_mergewith[Left] = actor->currentProcess->GetEquippedLeftHand();
			a_mergewith[Right] = actor->currentProcess->GetEquippedRightHand();
		}

		const auto stripconfig = UserData::StripData::GetSingleton();
		const auto manager = RE::ActorEquipManager::GetSingleton();
		const auto& inventory = actor->GetInventory();
		for (const auto& [form, data] : inventory) {
			if (form->IsNot(RE::FormType::Armor) || !data.second->IsWorn()) {
				continue;
			}

			switch (stripconfig->CheckStrip(form)) {
			case UserData::Strip::NoStrip:
				continue;
			case UserData::Strip::None:
				if (const auto biped = form->As<RE::TESObjectARMO>()) {
					const auto biped_slots = static_cast<uint32_t>(biped->GetSlotMask());
					if ((biped_slots & slots) == 0) {
						continue;
						// } else if (slots & static_cast<uint32_t>(SlotMask::kFeet)) {
						// 	const auto hdtspell = [&]() -> RE::SpellItem* {
						// 		static const auto* const heeleffect = RE::TESDataHandler::GetSingleton()->LookupForm<RE::EffectSetting>(0x800, "hdtHighHeel.esm"sv);
						// 		if (!heeleffect)
						// 			return nullptr;
						// 		for (const auto& spell : actor->addedSpells) {
						// 			if (!spell || spell->effects.empty())
						// 				continue;
						// 			for (const auto& effect : spell->effects) {
						// 				if (effect && effect->baseEffect == heeleffect)
						// 					return spell;
						// 			}
						// 		}
						// 		return nullptr;
						// 	}();
						// 	if (hdtspell) {
						// 		actor->RemoveSpell(hdtspell);
						// 		a_mergewith[Spell] = hdtspell;
						// 	}
					}
				}
				break;
			case UserData::Strip::Always:
				break;
			}
			a_mergewith.push_back(form);
			manager->UnequipObject(actor, form);
		}
		logger::info("Stripping By Data {}, Returning Equipment: {}", a_mergewith.size(), join());
		actor->Update3DModel();
		return a_mergewith;
	}

	// NOTE: Stuff below isnt implemented (yet) wanna see if its actually needed
	// ; ActorRef.QueueNiNodeUpdate()
	// ; ; NiOverride High Heels
	// ; If(Config.RemoveHeelEffect)
	// ; 	If(ActorRef.GetWornForm(0x80))
	// ; 		if Config.HasNiOverride
	// ; 			bool UpdateNiOPosition = NiOverride.RemoveNodeTransformPosition(ActorRef, false, vanilla_sex == 1, "NPC", "SexLab.esm")
	// ; 			if NiOverride.HasNodeTransformPosition(ActorRef, false, vanilla_sex == 1, "NPC", "internal")
	// ; 				float[] pos = NiOverride.GetNodeTransformPosition(ActorRef, false, vanilla_sex == 1, "NPC", "internal")
	// ; 				Log(pos, "RemoveHeelEffect (NiOverride)")
	// ; 				pos[0] = -pos[0]
	// ; 				pos[1] = -pos[1]
	// ; 				pos[2] = -pos[2]
	// ; 				NiOverride.AddNodeTransformPosition(ActorRef, false, vanilla_sex == 1, "NPC", "SexLab.esm", pos)
	// ; 				NiOverride.UpdateNodeTransform(ActorRef, false, vanilla_sex == 1, "NPC")
	// ; 			elseIf UpdateNiOPosition
	// ; 				NiOverride.UpdateNodeTransform(ActorRef, false, vanilla_sex == 1, "NPC")
	// ; 			endIf
	// ; 		endIf
	// ; 	EndIf
	// ; 	HDTHeelSpell = sslpp.GetHDTHeelSpell(ActorRef)
	// ; 	If(HDTHeelSpell)
	// ; 		Log("Removing HDT Heel Effect: " + HDTHeelSpell)
	// ; 		ActorRef.RemoveSpell(HDTHeelSpell)
	// ; 	EndIf
	// ; EndIf

}	 // namespace Papyrus::ActorAlias
