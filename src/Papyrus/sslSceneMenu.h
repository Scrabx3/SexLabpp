#pragma once

#include "Registry/Interface/SceneMenu.h"

namespace Papyrus::SceneMenu
{
	inline void OpenMenu(RE::StaticFunctionTag*) { Registry::Interface::SceneMenu::Show(); }
	inline void CloseMenu(RE::StaticFunctionTag*) { Registry::Interface::SceneMenu::Hide(); }
	inline bool IsMenuOpen(RE::StaticFunctionTag*) { return Registry::Interface::SceneMenu::IsOpen(); }

	void SetPositions(RE::StaticFunctionTag*, std::vector<RE::Actor*> a_positions);
	void UpdateEnjoyment(RE::StaticFunctionTag*, RE::Actor* a_position, float a_enjoyment);
	void SetEnjoyment(RE::StaticFunctionTag*, RE::Actor* a_position, float a_enjoyment);

	inline bool Register(VM* a_vm)
	{
    REGISTERFUNC(OpenMenu, "sslSceneMenu", true);
    REGISTERFUNC(CloseMenu, "sslSceneMenu", true);
    REGISTERFUNC(IsMenuOpen, "sslSceneMenu", true);

    REGISTERFUNC(SetPositions, "sslSceneMenu", true);
    REGISTERFUNC(UpdateEnjoyment, "sslSceneMenu", true);
    REGISTERFUNC(SetEnjoyment, "sslSceneMenu", true);

		return true;
	}
}	 // namespace Papyrus::SceneMenu
