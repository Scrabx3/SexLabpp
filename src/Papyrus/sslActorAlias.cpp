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
		const auto state = act->AsActorState();
		state->actorState1.sneaking = 0;
		state->actorState1.flyState = RE::FLY_STATE::kNone;
		state->actorState2.weaponState = RE::WEAPON_STATE::kSheathed;
		if (act->IsPlayerRef()) {
			RE::PlayerCharacter::GetSingleton()->SetAIDriven(true);
			state->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kAlive;
		} else {
			state->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kRestrained;
		}

		act->StopCombat();
		act->PauseCurrentDialogue();
		act->InterruptCast(false);
		act->StopInteractingQuick(true);

		if (const auto currentProcess = act->GetActorRuntimeData().currentProcess) {
			currentProcess->ClearMuzzleFlashes();
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
		const auto state = act->AsActorState();
		state->actorState1.sneaking = 0;
		state->actorState1.flyState = RE::FLY_STATE::kNone;
		state->actorState1.lifeState = RE::ACTOR_LIFE_STATE::kAlive;
		state->actorState2.weaponState = RE::WEAPON_STATE::kSheathed;
		if (act->IsPlayerRef()) {
			RE::PlayerCharacter::GetSingleton()->SetAIDriven(false);
		}

		if (const auto charController = act->GetCharController(); charController) {
			charController->flags.reset(RE::CHARACTER_FLAGS::kNotPushable);

			charController->flags.set(RE::CHARACTER_FLAGS::kRecordHits);
			charController->flags.set(RE::CHARACTER_FLAGS::kHitFlags);
		}
	}

}	 // namespace Papyrus::sslActorAlias

