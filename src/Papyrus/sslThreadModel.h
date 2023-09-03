#pragma once

namespace Papyrus::ThreadModel
{
	enum FurniStatus
	{
		Disallow = 0,
		Allow = 1,
		Prefer = 2,
	};

	RE::TESObjectREFR* FindCenter(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst,
		const std::vector<RE::BSFixedString> a_scenes, const RE::BSFixedString a_preferedscene, std::vector<RE::BSFixedString> a_out, FurniStatus a_status);
	bool GetIsCompatiblecenter(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::BSFixedString a_sceneid, RE::TESObjectREFR* a_center);

	std::vector<float> GetBaseCoordinates(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, RE::BSFixedString a_scene);
	RE::BSFixedString PlaceAndPlay(VM* a_vm, StackID a_stackID, RE::TESQuest*,
		std::vector<RE::Actor*> a_positions, std::vector<float> a_coordinates, RE::BSFixedString a_scene, RE::BSFixedString a_stage);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(FindCenter, "sslThreadModel", true);
		REGISTERFUNC(GetIsCompatiblecenter, "sslThreadModel", true);

		REGISTERFUNC(GetBaseCoordinates, "sslThreadModel", true);
		REGISTERFUNC(PlaceAndPlay, "sslThreadModel", false);

		return true;
	}

}	 // namespace Papyrus::ThreadModel
