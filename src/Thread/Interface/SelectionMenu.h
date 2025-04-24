#pragma once

namespace Thread::Interface
{
	template <typename T>
	concept Iterable = requires (T a)	{
		typename T::iterator;
		{ a.begin() }->std::input_iterator;
		{ a.end() }->std::input_iterator;
	};

	class SelectionMenu :
		public RE::IMenu
	{
		using GRefCountBaseStatImpl::operator new;
		using GRefCountBaseStatImpl::operator delete;

	public:
		struct Item
		{
			Item(std::string_view a_name, std::string_view a_value) :
				name(a_name), value(a_value) {}

			RE::GFxValue name;
			RE::GFxValue value;
		};

	public:
		static constexpr std::string_view MENU_NAME{ "SLSelectionMenu" };
		static constexpr std::string_view FILEPATH{ "SexLab\\SelectionMenu" };

		SelectionMenu();
		~SelectionMenu() = default;
		static void Register() { (RE::UI::GetSingleton()->Register(MENU_NAME, Create), logger::info("Registered Menu: {}", MENU_NAME)); }
		static RE::IMenu* Create() { return new SelectionMenu(); }

		static std::vector<Item>::const_iterator CreateSelectionAndWait(const std::vector<Item>& a_items);

	public:
		static void Show() { RE::UIMessageQueue::GetSingleton()->AddMessage(MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr); }
		static void Hide() { RE::UIMessageQueue::GetSingleton()->AddMessage(MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr); }
		static bool IsOpen() { return RE::UI::GetSingleton()->IsMenuOpen(MENU_NAME); }

	protected:
		// IMenu
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

	private:
		struct SetSelection : public RE::GFxFunctionHandler
		{
			void Call(Params& a_args) override;
		};

	private:
		static inline std::mutex _m;
		static inline std::condition_variable _cv;
		static inline const std::vector<Item>* items;
		static inline std::vector<Item>::const_iterator selectedItem;
	};
}
