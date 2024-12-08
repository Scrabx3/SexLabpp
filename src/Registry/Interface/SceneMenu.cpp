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
		this->inputContext = Context::kNone;
		this->depthPriority = 6;
		this->menuFlags.set(
			Flag::kAlwaysOpen,
			Flag::kCustomRendering,
			Flag::kAssignCursorToRenderer);

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

		static auto hijackShowMessage = [&]() {
			auto hud = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME);
			RE::GFxValue hudMain, showMessage;
			success = hud->GetVariable(&hudMain, "_root.HUDMovieBaseInstance");
			assert(success);
			success = hudMain.GetMember("ShowMessage", &showMessage);
			assert(success);
			success = hudMain.SetMember("ShowMessage_SEXLABREROUTE", showMessage);
			assert(success);

			RE::GFxFunctionHandler* fn = new HUDMenu_ShowMessageEx;
			RE::GFxValue dst;
			hud->CreateFunction(&dst, fn);
			success = hudMain.SetMember("ShowMessage", dst);
			assert(success);
			return 0;
		}();
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

		const auto controls = RE::ControlMap::GetSingleton();
		switch (*a_message.type) {
		case Type::kShow:
			mainFourEnabled = controls->AreControlsEnabled(RE::ControlMap::UEFlag::kMainFour);
			menuEnabled = controls->AreControlsEnabled(RE::ControlMap::UEFlag::kMenu);
			controls->ToggleControls(RE::ControlMap::UEFlag::kMainFour, false);
			controls->ToggleControls(RE::ControlMap::UEFlag::kMenu, false);
			RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			return Result::kHandled;
		case Type::kForceHide:
		case Type::kHide:
			controls->ToggleControls(RE::ControlMap::UEFlag::kMainFour, mainFourEnabled);
			controls->ToggleControls(RE::ControlMap::UEFlag::kMenu, menuEnabled);
			RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
			return Result::kHandled;
		case Type::kUserEvent:
			if (!RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode()) {
				if (const auto& data = static_cast<RE::BSUIMessageData*>(a_message.data)) {
					std::string navEq{ "" };
					const auto events = RE::UserEvents::GetSingleton();
					if (data->fixedStr == events->strafeRight) {
						navEq = "right";
					} else if (data->fixedStr == events->strafeLeft) {
						navEq = "left";
					} else if (data->fixedStr == events->forward) {
						navEq = "up";
					} else if (data->fixedStr == events->back) {
						navEq = "down";
					}
					RE::GFxValue arg{ navEq.c_str() };
					this->uiMovie->InvokeNoReturn("_root.main.handleInputEx", &arg, 1);
					return Result::kHandled;
				}
			}
			__fallthrough;
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}

	void HUDMenu_ShowMessageEx::Call(Params& a_args)
	{
		const auto ui = RE::UI::GetSingleton();
		if (SceneMenu::IsOpen()) {
			auto scene = ui->GetMovieView(SceneMenu::NAME);
			scene->InvokeNoReturn("_root.main.ShowMessage", a_args.args, a_args.argCount);
		}
		auto hud = ui->GetMovieView(RE::HUDMenu::MENU_NAME);
		hud->InvokeNoReturn("_root.HUDMovieBaseInstance.ShowMessage_SEXLABREROUTE", a_args.args, a_args.argCount);
	}

}