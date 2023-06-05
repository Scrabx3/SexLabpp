#include "Scale.h"

namespace Registry
{
	float Scale::GetScale(RE::TESObjectREFR* a_reference)
	{
		assert(a_reference);
    auto ret = a_reference->GetScale();
		const auto node = a_reference->GetNodeByName(basenode);
		return node ? ret * node->local.scale : ret;
	}

	void Scale::SetScale(RE::Actor* a_actor, float a_absolutescale)
	{
		assert(a_actor && a_absolutescale > 0.0f);
		const auto base = a_actor->GetActorBase();
		const auto female = base ? base->GetSex() == RE::SEXES::kFemale : false;
		transformInterface->AddNodeTransformScaleMode(a_actor, false, female, basenode, namekey, ScaleModes::Multiplicative);
		transformInterface->RemoveNodeTransformScale(a_actor, false, female, basenode, namekey);
		transformInterface->UpdateNodeTransforms(a_actor, false, female, basenode);

		float basescale = GetScale(a_actor);
		// base * x = scale <=> x = scale / base
		float targetscale = a_absolutescale / basescale;

		transformInterface->AddNodeTransformScale(a_actor, false, female, basenode, namekey, targetscale);
		transformInterface->UpdateNodeTransforms(a_actor, false, female, basenode);
	}

  void Scale::RemoveScale(RE::Actor* a_actor)
  {
    assert(a_actor);
		const auto base = a_actor->GetActorBase();
		const auto female = base ? base->GetSex() == RE::SEXES::kFemale : false;
		transformInterface->RemoveNodeTransformScale(a_actor, false, female, basenode, namekey);
		transformInterface->RemoveNodeTransformScaleMode(a_actor, false, female, basenode, namekey);
		transformInterface->UpdateNodeTransforms(a_actor, false, female, basenode);
  }

}  // namespace Registry
