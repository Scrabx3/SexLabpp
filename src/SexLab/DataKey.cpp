#include "SexLab/DataKey.h"

#include "SexLab/RaceKey.h"

namespace SexLab::DataKey
{
	void AddGender(const RE::Actor* a_actor, stl::enumeration<Gender, std::uint32_t>& a_key)
	{
		const auto base = a_actor->GetActorBase();
		if (!base) {
			logger::error("Unable to retrieve actor base from actor {}", a_actor->GetFormID());
			return;
		}
		if (!IsNPC(a_actor)) {
			a_key.set(Gender::Creature);
		}
		switch (base->GetSex()) {
		case RE::SEXES::kMale:
			a_key.set(Key::Male);
			break;
		case RE::SEXES::kFemale:
			if (a_key.none(Key::Creature) && IsFuta(a_actor))
				a_key.set(Key::Futa);
			else
				a_key.set(Key::Female);
			break;
		default:
			logger::error("Unable to retrieve base gender from actor {}", a_actor->GetFormID());
			a_key.set(Key::Male);
			break;
		}
		for (auto&& f : base->factions) {
			if (f.faction == GameForms::GenderFaction) {
				switch (f.rank) {
				case 0:
					a_key.set(Gender::Overwrite_Male);
					break;
				case 1:
					a_key.set(Gender::Overwrite_Female);
					break;
				default:
					logger::info("Actor {} has invalid gender faction rank ({})", a_actor->GetFormID(), f.rank);
					break;
				}
				break;
			}
		}
	}

	uint32_t BuildKey(RE::Actor* a_ref, bool a_victim, std::string& a_racekey)
	{
		stl::enumeration<Key, std::uint32_t> key{};
		AddGender(a_ref, key);
		// AddRaceID(a_ref); TODO: implement
		key.set(Key(RaceKey::GetRaceID(a_racekey) << CrtBits));

		if (a_victim)
			key.set(Key::Victim);

		if (IsVampire(a_ref))
			key.set(Key::Vampire);

		if (a_ref->IsDead() || a_ref->AsActorState()->GetLifeState() == RE::ACTOR_LIFE_STATE::kDying)
			key.set(Key::Dead);

		// TODO: Check amputation n all that I guess
		return static_cast<uint32_t>(key.underlying());
	}

	uint32_t BuildCustomKey(uint32_t a_gender, std::string a_racekey, std::vector<bool> a_extradata)
	{
		uint32_t ret;
		switch (a_gender) {
		default:
		case 0:
			ret = Key::Male;
			break;
		case 1:
			ret = Key::Female;
			break;
		case 2:
			ret = Key::Futa;
			break;
		case 3:
			ret = Key::Male | Key::Creature;
			break;
		case 4:
			ret = Key::Female | Key::Creature;
			break;
		}
		const size_t limit = min(ExtraBits, a_extradata.size());
		for (size_t i = 0; i < limit; i++) {
			if (a_extradata[i])
				ret |= 1 << ((CrtBits + GenderBits) + i);
		}
		const int raceid = RaceKey::GetRaceID(a_racekey);
		return ret | (raceid << GenderBits);
	}

	// a_key < a_cmp
	bool IsLess(uint32_t a_key, uint32_t a_cmp)
	{
    // blank & empty
		if (!a_key || (a_key & Key::Blank))
			return false;
		if (!a_cmp || (a_key & Key::Blank))
			return true;

    // race
		const auto crt_key = a_key & Key::CrtTotal;
		const auto crt_cmp = a_cmp & Key::CrtTotal;
		if (crt_key != crt_cmp)
      return crt_key < crt_cmp;

    // gender
		const auto minimizegender = [](uint32_t a_key) -> uint32_t {
			if (a_key & Key::Overwrite_Male) {
				return (a_key & Key::Creature) | Key::Male;
			} else if (a_key & Key::Overwrite_Female) {
				return (a_key & Key::Creature) | Key::Female;
			}
			return a_key & GendersTotal;
		};
		const auto genderkey = minimizegender(a_key);
		const auto gendercmp = minimizegender(a_cmp);
		if (genderkey != gendercmp)
			return genderkey < gendercmp;

		// no victim > victim
		const auto victimkey = a_key & Key::Victim;
		const auto victimcmp = a_cmp & Key::Victim;
		if (victimcmp != victimkey)
			return victimkey > victimcmp;
		// no vampire < vampire
		const auto vampirekey = a_key & Key::Vampire;
		const auto vampirecmp = a_cmp & Key::Vampire;
		if (vampirecmp != vampirekey)
			return vampirekey < vampirecmp;

		return false;
	}

	// Does a_key fill a_match?
	bool MatchKey(uint32_t a_key, uint32_t a_match)
	{
		if (!a_key || (a_key & Key::Blank))
			return true;

		if ((a_key & Key::MandatoryKeys) != (a_match & Key::MandatoryKeys))
			return false;

		if (a_key & Key::Overwrite_Male) {
			a_key = (a_key & ~Key::GendersBaseTotal) | Key::Male;
		} else if (a_key & Key::Overwrite_Female) {
			a_key = (a_key & ~Key::GendersBaseTotal) | Key::Female;
		}
		const auto match = a_match & ~Key::MandatoryKeys;
		return ((a_key & ~Key::MandatoryKeys) & match) == match;
	}

}
