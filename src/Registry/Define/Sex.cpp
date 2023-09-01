#include "Sex.h"

namespace Registry
{
  Sex GetSex(RE::Actor* a_actor, bool a_skipfactions)
	{
		Sex ret = Sex::None;
		if (!a_skipfactions) {
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
		}

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
			if (!IsNPC(a_actor))
				return Settings::bCreatureGender ? Sex::Female : Sex::Male;
			else if (IsFuta(a_actor))
				return Sex::Futa;
			return Sex::Female;
		}
	}

  Sex GetSex(RE::Actor* a_actor)
  {
    return GetSex(a_actor, false);
  }

} // namespace Registry
