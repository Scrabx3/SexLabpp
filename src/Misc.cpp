#include "Misc.h"

#include "Papyrus/Settings.h"
#include "SexLab/DataKey.h"

namespace SexLab
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
		static const auto ActorTypeNPC = RE::TESForm::LookupByID<RE::BGSKeyword>(0x13794);
		return a_actor->HasKeyword(ActorTypeNPC);
	}

	bool IsVampire(const RE::Actor* a_actor)
	{
		static const auto vampire = RE::TESForm::LookupByID<RE::BGSKeyword>(0xA82BB);
		return a_actor->HasKeyword(vampire);
	}

	bool IsBed(const RE::TESObjectREFR* a_reference)
	{
		if (a_reference->GetName()[0] == '\0')
			return false;
		const auto root = a_reference->Get3D();
		const auto extra = root ? root->GetExtraData("FRN") : nullptr;
		const auto node = extra ? netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra) : nullptr;
		return node && !node->markers.empty() && node->markers[0].animationType.all(RE::BSFurnitureMarker::AnimationType::kSleep);
	}

	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor)
	{
		const auto base = a_actor->GetTemplateActorBase();
		return base ? base : a_actor->GetActorBase();
	}

}
