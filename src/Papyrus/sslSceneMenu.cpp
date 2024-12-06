#include "sslSceneMenu.h"

#include "Registry/Library.h"

#include "Registry/Library.h"

namespace Papyrus::SceneMenu
{
	inline static RE::FormID sceneMenuId{ 0 };
#define VALIDATE                                   \
	if (!a_qst || a_qst->GetFormID() != sceneMenuId) \
		return;                                        \

	void OpenMenu(RE::StaticFunctionTag*, RE::TESQuest* a_qst)
	{
		if (sceneMenuId != 0)  {
			return;
		}
		sceneMenuId = a_qst->GetFormID();
		Registry::Interface::SceneMenu::Show();
	}

	void CloseMenu(RE::StaticFunctionTag*, RE::TESQuest* a_qst)
	{
		VALIDATE;
		Registry::Interface::SceneMenu::Hide();
		sceneMenuId = 0;
	}

	bool IsMenuOpen(RE::StaticFunctionTag*)
	{
		return Registry::Interface::SceneMenu::IsOpen();
	}

	void SetPositions(RE::StaticFunctionTag*, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions)
	inline static RE::FormID sceneMenuId{ 0 };
#define VALIDATE                                   \
	if (!a_qst || a_qst->GetFormID() != sceneMenuId) \
		return;                                        \

	void OpenMenu(RE::StaticFunctionTag*, RE::TESQuest* a_qst)
	{
		if (sceneMenuId != 0)  {
			return;
		}
		sceneMenuId = a_qst->GetFormID();
		Registry::Interface::SceneMenu::Show();
	}

	void CloseMenu(RE::StaticFunctionTag*, RE::TESQuest* a_qst)
	{
		VALIDATE;
		Registry::Interface::SceneMenu::Hide();
		sceneMenuId = 0;
	}

	bool IsMenuOpen(RE::StaticFunctionTag*)
	{
		return Registry::Interface::SceneMenu::IsOpen();
	}

	void SetPositions(RE::StaticFunctionTag*, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions)
	{
		VALIDATE;
		VALIDATE;
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

	void SetStage(STATICARGS, RE::TESQuest* a_qst, RE::BSFixedString a_sceneId, RE::BSFixedString a_stageId)
	{
		VALIDATE;
		const auto lib = Registry::Library::GetSingleton();
		const auto scene = lib->GetSceneByID(a_sceneId);
		if (!scene) {
			TRACESTACK("SetStage: Scene not found");
			return;
		}
		const auto stage = scene->GetStageByID(a_stageId);
		if (!stage) {
			TRACESTACK("SetStage: Stage not found");
			return;
		}
		SKSE::GetTaskInterface()->AddUITask([=]() {
			const auto view = RE::UI::GetSingleton()->GetMovieView(Registry::Interface::SceneMenu::NAME);
			const auto& edges = scene->GetAdjacentStages(stage);
			std::vector<RE::GFxValue> args{};
			if (edges) {
				args.reserve(edges->size());
				for (const auto& edge : *edges) {
					RE::GFxValue arg;
					view->CreateObject(&arg);
					arg.SetMember("id", { edge->id.c_str() });
					arg.SetMember("name", { edge->navtext.c_str() });
					arg.SetMember("length", { edge->fixedlength > 0 });
					bool hasClimax = false;
					for (const auto& pos : edge->positions) {
						if (pos.climax) {
							hasClimax = true;
							break;
						}
					}
					arg.SetMember("climax", { hasClimax });
					bool endNode = scene->GetNumAdjacentStages(edge) == 0;
					arg.SetMember("end", { endNode });
					args.push_back(arg);
				}
			} else {
				RE::GFxValue arg;
				view->CreateObject(&arg);
				arg.SetMember("id", { "" });
				arg.SetMember("name", { "$SL_EndScene" });
				args.push_back(arg);
			}
			view->InvokeNoReturn("_root.main.setStages", args.data(), static_cast<uint32_t>(args.size()));
		});
	}

	void SetTimer(RE::StaticFunctionTag*, RE::TESQuest* a_qst, float afTime)
	{
		VALIDATE;
		SKSE::GetTaskInterface()->AddUITask([=]() {
			const auto view = RE::UI::GetSingleton()->GetMovieView(Registry::Interface::SceneMenu::NAME);
			RE::GFxValue arg { afTime };
			view->InvokeNoReturn("_root.main.setTimer", &arg, 1);
		});
	}

	void UpdateEnjoyment(RE::StaticFunctionTag*, RE::TESQuest* a_qst, RE::Actor* a_position, float a_enjoyment)
	{
		VALIDATE;
		VALIDATE;
		SKSE::GetTaskInterface()->AddUITask([=]() {
			const auto view = RE::UI::GetSingleton()->GetMovieView(Registry::Interface::SceneMenu::NAME);
			std::vector<RE::GFxValue> args{};
			args.emplace_back(a_position->GetFormID());
			args.emplace_back(a_enjoyment);
			view->InvokeNoReturn("_root.main.updateSliderPct", args.data(), static_cast<uint32_t>(args.size()));
		});
	}

	void SetEnjoyment(RE::StaticFunctionTag*, RE::TESQuest* a_qst, RE::Actor* a_position, float a_enjoyment)
	void SetEnjoyment(RE::StaticFunctionTag*, RE::TESQuest* a_qst, RE::Actor* a_position, float a_enjoyment)
	{
		VALIDATE;
		VALIDATE;
		SKSE::GetTaskInterface()->AddUITask([=]() {
			const auto view = RE::UI::GetSingleton()->GetMovieView(Registry::Interface::SceneMenu::NAME);
			std::vector<RE::GFxValue> args{};
			args.emplace_back(a_position->GetFormID());
			args.emplace_back(a_enjoyment);
			view->InvokeNoReturn("_root.main.setSliderPct", args.data(), static_cast<uint32_t>(args.size()));
		});
	}

}	 // namespace Papyrus::SceneMenu
