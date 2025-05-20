#include "SelectionMenu.h"

#include "Interface.h"

namespace Thread::Interface
{
	SelectionMenu::SelectionMenu() :
		RE::IMenu()
	{
		this->inputContext = Context::kMenuMode;
		this->depthPriority = 3;
		this->menuFlags.set(
      Flag::kPausesGame,
			Flag::kUsesMenuContext,
			Flag::kCustomRendering,
			Flag::kApplicationMenu);

		auto dmanager = RE::BSInputDeviceManager::GetSingleton();
		if (!dmanager->IsGamepadEnabled()) {
			this->menuFlags.set(Flag::kUsesCursor);
		}

		auto scaleform = RE::BSScaleformManager::GetSingleton();
		[[maybe_unused]] bool success = scaleform->LoadMovieEx(this, FILEPATH, [](RE::GFxMovieDef* a_def) -> void {
			a_def->SetState(
				RE::GFxState::StateType::kLog,
				RE::make_gptr<FlashLogger<SelectionMenu>>().get());
		});
		assert(success);

		auto view = this->uiMovie;
		view->SetMouseCursorCount(this->menuFlags.all(Flag::kUsesCursor) ? 1 : 0);
		FunctionManager::AttachSKSEFunctions(view);

		RE::GFxValue main;
		success = view->GetVariable(&main, "_root.main");
		assert(success);
		FunctionManager::AttachFunction<SetSelection>(view, main, "SetSelection");
	}

	std::vector<SelectionMenu::Item>::const_iterator SelectionMenu::CreateSelectionAndWait(const std::vector<Item>& a_items)
	{
		std::unique_lock lock{ _m };
		items = &a_items;
		Show();
		_cv.wait(lock);
		return selectedItem;
	}

	RE::UI_MESSAGE_RESULTS SelectionMenu::ProcessMessage(RE::UIMessage& a_message)
	{
		using Type = RE::UI_MESSAGE_TYPE;
		using Result = RE::UI_MESSAGE_RESULTS;

		switch (*a_message.type) {
		case Type::kShow:
			{
				assert(items);
				std::vector<RE::GFxValue> values{};
				values.reserve(items->size());
				for (size_t i = 0; i < items->size(); i++) {
					const auto& item = items->at(i);
					RE::GFxValue value;
					this->uiMovie->CreateObject(&value);
					RE::GFxValue index{ static_cast<double>(i) };
					value.SetMember("name", item.GetGFxName());
					value.SetMember("type", item.GetGFxValue());
					value.SetMember("index", index);
					values.push_back(value);
				}
				SelectionMenu::selectedItem = items->end();
				this->uiMovie->InvokeNoReturn("_root.main.loadList", values.data(), static_cast<uint32_t>(values.size()));
			}
			return Result::kHandled;
		case Type::kForceHide:
		case Type::kHide:
			_cv.notify_all();
			return Result::kHandled;
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}

	void SelectionMenu::SetSelection::Call(Params& a_args)
	{
		assert(a_args.argCount > 0);
		const auto index = a_args.args->GetUInt();
		if (index >= items->size()) {
			logger::error("Invalid index {} for selection menu", index);
			SelectionMenu::selectedItem = items->end();
		} else {
			SelectionMenu::selectedItem = items->begin() + index;
		}
		SelectionMenu::Hide();
	}

}	 // namespace Thread::Interface
