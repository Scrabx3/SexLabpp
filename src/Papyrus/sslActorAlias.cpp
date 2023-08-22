#include "sslActorAlias.h"

namespace Papyrus::sslActorAlias
{
  // COMEBACK: Make a map to store charController flags?

	void LockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias)
	{
		if (!a_alias) {
			a_vm->TraceStack("Cannot call LockActorImpl on a none reference", a_stackID);
			return;
		}
		const auto act = a_alias->GetActorReference();
		if (!act) {
			a_vm->TraceStack("LockActorImpl requires the filled reference to be an actor", a_stackID);
			return;
		}
		act->actorState1.sneaking = 0;
		act->actorState1.flyState = RE::FLY_STATE::kNone;
		act->actorState2.weaponState = RE::WEAPON_STATE::kSheathed;
		if (act->IsPlayerRef()) {
			RE::PlayerCharacter::GetSingleton()->SetAIDriven(true);
			act->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kAlive;
		} else {
			act->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kRestrained;
		}

		act->StopCombat();
		act->PauseCurrentDialogue();
		act->InterruptCast(false);
		act->StopInteractingQuick(true);

		if (const auto process = act->currentProcess) {
			process->ClearMuzzleFlashes();
		}

		if (const auto charController = act->GetCharController(); charController) {
			charController->flags.set(RE::CHARACTER_FLAGS::kNotPushable);

			charController->flags.reset(RE::CHARACTER_FLAGS::kRecordHits);
			charController->flags.reset(RE::CHARACTER_FLAGS::kHitFlags);
		}
		act->StopMoving(1.0f);
	}

	void UnlockActorImpl(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias)
	{
		if (!a_alias) {
			a_vm->TraceStack("Cannot call LockActorImpl on a none reference", a_stackID);
			return;
		}
		const auto act = a_alias->GetActorReference();
		if (!act) {
			a_vm->TraceStack("LockActorImpl requires the filled reference to be an actor", a_stackID);
			return;
		}
		act->actorState1.sneaking = 0;
		act->actorState1.flyState = RE::FLY_STATE::kNone;
		act->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kAlive;
		act->actorState2.weaponState = RE::WEAPON_STATE::kSheathed;
		if (act->IsPlayerRef()) {
			RE::PlayerCharacter::GetSingleton()->SetAIDriven(false);
		}

		if (const auto charController = act->GetCharController(); charController) {
			charController->flags.reset(RE::CHARACTER_FLAGS::kNotPushable);

			charController->flags.set(RE::CHARACTER_FLAGS::kRecordHits);
			charController->flags.set(RE::CHARACTER_FLAGS::kHitFlags);
		}
	}

	std::vector<RE::TESForm*> StripByData(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias, Registry::Position::StripData a_stripdata, std::vector<uint32_t> a_overwrite)
	{
		if (!a_alias) {
			a_vm->TraceStack("Cannot call StripByData on a none alias", a_stackID);
			return {};
		}
		return StripByDataEx(a_vm, a_stackID, a_alias, a_stripdata, a_overwrite, {});
	}

	std::vector<RE::TESForm*> StripByDataEx(VM* a_vm, StackID a_stackID, RE::BGSRefAlias* a_alias,
		[[maybe_unused]] Registry::Position::StripData a_stripdata, 
		std::vector<uint32_t> a_overwrite, 
		std::vector<RE::TESForm*> a_mergewith)
	{
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

		
	// ; int[] Strip
	// ; If(StripOverride.Length == 2)
	// ; 	Strip = StripOverride
	// ; Else
	// ; 	Strip = Config.GetStripSettings(vanilla_sex == 1, Thread.UseLimitedStrip(), Thread.IsType[0], IsVictim())
	// ; EndIf
	// ; ; Gear
	// ; If(Strip[1])
	// ; 	RightHand = ActorRef.GetEquippedObject(1)
	// ; 	If(RightHand && sslpp.CheckStrip(RightHand) != -1)
	// ; 		ActorRef.UnequipItemEX(RightHand, ActorRef.EquipSlot_RightHand, false)
	// ; 	EndIf
	// ; 	LeftHand = ActorRef.GetEquippedObject(0)
	// ; 	If(LeftHand && sslpp.CheckStrip(RightHand) != -1)
	// ; 		ActorRef.UnequipItemEX(LeftHand, ActorRef.EquipSlot_LeftHand, false)
	// ; 	EndIf
	// ; EndIf
	// ; Form[] gear = sslpp.StripActor(ActorRef, Strip[0])
	// ; Equipment = PapyrusUtil.MergeFormArray(Equipment, gear)
	// ; Log("STRIPPING -> Stripped Items: Weapon (Right):" + RightHand + " / Weapon (Left): " + LeftHand + " / Armor: " + Equipment)
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

		return a_mergewith;
	}

}	 // namespace Papyrus::sslActorAlias

