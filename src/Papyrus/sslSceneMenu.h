#pragma once

#include "Registry/Interface/SceneMenu.h"

namespace Papyrus::SceneMenu
{
	void OpenMenu(RE::StaticFunctionTag*, RE::TESQuest* a_qst);
	void CloseMenu(RE::StaticFunctionTag*, RE::TESQuest* a_qst);
	bool IsMenuOpen(RE::StaticFunctionTag*);

	void SetPositions(RE::StaticFunctionTag*, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions);
	void SetStage(STATICARGS, RE::TESQuest* a_qst, RE::BSFixedString a_sceneId, RE::BSFixedString a_stageId);
	void SetTimer(RE::StaticFunctionTag*, RE::TESQuest* a_qst, float afTime);
	void UpdateEnjoyment(RE::StaticFunctionTag*, RE::TESQuest* a_qst, RE::Actor* a_position, float a_enjoyment);
	void SetEnjoyment(RE::StaticFunctionTag*, RE::TESQuest* a_qst, RE::Actor* a_position, float a_enjoyment);

	inline bool Register(VM* a_vm)
	{
    REGISTERFUNC(OpenMenu, "sslSceneMenu", true);
    REGISTERFUNC(CloseMenu, "sslSceneMenu", true);
    REGISTERFUNC(IsMenuOpen, "sslSceneMenu", true);
    REGISTERFUNC(SetStage, "sslSceneMenu", true);

    REGISTERFUNC(SetPositions, "sslSceneMenu", true);
    REGISTERFUNC(SetStage, "sslSceneMenu", true);
    REGISTERFUNC(SetTimer, "sslSceneMenu", true);
    REGISTERFUNC(UpdateEnjoyment, "sslSceneMenu", true);
    REGISTERFUNC(SetEnjoyment, "sslSceneMenu", true);

		return true;
	}
}	 // namespace Papyrus::SceneMenu
