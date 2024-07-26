#include "SceneMenu.h"

#include "Interface.h"

namespace Registry::Interface
{
	///
	/// Menu
	///

	SceneMenu::SceneMenu() :
		RE::IMenu()
	{
		this->inputContext = Context::kMenuMode;
		this->depthPriority = 3;
		this->menuFlags.set(
			Flag::kCustomRendering,
			Flag::kAssignCursorToRenderer);

		auto dmanager = RE::BSInputDeviceManager::GetSingleton();
		if (!dmanager->IsGamepadEnabled()) {
			this->menuFlags.set(Flag::kUsesCursor);
		}

		auto scaleform = RE::BSScaleformManager::GetSingleton();
		[[maybe_unused]] bool success = scaleform->LoadMovieEx(this, FILEPATH, [](RE::GFxMovieDef* a_def) -> void {
			a_def->SetState(
				RE::GFxState::StateType::kLog,
				RE::make_gptr<FlashLogger<SceneMenu>>().get());
		});
		assert(success);

		auto view = this->uiMovie;
		view->SetMouseCursorCount(0);
		FunctionManager::AttachSKSEFunctions(view);
	}

	void SceneMenu::Register()
	{
		RE::UI::GetSingleton()->Register(NAME, Create);
		logger::info("Registered Scene Menu Menu");
	}

	RE::UI_MESSAGE_RESULTS SceneMenu::ProcessMessage(RE::UIMessage& a_message)
	{
		using Type = RE::UI_MESSAGE_TYPE;
		using Result = RE::UI_MESSAGE_RESULTS;

		switch (*a_message.type) {
		case Type::kShow:
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}
}