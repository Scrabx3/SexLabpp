#include "sslActorAlias.h"

#include "Registry/Util/Scale.h"
#include "UserData/StripData.h"

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
			switch (actor->actorState1.lifeState) {
			case RE::ACTOR_LIFE_STATE::kUnconcious:
				actor->SetActorValue(RE::ActorValue::kVariable05, STATUS05::Unconscious);
			case RE::ACTOR_LIFE_STATE::kDying:
			case RE::ACTOR_LIFE_STATE::kDead:
				actor->SetActorValue(RE::ActorValue::kVariable05, STATUS05::Dying);
				actor->Resurrect(false, true);
				break;
			}
			actor->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kRestrained;
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
					a_mergewith[Right] = form->AsReference();
				else
					a_mergewith[Left] = form->AsReference();
			} else {
				a_mergewith.push_back(form);
			}
			manager->UnequipObject(actor, form);
		}
		std::vector<RE::FormID> ids{};
		ids.reserve(a_mergewith.size());
		for (auto&& it : a_mergewith)
			ids.push_back(it ? it->formID : 0);
		logger::info("Stripping, Policy: {}, Stripped Equipment: [{:X}]", a_mergewith.size(), fmt::join(ids, ", "));
		actor->Update3DModel();
		return a_mergewith;
	}

}	 // namespace Papyrus::ActorAlias
