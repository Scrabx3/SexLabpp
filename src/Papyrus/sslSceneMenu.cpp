#include "sslSceneMenu.h"

namespace Papyrus::SceneMenu
{
	void SetPositions(RE::StaticFunctionTag*, std::vector<RE::Actor*> a_positions)
	{
		SKSE::GetTaskInterface()->AddUITask([a_positions = std::move(a_positions)]() {
			const auto view = RE::UI::GetSingleton()->GetMovieView(Registry::Interface::SceneMenu::NAME);
			std::vector<RE::GFxValue> args{};
			args.reserve(a_positions.size());
			for (const auto& pos : a_positions) {
				if (!pos) {
					logger::error("SetPositions: Actor is null");
					continue;
				}
				RE::GFxValue arg;
				view->CreateObject(&arg);
				arg.SetMember("id", { pos->GetFormID() });
				arg.SetMember("name", { pos->GetName() });
				args.push_back(arg);
			}
			view->InvokeNoReturn("_root.main.setSliders", args.data(), static_cast<uint32_t>(args.size()));
		});
	}

	void UpdateEnjoyment(RE::StaticFunctionTag*, RE::Actor* a_position, float a_enjoyment)
	{
		SKSE::GetTaskInterface()->AddUITask([=]() {
			const auto view = RE::UI::GetSingleton()->GetMovieView(Registry::Interface::SceneMenu::NAME);
			std::vector<RE::GFxValue> args{};
			args.emplace_back(a_position->GetFormID());
			args.emplace_back(a_enjoyment);
			view->InvokeNoReturn("_root.main.updateSliderPct", args.data(), static_cast<uint32_t>(args.size()));
		});
	}

	void SetEnjoyment(RE::StaticFunctionTag*, RE::Actor* a_position, float a_enjoyment)
	{
		SKSE::GetTaskInterface()->AddUITask([=]() {
			const auto view = RE::UI::GetSingleton()->GetMovieView(Registry::Interface::SceneMenu::NAME);
			std::vector<RE::GFxValue> args{};
			args.emplace_back(a_position->GetFormID());
			args.emplace_back(a_enjoyment);
			view->InvokeNoReturn("_root.main.setSliderPct", args.data(), static_cast<uint32_t>(args.size()));
		});
	}

}	 // namespace Papyrus::SceneMenu
