#include "Misc.h"

namespace Registry
{

	bool IsFuta(RE::Actor* a_actor)
	{
		static const auto sosfaction = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x00AFF8, "Schlongs of Skyrim.esp");
		if (!sosfaction)
			return false;

		bool ret = false;
		a_actor->VisitFactions([&ret](RE::TESFaction* a_faction, int8_t a_rank) -> bool {
			if (!a_faction || a_rank < 0)	
				return false;
			
			if (a_faction == sosfaction) {
				ret = true;
				return false;
			}
			const auto& excl = Settings::SOS_ExcludeFactions;
			if (std::find(excl.begin(), excl.end(), a_faction->formID) != excl.end()) {
				ret = false;
				return true;
			}
			std::string name{ a_faction->GetFullName() };
			if (!name.empty()) {
				ToLower(name);
				if (name.find("pubic") != std::string::npos) {
					ret = false;
					return true;
				}
			}
			return false;
		});
		return ret;
	}

	bool IsNPC(const RE::Actor* a_actor)
	{
		return a_actor->HasKeyword(GameForms::ActorTypeNPC);
	}

	bool IsVampire(const RE::Actor* a_actor)
	{
		return a_actor->HasKeyword(GameForms::Vampire);
	}

	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor)
	{
		const auto base = a_actor->GetTemplateActorBase();
		return base ? base : a_actor->GetActorBase();
	}

	void SetVehicle(RE::Actor* a_this, RE::TESObjectREFR* a_vehicle)
	{
		using func_t = void(RE::BSScript::Internal::VirtualMachine*, RE::VMStackID, RE::Actor*, RE::TESObjectREFR*);
		REL::Relocation<func_t> func{ REL::RelocationID(53940, 54764) };
		return func(nullptr, 0, a_this, a_vehicle);
	}

}
