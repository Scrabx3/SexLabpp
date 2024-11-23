#pragma once

#include "Registry/Interface/SceneMenu.h"

namespace Papyrus::SceneMenu
{
	void OpenMenu(RE::StaticFunctionTag*) { Registry::Interface::SceneMenu::Show(); }
	void CloseMenu(RE::StaticFunctionTag*) { Registry::Interface::SceneMenu::Hide(); }
	bool IsMenuOpen(RE::StaticFunctionTag*) { Registry::Interface::SceneMenu::IsOpen(); }

	void SetPositions(STATICARGS, std::vector<RE::Actor*> a_positions);
	void UpdateEnjoyment(STATICARGS, RE::Actor* a_position, float a_enjoyment);
	void SetEnjoyment(STATICARGS, RE::Actor* a_position, float a_enjoyment);

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
