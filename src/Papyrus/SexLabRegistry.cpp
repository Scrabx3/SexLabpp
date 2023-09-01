#include "SexLabRegistry.h"

#include "Registry/Animation.h"
#include "Registry/Define/RaceKey.h"

namespace Papyrus::SexLabRegistry
{
	int32_t GetRaceID(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Cannot get race id of a none ref", a_stackID);
			return 0;
		}
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_actor);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack(fmt::format("Invalid race key for actor {}", a_actor->formID).c_str(), a_stackID);
			return 0;
		}
		return static_cast<int32_t>(racekey);
	}

	int32_t MapRaceKeyToID(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_racekey)
	{
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_racekey);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack(fmt::format("Invalid race key {}", a_racekey).c_str(), a_stackID);
			return 0;
		}
		return static_cast<int32_t>(racekey);
	}

	std::vector<int32_t> GetRaceIDA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		const auto key = GetRaceID(a_vm, a_stackID, nullptr, a_actor);
		if (key == static_cast<int32_t>(Registry::RaceKey::None)) {
			return {};
		}
		std::vector<int32_t> ret{ key };
		const auto alternate = Registry::RaceHandler::GetVariantKey(Registry::RaceKey(key));
		if (alternate != Registry::RaceKey::None) {
			ret.push_back(static_cast<int32_t>(alternate));
		}
		return ret;
	}

	std::vector<int32_t> MapRaceKeyToIDA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_racekey)
	{
		const auto key = MapRaceKeyToID(a_vm, a_stackID, nullptr, a_racekey);
		if (key == static_cast<int32_t>(Registry::RaceKey::None)) {
			return {};
		}
		std::vector<int32_t> ret{ key };
		const auto alternate = Registry::RaceHandler::GetVariantKey(Registry::RaceKey(key));
		if (alternate != Registry::RaceKey::None) {
			ret.push_back(static_cast<int32_t>(alternate));
		}
		return ret;
	}

	RE::BSFixedString GetRaceKey(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Cannot get race key of a none ref", a_stackID);
			return 0;
		}
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_actor);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack(fmt::format("Invalid race key for race {:X}", a_actor->formID).c_str(), a_stackID);
			return 0;
		}
		return Registry::RaceHandler::AsString(racekey);
	}

	RE::BSFixedString GetRaceKeyByRace(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESRace* a_race)
	{
		if (!a_race) {
			a_vm->TraceStack("Cannot get race key of a none race", a_stackID);
			return 0;
		}
		const auto racekey = Registry::RaceHandler::GetRaceKey(a_race);
		if (racekey == Registry::RaceKey::None) {
			a_vm->TraceStack(fmt::format("Invalid race key for race {:X}", a_race->formID).c_str(), a_stackID);
			return 0;
		}
		return Registry::RaceHandler::AsString(racekey);
	}

	RE::BSFixedString MapRaceIDToRaceKey(RE::StaticFunctionTag*, int32_t a_raceid)
	{
		return Registry::RaceHandler::AsString(Registry::RaceKey(a_raceid));
	}

	std::vector<RE::BSFixedString> GetRaceKeyA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		const auto ids = GetRaceIDA(a_vm, a_stackID, nullptr, a_actor);
		std::vector<RE::BSFixedString> ret{};
		for (auto&& id : ids) {
			ret.push_back(Registry::RaceHandler::AsString(Registry::RaceKey(id)));
		}
		return ret;
	}

	std::vector<RE::BSFixedString> GetRaceKeyByRaceA(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESRace* a_race)
	{
		if (!a_race) {
			a_vm->TraceStack("Cannot get racekeys from none race", a_stackID);
			return {};
		}
		const auto key = Registry::RaceHandler::GetRaceKey(a_race);
		if (key == Registry::RaceKey::None)
			return {};
		const auto variant = Registry::RaceHandler::GetVariantKey(key);
		if (variant == Registry::RaceKey::None)
			return { Registry::RaceHandler::AsString(key) };
		return { Registry::RaceHandler::AsString(key), Registry::RaceHandler::AsString(variant) };
	}

	std::vector<RE::BSFixedString> MapRaceIDToRaceKeyA(RE::StaticFunctionTag*, int32_t a_raceid)
	{
		const auto key1 = Registry::RaceHandler::AsString(Registry::RaceKey(a_raceid));
		if (key1.empty())
			return {};
		const auto variant = Registry::RaceHandler::GetVariantKey(Registry::RaceKey(a_raceid));
		if (variant == Registry::RaceKey::None)
			return { key1 };
		return { key1, Registry::RaceHandler::AsString(variant) };
	}

	std::vector<RE::BSFixedString> GetAllRaceKeys(RE::StaticFunctionTag*, bool a_ignoreambiguous)
	{
		return Registry::RaceHandler::GetAllRaceKeys(a_ignoreambiguous);
	}

	int32_t GetSex(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, bool a_ignoreoverwrite)
	{
		if (!a_actor) {
			a_vm->TraceStack("Cannot get sex from none ref", a_stackID);
			return 0;
		}
		auto sex = Registry::GetSex(a_actor, a_ignoreoverwrite);
		switch (sex) {
		case Registry::Sex::Male:
			return Registry::IsNPC(a_actor) ? 0 : 3;
		case Registry::Sex::Female:
			return Registry::IsNPC(a_actor)	 ? 1 :
						 Settings::bCreatureGender ? 4 :
																				 3;
		case Registry::Sex::Futa:
			return Registry::IsNPC(a_actor)	 ? 2 :
						 Settings::bCreatureGender ? 4 :
																				 3;
		default:
			return 0;
		}
	}

}	 // namespace Papyrus::SexLabRegistry
