#include "Misc.h"

#include "Papyrus/Settings.h"
#include "SexLab/DataKey.h"

namespace SexLab
{

	bool IsFuta(const RE::Actor* a_actor)
	{
		static const auto sosfaction = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x00AFF8, "Schlongs of Skyrim.esp");
		if (!sosfaction)
			return false;

		const auto base = a_actor->GetActorBase();
		if (!base)
			return false;

		auto ret = false;
		for (auto&& f : base->factions) {
			if (!f.faction)
				continue;
			if (f.faction == sosfaction)
				ret = true;
			if (std::find(Settings::SOS_ExcludeFactions.begin(), Settings::SOS_ExcludeFactions.end(), f.faction->formID) != Settings::SOS_ExcludeFactions.end())
				return false;

			std::string name{ f.faction->GetFullName() };
			if (!name.empty()) {
				ToLower(name);
				if (name.find("pubic") != std::string::npos) {
					return false;
				}
			}
		}
		return a_actor->IsInFaction(sosfaction);
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

	bool GetIsBed(const RE::TESObjectREFR* a_reference)
	{
		if (a_reference->GetName()[0] == '\0')
			return false;
		const auto root = a_reference->Get3D();
		const auto extra = root ? root->GetExtraData("FRN") : nullptr;
		const auto node = extra ? netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra) : nullptr;
		return node && !node->markers.empty() && node->markers[0].animationType.all(RE::BSFurnitureMarker::AnimationType::kSleep);
	}
}
