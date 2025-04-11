#include "SceneMenu.h"

#include "Interface.h"

namespace Thread::Interface
{
	KeyType GetKeyType(uint32_t a_keyCode)
	{
		const auto get = [](uint32_t key) {
			return key >= SKSE::InputMap::kMacro_GamepadOffset ? SKSE::InputMap::GamepadKeycodeToMask(key) : key;
		};
		if (a_keyCode == get(Settings::iKeyUp))
			return KeyType::Up;
		if (a_keyCode == get(Settings::iKeyDown))
			return KeyType::Down;
		if (a_keyCode == get(Settings::iKeyLeft))
			return KeyType::Left;
		if (a_keyCode == get(Settings::iKeyRight))
			return KeyType::Right;
		if (a_keyCode == get(Settings::iKeyAdvance))
			return KeyType::Select;
		if (a_keyCode == get(Settings::iKeyEnd))
			return KeyType::End;
		if (a_keyCode == get(Settings::iKeyExtra1))
			return KeyType::Extra1;
		if (a_keyCode == get(Settings::iKeyExtra2))
			return KeyType::Extra2;
		if (a_keyCode == get(Settings::iKeyMod))
			return KeyType::Modes;
		if (a_keyCode == get(Settings::iKeyReset))
			return KeyType::Reset;
		return KeyType::None;
	}

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

		const auto input = RE::BSInputDeviceManager::GetSingleton();
		switch (*a_message.type) {
		case Type::kShow:
			input->AddEventSink<RE::InputEvent*>(this);
			return Result::kHandled;
		case Type::kHide:
			input->RemoveEventSink(this);
			return Result::kHandled;
		case Type::kUserEvent:
		case Type::kScaleformEvent:
			return Result::kPassOn;
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}

	RE::BSEventNotifyControl SceneMenu::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
	{
		using EventResult = RE::BSEventNotifyControl;
		if (!a_event) {
			return EventResult::kContinue;
		}
		const auto ui = RE::UI::GetSingleton();
		if (ui->IsMenuOpen(RE::Console::MENU_NAME) || ui->GameIsPaused() || ui->IsApplicationMenuOpen())
			return EventResult::kContinue;
		KeyType navType = KeyType::None;
		bool mode = false;
		bool reset = false;
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
			const auto dxCode = idEvent->GetIDCode();
			const auto type = GetKeyType(dxCode);
			switch (type) {
			case KeyType::None:
				break;
			case KeyType::Reset:
				reset = true;
				break;
			case KeyType::Modes:
				mode = true;
				break;
			default:
				navType = type;
				break;
			}
		}
		if (navType == KeyType::None)
			return EventResult::kContinue;
		auto enumName = magic_enum::enum_name(navType);
		if (enumName.empty())
			return EventResult::kContinue;
		std::string navEq{ enumName };
		Util::ToLower(navEq);
		std::vector<RE::GFxValue> args{
			RE::GFxValue(navEq.c_str()),
			RE::GFxValue(mode),
			RE::GFxValue(reset)
		};
		this->uiMovie->InvokeNoReturn("_root.main.handleInputEx", args.data(), static_cast<uint32_t>(args.size()));
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
