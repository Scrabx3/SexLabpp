#include "SceneMenu.h"

#include "Interface.h"
#include "Registry/Library.h"

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

	uint32_t GetKeyCode(KeyType a_keyType)
	{
		const auto get = [](uint32_t key) {
			return key >= SKSE::InputMap::kMacro_GamepadOffset ? SKSE::InputMap::GamepadKeycodeToMask(key) : key;
		};
		switch (a_keyType) {
		case KeyType::Up:
			return get(Settings::iKeyUp);
		case KeyType::Down:
			return get(Settings::iKeyDown);
		case KeyType::Left:
			return get(Settings::iKeyLeft);
		case KeyType::Right:
			return get(Settings::iKeyRight);
		case KeyType::Select:
			return get(Settings::iKeyAdvance);
		case KeyType::End:
			return get(Settings::iKeyEnd);
		case KeyType::Extra1:
			return get(Settings::iKeyExtra1);
		case KeyType::Extra2:
			return get(Settings::iKeyExtra2);
		case KeyType::Modes:
			return get(Settings::iKeyMod);
		case KeyType::Reset:
			return get(Settings::iKeyReset);
		default:
			return 0; // Return 0 for KeyType::None or invalid KeyType
		}
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

	RE::Actor* SceneMenu::GFxFunctionHandlerWrapper::GetActorByReferenceId(Params& a_args, size_t argIdx)
	{
		assert(a_args.argCount > argIdx);
		if (a_args.args[argIdx].GetType() != RE::GFxValue::ValueType::kNumber) {
			logger::warn("GetPermutationData: Expected number argument");
			return;
		}
		const auto formId = static_cast<int32_t>(a_args.args->GetNumber());
		const auto positions = threadInstance->GetActors();
		for (auto&& actor : positions) {
			if (actor->GetFormID() != formId) {
				continue;
			}
			return actor;
		}
		logger::warn("GetActorByReferenceId: Invalid form ID {}", formId);
		return nullptr;
	}

	void Thread::Interface::SceneMenu::GetHotkeyCombination::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		auto& idArg = a_args.args[0];
		if (idArg.GetType() != RE::GFxValue::ValueType::kString) {
			logger::warn("GetHotkeyCombination: Expected string argument");
			return a_args.retVal->SetString("");
		}
		std::string idStr{ idArg.GetString() };
		const auto enumName = magic_enum::enum_cast<KeyType>(idStr, magic_enum::case_insensitive);
		const auto keyCode = GetKeyCode(enumName.value_or(KeyType::None));
		if (keyCode == 0) {
			logger::warn("GetHotkeyCombination: Invalid key type {}", idStr);
			return a_args.retVal->SetString("");
		}
		const auto nameStr = SKSE::InputMap::GetKeyName(keyCode);
		a_args.retVal->SetString(nameStr.c_str());
	}

	void Thread::Interface::SceneMenu::GetActiveSceneName::Call(Params& a_args)
	{
		const auto activeScene = threadInstance->GetActiveScene();
		a_args.retVal->SetString(activeScene->name.c_str());
	}

	void Thread::Interface::SceneMenu::GetActiveSceneAuthor::Call(Params& a_args)
	{
		const auto activeScene = threadInstance->GetActiveScene();
		const auto package = Registry::Library::GetSingleton()->GetPackageFromScene(activeScene);
		assert(package);
		a_args.retVal->SetString(package->GetAuthor().c_str());
	}

	void Thread::Interface::SceneMenu::GetActiveSceneOrigin::Call(Params& a_args)
	{
		const auto activeScene = threadInstance->GetActiveScene();
		const auto package = Registry::Library::GetSingleton()->GetPackageFromScene(activeScene);
		assert(package);
		a_args.retVal->SetString(package->GetName().c_str());
	}

	void Thread::Interface::SceneMenu::GetActiveSceneTags::Call(Params& a_args)
	{
		const auto activeScene = threadInstance->GetActiveScene();
		const auto tags = activeScene->tags.AsVector();
		for (const auto& tag : tags) {
			RE::GFxValue value{ RE::GFxValue::ValueType::kString };
			value.SetString(tag.c_str());
			a_args.retVal->PushBack(value);
		}
	}

	void Thread::Interface::SceneMenu::GetActiveAnnotations::Call(Params& a_args)
	{
		const auto activeScene = threadInstance->GetActiveScene();
		for (const auto& annotation : activeScene->annotations) {
			RE::GFxValue value;
			value.SetString(annotation.c_str());
			a_args.retVal->PushBack(value);
		}
	}

	void Thread::Interface::SceneMenu::SetActiveSceneAnnotations::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		const auto activeScene = threadInstance->GetActiveScene();
		if (a_args.args->GetType() != RE::GFxValue::ValueType::kArray) {
			logger::warn("SetActiveSceneAnnotations: Expected string argument");
			return;
		}
		activeScene->annotations.clear();
		for (std::uint32_t i = 0; i < a_args.args->GetArraySize(); ++i) {
			RE::GFxValue value;
			a_args.args->GetElement(i, &value);
			if (value.GetType() != RE::GFxValue::ValueType::kString) {
				logger::warn("SetActiveSceneAnnotations: Expected string argument");
				return;
			}
			activeScene->annotations.emplace_back(value.GetString());
		}
	}

	void Thread::Interface::SceneMenu::GetOffset::Call(Params& a_args)
	{
		assert(a_args.argCount > 0);
		if (a_args.args[0].GetType() != RE::GFxValue::ValueType::kString) {
			logger::warn("GetOffset: Expected string argument");
			return a_args.retVal->SetNumber(0.0f);
		}
		auto offsetName = a_args.args[0].GetString();
		auto offsetIdx = magic_enum::enum_cast<Registry::CoordinateType>(offsetName, magic_enum::case_insensitive);
		if (!offsetIdx.has_value()) {
			logger::warn("GetOffset: Invalid offset type {}", offsetName);
			return a_args.retVal->SetNumber(0.0f);
		}
		const auto activeScene = threadInstance->GetActiveScene();
		const auto activeStage = threadInstance->GetActiveStage();
		auto& offsets = activeScene->furnitures.offset;
		if (a_args.argCount > 1 && a_args.args[1].GetType() == RE::GFxValue::ValueType::kNumber) {
			const auto positionIdx = static_cast<int32_t>(a_args.args[1].GetNumber());
			if (positionIdx < 0 || positionIdx >= activeStage->positions.size()) {
				logger::warn("GetOffset: Invalid offset index {}", positionIdx);
				return a_args.retVal->SetNumber(0.0f);
			}
			const auto& position = activeStage->positions[positionIdx];
			offsets = position.offset;
		}
		const auto offset = offsets.GetOffset(offsetIdx.value());
		a_args.retVal->SetNumber(offset);
	}

	void Thread::Interface::SceneMenu::SetOffset::Call(Params& a_args)
	{
		assert(a_args.argCount > 1);
		if (a_args.args[0].GetType() != RE::GFxValue::ValueType::kString) {
			logger::warn("GetOffset: Expected string argument");
			return;
		}
		auto offsetName = a_args.args[0].GetString();
		auto offsetIdx = magic_enum::enum_cast<Registry::CoordinateType>(offsetName, magic_enum::case_insensitive);
		if (!offsetIdx.has_value()) {
			logger::warn("GetOffset: Invalid offset type {}", offsetName);
			return;
		}
		if (a_args.args[1].GetType() != RE::GFxValue::ValueType::kNumber) {
			logger::warn("GetOffset: Expected number argument");
			return;
		}
		const auto activeScene = threadInstance->GetActiveScene();
		const auto activeStage = threadInstance->GetActiveStage();
		const auto value = a_args.args[1].GetNumber();
		auto& offsets = activeScene->furnitures.offset;
		if (a_args.argCount > 2 && a_args.args[2].GetType() == RE::GFxValue::ValueType::kNumber) {
			const auto positionIdx = static_cast<int32_t>(a_args.args[2].GetNumber());
			if (positionIdx < 0 || positionIdx >= activeStage->positions.size()) {
				logger::warn("GetOffset: Invalid offset index {}", positionIdx);
				return;
			}
			const auto& position = activeStage->positions[positionIdx];
			offsets = position.offset.GetOffset();
		}
		offsets.SetOffset(value, offsetIdx.value());
	}

	void Thread::Interface::SceneMenu::ResetOffsets::Call(Params& a_args)
	{
		const auto activeScene = threadInstance->GetActiveScene();
		const auto activeStage = threadInstance->GetActiveStage();
		auto& offsets = activeScene->furnitures.offset;
		if (a_args.argCount > 0 && a_args.args[0].GetType() == RE::GFxValue::ValueType::kNumber) {
			const auto positionIdx = static_cast<int32_t>(a_args.args[2].GetNumber());
			if (positionIdx < 0 || positionIdx >= activeStage->positions.size()) {
				logger::warn("GetOffset: Invalid offset index {}", positionIdx);
				return;
			}
			const auto& position = activeStage->positions[positionIdx];
			offsets = position.offset.GetOffset();
		}
		offsets.ResetOffset();
	}

	void Thread::Interface::SceneMenu::GetOffsetStepSize::Call(Params& a_args)
	{
		a_args.retVal->SetNumber(Settings::fAdjustStepSize);
	}

	void Thread::Interface::SceneMenu::AdjustOffsetStepSize::Call(Params& a_args)
	{
		assert(a_args.argCount > 0);
		if (a_args.args->GetType() != RE::GFxValue::ValueType::kBoolean) {
			logger::warn("AdjustOffsetStepSize: Expected boolean argument");
		} else {
			bool upward = a_args.args->GetBool();
			if (upward) {
				Settings::fAdjustStepSize += Settings::fAdjustStepSizeIncrement;
			} else {
				Settings::fAdjustStepSize -= Settings::fAdjustStepSizeIncrement;
			}
		}
		return a_args.retVal->SetNumber(Settings::fAdjustStepSize);
	}

	void Thread::Interface::SceneMenu::GetAdjustStageOnly::Call(Params& a_args)
	{
		a_args.retVal->SetBoolean(Settings::bAdjustStage);
	}

	void Thread::Interface::SceneMenu::SetAdjustStageOnly::Call(Params& a_args)
	{
		assert(a_args.argCount > 0);
		if (a_args.args->GetType() != RE::GFxValue::ValueType::kBoolean) {
			logger::warn("SetAdjustStageOnly: Expected boolean argument");
		} else {
			Settings::bAdjustStage = a_args.args->GetBool();
		}
	}

	void Thread::Interface::SceneMenu::GetActiveFurnitureName::Call(Params& a_args)
	{
		// COMEBACK: Get furniture type name instead of ID
		const auto type = threadInstance->GetFurnitureType();
		const auto name = threadInstance->GetCenterRef()->GetDisplayFullName();
		const auto nameStr = std::format("{} ({})", name, std::to_underlying(type));
		a_args.retVal->SetString(nameStr.c_str());
	}

	void Thread::Interface::SceneMenu::GetAlternateScenes::Call(Params& a_args)
	{
		const auto scenes = threadInstance->GetThreadScenes();
		for (const auto& scene : scenes) {
			RE::GFxValue object;
			a_args.movie->CreateObject(&object);
			RE::GFxValue name{ RE::GFxValue::ValueType::kString };
			RE::GFxValue id{ RE::GFxValue::ValueType::kString };
			name.SetString(scene->name.c_str());
			id.SetString(scene->id.c_str());
			object.SetMember("name", name);
			object.SetMember("id", id);
			a_args.retVal->PushBack(object);
		}
	}

	void Thread::Interface::SceneMenu::SetActiveScene::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		if (a_args.args->GetType() != RE::GFxValue::ValueType::kString) {
			logger::warn("SetActiveScene: Expected string argument");
			return;
		}
		const auto sceneId = a_args.args->GetString();
		const auto scenes = threadInstance->GetThreadScenes();
		for (const auto& scene : scenes) {
			if (scene->id == sceneId) {
				threadInstance->SetActiveScene(scene);
				return;
			}
		}
		logger::warn("SetActiveScene: No such scene registered: {}", sceneId);
	}

	void Thread::Interface::SceneMenu::PickRandomScene::Call(Params&)
	{
		const auto scenes = threadInstance->GetThreadScenes();
		if (scenes.size() <= 1) {
			logger::warn("PickRandomScene: No other scenes available");
			return;
		}
		const auto activeScene = threadInstance->GetActiveScene();
		do {
			const auto i = Random::draw<size_t>(0, scenes.size() - 1);
			if (scenes[i] != activeScene) {
				threadInstance->SetActiveScene(scenes[i]);
				return;
			}
		} while (true);
	}

	void Thread::Interface::SceneMenu::ToggleAnimationPaused::Call(Params& a_args)
	{
		threadInstance->SetAnimationPaused(!threadInstance->GetAnimationPaused());
	}

	void Thread::Interface::SceneMenu::ToggleAutoPlay::Call(Params& a_args)
	{
		threadInstance->SetAutoplayEnabled(!threadInstance->GetAutoplayEnabled());
	}

	void Thread::Interface::SceneMenu::IsAutoPlay::Call(Params& a_args)
	{
		a_args.retVal->SetBoolean(threadInstance->GetAutoplayEnabled());
	}

	void Thread::Interface::SceneMenu::MoveScene::Call(Params& a_args)
	{
		threadInstance->RequstMoveScene();
	}

	void Thread::Interface::SceneMenu::EndScene::Call(Params& a_args)
	{
		threadInstance->RequestEndScene();
	}

	void Thread::Interface::SceneMenu::GetPositions::Call(Params& a_args)
	{
		const auto positions = threadInstance->GetActors();
		for (const auto& position : positions) {
			const auto& infoData = threadInstance->GetPositionInfo(position)->data;
			RE::GFxValue value{ RE::GFxValue::ValueType::kObject }, name{ RE::GFxValue::ValueType::kString };
			RE::GFxValue id{ RE::GFxValue::ValueType::kNumber };
			auto nameStr = (infoData.IsSubmissive() ? "[S] "s : ""s) + position->GetDisplayFullName();
			name.SetString(nameStr);
			id.SetNumber(position->GetFormID());
			a_args.movie->CreateObject(&value);
			value.SetMember("name", name);
			value.SetMember("id", id);
			a_args.retVal->PushBack(value);
		}
	}

	void Thread::Interface::SceneMenu::GetPermutationData::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("GetPermutationData: Invalid actor reference ID");
			return;
		}
		const auto totalPermutations = threadInstance->GetUniquePermutations(actor);
		const auto currentPermutation = threadInstance->GetCurrentPermutation(actor);
		RE::GFxValue value{ RE::GFxValue::ValueType::kObject };
		a_args.movie->CreateObject(&value);
		RE::GFxValue total{ RE::GFxValue::ValueType::kNumber }, current{ RE::GFxValue::ValueType::kNumber };
		total.SetNumber(totalPermutations);
		current.SetNumber(currentPermutation);
		value.SetMember("total", total);
		value.SetMember("current", current);
		a_args.retVal->PushBack(value);
	}

	void Thread::Interface::SceneMenu::SelectNextPermutation::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("SelectNextPermutation: Invalid actor reference ID");
			return;
		}
		threadInstance->SetNextPermutation(actor);
	}

	void Thread::Interface::SceneMenu::GetGhostMode::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("GetGhostMode: Invalid actor reference ID");
			return;
		}
		const bool ghostMode = threadInstance->IsGhostMode(actor);
		a_args.retVal->SetBoolean(ghostMode);
	}

	void Thread::Interface::SceneMenu::SetGhostMode::Call(Params& a_args)
	{
		assert(a_args.argCount == 2);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("SetGhostMode: Invalid actor reference ID");
			return;
		}
		if (a_args.args[1].GetType() != RE::GFxValue::ValueType::kBoolean) {
			logger::warn("SetGhostMode: Expected boolean argument");
			return;
		}
		const bool ghostMode = a_args.args[1].GetBool();
		threadInstance->SetGhostMode(actor, ghostMode);
	}

	void Thread::Interface::SceneMenu::GetExpressionName::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("GetExpressionName: Invalid actor reference ID");
			return;
		}
		const auto expression = threadInstance->GetExpression(actor);
		if (!expression) {
			logger::warn("GetExpressionName: No expression found for actor {}", actor->GetDisplayFullName());
			return;
		}
		a_args.retVal->SetString(expression->id.c_str());
	}

	void Thread::Interface::SceneMenu::SetExpression::Call(Params& a_args)
	{
		assert(a_args.argCount == 2);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("SetExpression: Invalid actor reference ID");
			return;
		}
		if (a_args.args[1].GetType() != RE::GFxValue::ValueType::kString) {
			logger::warn("SetExpression: Expected string argument");
			return;
		}
		const auto expressionId = a_args.args[1].GetString();
		const auto expression = Registry::Expression::GetSingleton()->GetProfile(expressionId);
		if (!expression) {
			logger::warn("SetExpression: No such expression registered: {}", expressionId);
			return;
		}
		threadInstance->SetExpression(actor, expression);
	}

	void Thread::Interface::SceneMenu::GetExpressions::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("SetExpression: Invalid actor reference ID");
			return;
		}
		std::vector<const Registry::Expression::Profile*> expressions;
		Registry::Expression::GetSingleton()->ForEachProfile([&](const auto& expression) {
			if (expression.enabled) {
				expressions.push_back(&expression);
			}
			return false;
		});
		for (const auto& expression : expressions) {
			RE::GFxValue object;
			a_args.movie->CreateObject(&object);
			RE::GFxValue id{ RE::GFxValue::ValueType::kString };
			id.SetString(expression->id.c_str());
			object.SetMember("id", id);
			RE::GFxValue name{ RE::GFxValue::ValueType::kString };
			name.SetString(expression->id.c_str());
			object.SetMember("name", name);
			a_args.retVal->PushBack(object);
		}
	}

	void Thread::Interface::SceneMenu::GetVoiceName::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("GetVoiceName: Invalid actor reference ID");
			return;
		}
		const auto voice = threadInstance->GetVoice(actor);
		if (!voice) {
			logger::warn("GetVoiceName: No voice found for actor {}", actor->GetDisplayFullName());
			return;
		}
		a_args.retVal->SetString(voice->displayName.c_str());
	}

	void Thread::Interface::SceneMenu::SetVoice::Call(Params& a_args)
	{
		assert(a_args.argCount == 2);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("SetVoice: Invalid actor reference ID");
			return;
		}
		if (a_args.args[1].GetType() != RE::GFxValue::ValueType::kString) {
			logger::warn("SetVoice: Expected string argument");
			return;
		}
		const auto voiceId = a_args.args[1].GetString();
		const auto voice = Registry::Voice::GetSingleton()->GetVoiceByName(voiceId);
		if (!voice) {
			logger::warn("SetVoice: No such voice registered: {}", voiceId);
			return;
		}
		threadInstance->SetVoice(actor, voice);
	}

	void Thread::Interface::SceneMenu::GetVoices::Call(Params& a_args)
	{
		assert(a_args.argCount == 1);
		const auto actor = GetActorByReferenceId(a_args, 0);
		if (!actor) {
			logger::warn("SetVoice: Invalid actor reference ID");
			return;
		}
		std::vector<const Registry::Voice::VoiceObject*> voices;
		Registry::Voice::GetSingleton()->GetAllVoiceNames(actor);
		for (const auto& voice : voices) {
			RE::GFxValue object;
			a_args.movie->CreateObject(&object);
			RE::GFxValue id{ RE::GFxValue::ValueType::kString };
			id.SetString(voice->name.c_str());
			object.SetMember("id", id);
			RE::GFxValue name{ RE::GFxValue::ValueType::kString };
			name.SetString((voice->displayName.empty() ? voice->name : voice->displayName).c_str());
			object.SetMember("name", name);
			a_args.retVal->PushBack(object);
		}
	}
}
