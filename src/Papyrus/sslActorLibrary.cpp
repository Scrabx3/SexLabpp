#include "sslActorLibrary.h"

#include "Registry/Validation.h"
#include "UserData/StripData.h"

namespace Papyrus::ActorLibrary
{
	int32_t ValidateActorImpl(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor)
		{
			a_vm->TraceStack("Cannot validate a none reference", a_stackID);
			return -1;
		}
		const auto code = Registry::IsValidActorImpl(a_actor);
		return code == 0 ? -2 : code;
	}

	void WriteStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form, bool a_neverstrip)
	{
		if (!a_form) {
			a_vm->TraceStack("Cannot write strip settings for a none reference", a_stackID);
			return;
		}
		const auto strip = a_neverstrip ? UserData::Strip::NoStrip : UserData::Strip::Always;
		UserData::StripData::GetSingleton()->AddArmor(a_form, strip);
	}

	void EraseStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		if (!a_form) {
			a_vm->TraceStack("Cannot erase strip setting of a none reference", a_stackID);
			return;
		}
		UserData::StripData::GetSingleton()->RemoveArmor(a_form);
	}

	void EraseStripAll(RE::StaticFunctionTag*)
	{
		UserData::StripData::GetSingleton()->RemoveArmorAll();
	}

	int32_t CheckStrip(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form)
	{
		if (!a_form) {
			a_vm->TraceStack("Cannot check strip settings for none reference", a_stackID);
			return 0;
		}
		switch (UserData::StripData::GetSingleton()->CheckStrip(a_form)) {
		case UserData::Strip::NoStrip:
			return -1;
		case UserData::Strip::Always:
			return 1;
		default:
			return 0;
		}
	}

	std::vector<RE::TESForm*> UnequipSlots(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_reference, uint32_t a_slotmask)
	{
		if (!a_reference) {
			a_vm->TraceStack("Cannot retrieve hdt spell from a none reference", a_stackID);
			return {};
		}
		std::vector<RE::TESForm*> ret{};
		const auto& manager = RE::ActorEquipManager::GetSingleton();
		const auto cstrip = UserData::StripData::GetSingleton();
		const auto& inventory = a_reference->GetInventory();
		for (const auto& [form, data] : inventory) {
			if (form->Is(RE::FormType::LeveledItem) || !data.second->IsWorn()) {
				continue;
			}
			const auto strip = [&]() {
				manager->UnequipObject(a_reference, form);
				ret.push_back(form);
			};
			switch (cstrip->CheckStrip(form)) {
			case UserData::Strip::NoStrip:
				continue;
			case UserData::Strip::Always:
				strip();
				continue;
			}
			const auto& biped = form->As<RE::BGSBipedObjectForm>();
			if (biped) {
				const auto& slots = static_cast<uint32_t>(biped->GetSlotMask());
				if (slots & a_slotmask) {
					strip();
					continue;
				}
			}
		}
		return ret;
	}

	bool HasVehicle(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return false;
		}
		return a_actor->unk1E8;
	}

}	 // namespace Papyrus::ActorLibrary
