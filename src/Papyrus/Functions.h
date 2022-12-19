#pragma once

namespace SLPP
{
#define REGISTERFUNC(func, c) a_vm->RegisterFunction(#func##sv, c, func)

	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;

	std::vector<std::string> MergeStringArrayEx(RE::StaticFunctionTag*, std::vector<std::string> a_array1, std::vector<std::string> a_array2, bool a_removedupes);
	std::vector<std::string> RemoveStringEx(RE::StaticFunctionTag*, std::vector<std::string> a_array, std::string a_remove);

	void SetPositions(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::Actor*> a_positions, RE::TESObjectREFR* a_center);
	RE::TESAmmo* GetEquippedAmmo(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);

	bool MatchTags(RE::StaticFunctionTag*, std::vector<std::string_view> a_tags, std::vector<std::string_view> a_match);

	std::vector<RE::TESObjectREFR*> FindBeds(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* akCenterRef, float a_radius, float a_radiusz);
	bool IsBed(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::TESObjectREFR* a_reference);

	RE::SpellItem* GetHDTHeelSpell(VM* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_reference);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(MergeStringArrayEx, "sslpp");
		REGISTERFUNC(RemoveStringEx, "sslpp");
		REGISTERFUNC(SetPositions, "sslpp");
		REGISTERFUNC(GetEquippedAmmo, "sslpp");
		REGISTERFUNC(MatchTags, "sslpp");
		REGISTERFUNC(FindBeds, "sslpp");
		REGISTERFUNC(IsBed, "sslpp");
		REGISTERFUNC(GetHDTHeelSpell, "sslpp");

		return true;
	}
}  // namespace SLPP
