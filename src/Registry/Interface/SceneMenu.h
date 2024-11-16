#pragma once

namespace Registry::Interface
{
	class SceneMenu :
		public RE::IMenu
	{
		using GRefCountBaseStatImpl::operator new;
		using GRefCountBaseStatImpl::operator delete;

	public:
		static constexpr std::string_view NAME{ "SLSceneMenu" };
		static constexpr std::string_view FILEPATH{ "SexLab\\SceneMenu" };

		SceneMenu();
		~SceneMenu() = default;
		static void Register();
		static RE::IMenu* Create() { return new SceneMenu(); }

	public:
		static void Show() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr); }
		static void Hide() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr); }
		static void ForceHide() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kForceHide, nullptr); }
		static bool IsOpen() { return RE::UI::GetSingleton()->IsMenuOpen(NAME); }

	protected:
		// IMenu
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;
	};
	
	struct HUDMenu_ShowMessageEx : public RE::GFxFunctionHandler
	{
		void Call(Params& a_args) override;
	};
}
