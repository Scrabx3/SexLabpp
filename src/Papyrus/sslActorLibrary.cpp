#include "Papyrus/sslActorLibrary.h"

namespace Papyrus::ActorLibrary
{
  inline bool CanAnimateImpl(RE::Actor* a_actor)
	{
		const auto race = a_actor->GetRace();
		if (!race || race->IsChildRace()) {
			return false;
		}
		std::string i{ race->GetFormEditorID() }, n{ race->GetName() };
		SexLab::ToLower(i);
		SexLab::ToLower(n);
		constexpr std::array base{ "moli", "child", "little ", "117 ", "enfant", "teen" };
		for (auto&& str : base)
			if (i.find(str) != std::string::npos || n.find(str) != std::string::npos)
				return false;

		if (a_actor->GetScale() < 0.92) {
			std::array scale{ "elin", "monli" };
			for (auto&& str : base)
				if ((i.find(str) != std::string::npos || n.find(str) != std::string::npos))
					return false;
		}
		return true;
	}

	bool CanAnimate(VM* a_vm, StackID a_stackID, RE::TESQuest*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Cannot validate a none reference", a_stackID);
			return false;
		}
		return CanAnimateImpl(a_actor);
	}

	bool CanAnimateActor(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
  {
		if (!a_actor) {
			a_vm->TraceStack("Cannot validate a none reference", a_stackID);
			return false;
		}
		if (!a_actor->Is3DLoaded())
			return false;
		if (a_actor->IsOnMount())
			return false;
		if (a_actor->AsActorState()->IsFlying())
			return false;
		if (!CanAnimateImpl(a_actor))
      return false;

		return true;
  }

}	 // namespace Papyrus::ActorLibrary
