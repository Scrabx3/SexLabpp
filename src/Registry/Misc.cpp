#include "Misc.h"

namespace Registry
{
	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor)
	{
		const auto base = a_actor->GetTemplateActorBase();
		return base ? base : a_actor->GetActorBase();
	}
}
