#pragma once

namespace Thread::Interface
{
	enum class KeyType
	{
		None,
		Up,
		Down,
		Left,
		Right,
		Select,
		End,
		Extra1,
		Extra2,
		Modes,
		Reset,
		Mouse,
	};
	KeyType GetKeyType(uint32_t a_keyCode);

	class SceneMenu :
		public RE::IMenu,
		public RE::BSTEventSink<RE::InputEvent*>
		// public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
		using GRefCountBaseStatImpl::operator new;
		using GRefCountBaseStatImpl::operator delete;

	public:
		static constexpr std::string_view NAME{ "SLSceneMenu" };
		static constexpr std::string_view FILEPATH{ "SexLab\\SceneMenu" };
		static constexpr int8_t DEPTH_PRIORITY{ 4 }; 

		SceneMenu();
		~SceneMenu() = default;
		static void Register();
		static RE::IMenu* Create() { return new SceneMenu(); }

	public:
		static void Show() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr); }
		static void Hide() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr); }
		static bool IsOpen() { return RE::UI::GetSingleton()->IsMenuOpen(NAME); }

	protected:
		// IMenu
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

		// Events
		RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*) override;
		// RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;

	private:
		static constexpr std::string_view TRUE_HUD_NAME{ "TrueHUD" };
		bool mainFourEnabled{ true };

		struct HUDMenu_ShowMessageEx : public RE::GFxFunctionHandler
		{
			void Call(Params& a_args) override;
		};
	};
}
