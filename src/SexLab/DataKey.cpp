#include "SexLab/DataKey.h"

namespace SexLab::DataKey
{
	uint32_t BuildKey(RE::Actor* a_ref, bool a_victim, uint32_t a_raceid)
	{
    const auto g = GetGender(a_ref);
    if (g.all(SexLab::Gender::UNDEFINED))
      return 0;

		return static_cast<uint32_t>(g.underlying()) | (a_raceid << 8) | (a_victim << 16) | (IsVampire(a_ref) << 17);
	}

	uint32_t BuildCustomKey(uint32_t a_gender, uint32_t a_raceid, std::vector<bool> a_extradata)
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
			ret = Key::Crt_Male;
			break;
		case 4:
			ret = Key::Crt_Female;
			break;
		}
		const size_t limit = a_extradata.size() > ExtraDataCount ? ExtraDataCount : a_extradata.size();
		for (size_t i = 0; i < limit; i++) {
			if (a_extradata[i])
				ret |= 1 << (16 + i);
		}
		return ret | (a_raceid << 8);
	}

	std::vector<uint32_t> SortKeys(const std::vector<uint32_t>& a_keys)
  {
    std::vector<uint32_t> ret{ a_keys };
    for (size_t i = 1; i < ret.size(); i++) {
      uint32_t key = ret[i];
      size_t j = i - 1;
      while (j >= 0 && IsLess(key, ret[j])) {
        ret[j + 1] = ret[j];
				j--;
      }
      ret[j + 1] = key;
    }
    return ret;
  }

	// a_key <= a_cmp
	bool IsLess(uint32_t a_key, uint32_t a_cmp)
	{
    // blank & empty
		if (!a_cmp || (a_cmp & Key::Blank))
			return true;
		else if (!a_key || (a_key & Key::Blank))
			return false;

    // race
		const auto crt_key = a_key & Key::CrtTotal;
		const auto crt_cmp = a_cmp & Key::CrtTotal;
		if (crt_key != crt_cmp)
      return crt_key < crt_cmp;

    // gender
		const auto minimizegender = [](uint32_t a_key) -> uint32_t {
			if (a_key & Key::Overwrite_Male) {
				return a_key & Key::Human ? Key::Male : Key::Crt_Male;
			} else if (a_key & Key::Overwrite_Female) {
				return a_key & Key::Human ? Key::Female : Key::Crt_Female;
			}
			return a_key & 0xFF;
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

	bool MatchKey(uint32_t a_key, uint32_t a_match)
	{
		if (a_key & Key::Blank)
			return true;

		if ((a_key & Key::CrtTotal) != (a_match & Key::CrtTotal))
			return false;

		if (a_key & Key::Overwrite_Male) {
			a_key |= a_key & Key::Human ? Key::Male : Key::Crt_Male;
		} else if (a_key & Key::Overwrite_Female) {
			a_key |= a_key & Key::Human ? Key::Female : Key::Crt_Female;
		}
		const auto match = a_match & (~Key::CrtTotal);
		return ((a_key & (~Key::CrtTotal)) & match) == match;
	}

	bool MatchArray(const std::vector<uint32_t>& a_key, const std::vector<uint32_t>& a_match)
	{
		for (size_t i = 0; i < a_key.size(); i++) {
			if (!MatchKey(a_key[i], a_match[i]))
				return false;
		}
		return true;
	}
}
