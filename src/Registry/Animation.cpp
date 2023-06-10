#include "Animation.h"

namespace Registry
{
	Sex GetSex(RE::Actor* a_actor)
	{
		Sex ret = Sex::None;
		a_actor->VisitFactions([&](auto a_faction, auto a_rank) {
			if (a_faction == GameForms::GenderFaction) {
				switch (a_rank) {
				case 0:
					ret = Sex::Male;
					break;
				case 1:
					ret = Sex::Female;
					break;
				case 2:
					ret = Sex::Futa;
					break;
				default:
					logger::info("Actor {} has invalid gender faction rank ({})", a_actor->GetFormID(), a_rank);
					break;
				}
				return true;
			}
			return false;
		});
		if (ret != Sex::None)
			return ret;

		const auto base = a_actor->GetActorBase();
		if (!base) {
			logger::error("Unable to retrieve actor base for actor {:X}", a_actor->formID);
			return Sex::None;
		}
		switch (base->GetSex()) {
		default:
		case RE::SEXES::kMale:
			return Sex::Male;
		case RE::SEXES::kFemale:
			if (IsNPC(a_actor) && IsFuta(a_actor))
				return Sex::Female;
			else
				return Sex::Futa;
			break;
		}
		return Sex::None;
	}

	Stage* Scene::GetStageByKey(std::string_view a_key) const
	{
		for (auto&& [key, dest] : graph) {
			if (key->id == a_key)
				return key;
		}
		return nullptr;
	}

}	 // namespace Registry
