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
		this->depthPriority = DEPTH_PRIORITY;
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
		// RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(this);

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

		// const auto controls = RE::ControlMap::GetSingleton();
		const auto input = RE::BSInputDeviceManager::GetSingleton();
		switch (*a_message.type) {
		case Type::kShow:
			input->AddEventSink<RE::InputEvent*>(this);
			// mainFourEnabled = controls->AreControlsEnabled(RE::ControlMap::UEFlag::kMainFour);
			// controls->ToggleControls(RE::ControlMap::UEFlag::kMainFour, false);
			// RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			// RE::UIMessageQueue::GetSingleton()->AddMessage(TRUE_HUD_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			return Result::kHandled;
		case Type::kHide:
			input->RemoveEventSink(this);
			// isShowing = false;
			// __fallthrough;
			// case Type::kForceHide:
			// if (!controls->AreControlsEnabled(RE::ControlMap::UEFlag::kMainFour)) {
			// 	controls->ToggleControls(RE::ControlMap::UEFlag::kMainFour, mainFourEnabled);
			// }
			// RE::UIMessageQueue::GetSingleton()->AddMessage(RE::HUDMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
			// RE::UIMessageQueue::GetSingleton()->AddMessage(TRUE_HUD_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
			return Result::kHandled;
		case Type::kUserEvent:
			// if (!RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode()) {
			// 	if (const auto& data = static_cast<RE::BSUIMessageData*>(a_message.data)) {
			// 		std::string navEq{ "" };
			// 		const auto events = RE::UserEvents::GetSingleton();
			// 		if (data->fixedStr == events->strafeRight) {
			// 			navEq = "right";
			// 		} else if (data->fixedStr == events->strafeLeft) {
			// 			navEq = "left";
			// 		} else if (data->fixedStr == events->forward) {
			// 			navEq = "up";
			// 		} else if (data->fixedStr == events->back) {
			// 			navEq = "down";
			// 		} else {
			// 			return Result::kPassOn;
			// 		}
			// 		RE::GFxValue arg{ navEq.c_str() };
			// 		this->uiMovie->InvokeNoReturn("_root.main.handleInputEx", &arg, 1);
			// 		return Result::kHandled;
			// 	}
			// }
			// __fallthrough;
		case Type::kScaleformEvent:
			return Result::kPassOn;
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}

	// RE::BSEventNotifyControl SceneMenu::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	// {
	// 	using EventResult = RE::BSEventNotifyControl;
	// 	if (!isShowing || !a_event) {
	// 		return EventResult::kContinue;
	// 	}
	// 	const auto ui = RE::UI::GetSingleton();
	// 	const auto menu = ui->GetMenu(a_event->menuName);
	// 	if (!menu || menu->depthPriority >= DEPTH_PRIORITY) {
	// 		return EventResult::kContinue;
	// 	} else if (!menu->InventoryItemMenu() && !menu->ApplicationMenu()) {
	// 		if (a_event->opening) {
	// 			RE::UIMessageQueue::GetSingleton()->AddMessage(a_event->menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
	// 		}
	// 	} else {
	// 		if (a_event->opening) {
	// 			Hide();
	// 		} else {
	// 			Show();
	// 		}
	// 	}
	// 	return EventResult::kContinue;
	// }

	RE::BSEventNotifyControl SceneMenu::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
	{
		using EventResult = RE::BSEventNotifyControl;
		if (!a_event) {
			return EventResult::kContinue;
		}
		const auto ui = RE::UI::GetSingleton();
		if (ui->IsMenuOpen(RE::Console::MENU_NAME) || ui->GameIsPaused() || ui->IsApplicationMenuOpen())
			return EventResult::kContinue;
		for (const RE::InputEvent* input = *a_event; input; input = input->next) {
			const auto buttonEvent = input->AsButtonEvent();
			if (!buttonEvent || !buttonEvent->IsDown())
				continue;
			const auto idEvent = input->AsIDEvent();
			if (RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode()) {
				constexpr std::array directionEvents{
					"Forward"sv,
					"Back"sv,
					"Strafe Left"sv,
					"Strafe Right"sv
				};
				const auto& userEvent = idEvent->QUserEvent();
				const auto directionEvent = std::ranges::find_if(directionEvents, [&](const auto& e) { return userEvent == e; });
				if (directionEvent != directionEvents.end()) {
					continue;
				}
			}
			// https://ck.uesp.net/wiki/Input_Script#DXScanCodes
			const auto dxCode = idEvent->GetIDCode();
			std::string navEq{ "" };
			switch (dxCode) {
			case 0x39:	// Space
				navEq = "enter-gamepad_A";
				break;
			case 0x11:	// W
				navEq = "up";
				break;
			case 0x1E:	// A
				navEq = "left";
				break;
			case 0x1F:	// S
				navEq = "down";
				break;
			case 0x20:	// D
				navEq = "right";
				break;
			case 0xCF:	// End
				{
					SKSE::ModCallbackEvent modEvent{ "SL_EndScene" };
					SKSE::GetModCallbackEventSource()->SendEvent(&modEvent);
					continue;
				}
			}
			RE::GFxValue arg{ navEq.c_str() };
			this->uiMovie->InvokeNoReturn("_root.main.handleInputEx", &arg, 1);
		}
		return EventResult::kContinue;
	}

	void SceneMenu::HUDMenu_ShowMessageEx::Call(Params& a_args)
	{
		const auto ui = RE::UI::GetSingleton();
		if (SceneMenu::IsOpen()) {
			auto scene = ui->GetMovieView(SceneMenu::NAME);
			scene->InvokeNoReturn("_root.main.ShowMessage", a_args.args, a_args.argCount);
		} else {
			auto hud = ui->GetMovieView(RE::HUDMenu::MENU_NAME);
			hud->InvokeNoReturn("_root.HUDMovieBaseInstance.ShowMessage_SEXLABREROUTE", a_args.args, a_args.argCount);
		}
	}

}
