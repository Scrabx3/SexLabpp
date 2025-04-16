#pragma once

#include "Registry/Define/Animation.h"
#include "Thread/Thread.h"

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
	};
	KeyType GetKeyType(uint32_t a_keyCode);
	uint32_t GetKeyCode(KeyType a_keyType);

	class SceneMenu :
		public RE::IMenu,
		public RE::BSTEventSink<RE::InputEvent*>
	{
		using GRefCountBaseStatImpl::operator new;
		using GRefCountBaseStatImpl::operator delete;

		static constexpr std::string_view EVENT_END_SCENE{ "SL_EndScene" };

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

	private:
		static inline Instance* threadInstance{ nullptr };

		struct GFxFunctionHandlerWrapper : public RE::GFxFunctionHandler
		{
		protected:
			RE::Actor* GetActorByReferenceId(Params& a_args, size_t argIdx);
		};

			// clang-format off
		struct GetHotkeyCombination : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetActiveSceneName : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetActiveSceneAuthor : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetActiveSceneOrigin : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetActiveSceneTags : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetActiveAnnotations : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct SetActiveSceneAnnotations : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetOffset : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct SetOffset : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct ResetOffsets : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetOffsetStepSize : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct AdjustOffsetStepSize : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetAdjustStageOnly : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct SetAdjustStageOnly : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetActiveFurnitureName : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetAlternateScenes : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct SetActiveScene : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct PickRandomScene : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct ToggleAnimationPaused : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct ToggleAutoPlay : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct IsAutoPlay : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct MoveScene : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct EndScene : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetPositions : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetPermutationData : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct SelectNextPermutation : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetGhostMode : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct SetGhostMode : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetExpressionName : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct SetExpression : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetExpressions : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetVoiceName : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct SetVoice : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		struct GetVoices : public GFxFunctionHandlerWrapper { void Call(Params& a_args) override; };
		// clang-format on

	private:
		struct HUDMenu_ShowMessageEx : public RE::GFxFunctionHandler
		{
			void Call(Params& a_args) override;
		};
	};
}
