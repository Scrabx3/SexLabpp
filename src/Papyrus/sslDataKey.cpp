#include "Papyrus/sslDataKey.h"

#include "SexLab/DataKey.h"

namespace SLPP
{
	uint32_t DataKey::BuildDataKeyNative(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_ref, bool a_isvictim, uint32_t a_raceid)
	{
		if (!a_ref) {
			a_vm->TraceStack("Cannot build data key from a none reference", a_stackID);
			return 0;
		}
    const auto g = SexLab::GetGender(a_ref);
		if (g.all(SexLab::Gender::UNDEFINED))
			return 0;

		return static_cast<uint32_t>(g.underlying()) | (a_raceid << 8) | (a_isvictim << 16) | (SexLab::IsVampire(a_ref) << 17);
	}

	std::vector<uint32_t> DataKey::SortDataKeys(RE::StaticFunctionTag*, std::vector<uint32_t> a_keys)
	{
		decltype(a_keys) ret{ a_keys };
		for (size_t i = 1; i < ret.size(); i++) {
			uint32_t key = ret[i];
			size_t j = i - 1;
			while (j >= 0 && IsLess(nullptr, key, ret[j])) {
				ret[j + 1] = ret[j--];
			}
			ret[j + 1] = key;
		}
		return ret;
	}

	bool DataKey::IsLess(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp)
	{
		using Gender = SexLab::Gender;
		// Sort blank keys to the very end
		if (a_cmp == 0)
			return false;
		else if (a_key == 0)
			return true;

		// race id
		if ((a_key & 0xFF00) > (a_cmp & 0xFF00))
			return false;

		// gender
		if (a_cmp & Gender::UNDEFINED)
			return true;
		else if (a_key & Gender::UNDEFINED)
			return false;

		const auto gender = [](uint32_t a_key) -> uint32_t {
			if (a_key & Gender::Overwrite_Male) {
				return (a_key & 0b111) ? Gender::Male : Gender::Crt_Male;
			} else if (a_key & Gender::Overwrite_Female) {
				return (a_key & 0b111) ? Gender::Female : Gender::Crt_Female;
			}
			return a_key & 0xFF;
		};
		const auto genderkey = gender(a_key);
		const auto gendercmp = gender(a_cmp);
		if (genderkey > gendercmp)
			return false;
		// victim < novictim
		if ((a_cmp & (1 << 16)) && (~(a_key & (1 << 16))))
			return false;
		// novampire < vampire
		if ((~(a_cmp & (1 << 17))) && (a_key & (1 << 17)))
			return false;

		return true;
	}

	bool DataKey::Match(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp)
	{
		if (a_key & (1 << 31))	// blank
			return true;

		constexpr auto crt_bits = 0xFF00;
		if ((a_key & crt_bits) != (a_cmp & crt_bits))
			return false;

		const auto match = (a_cmp & (~crt_bits));
		return ((a_key & (~crt_bits)) & match) == match;
	}

	bool DataKey::MatchArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<uint32_t> a_key, std::vector<uint32_t> a_cmp)
	{
		if (a_key.size() != a_cmp.size()) {
			a_vm->TraceStack("Cannot match two arrays of unequal size", a_stackID);
			return false;
		}
		for (size_t i = 0; i < a_key.size(); i++) {
			if (!Match(nullptr, a_key[i], a_cmp[i])) {
				return false;
			}
		}
		return true;
	}

	
	uint32_t DataKey::GetLegacyGenderByKey(RE::StaticFunctionTag*, uint32_t a_key)
	{
		using Gender = SexLab::Gender;
		if (a_key & Gender::Male)
			return 0;
		else if (a_key & Gender::Female)
			return 1;
		else if (a_key & Gender::Crt_Male)
			return 2;
		else if (a_key & Gender::Crt_Female)
			return 3;
		return 0;
	}

	uint32_t DataKey::BuildByLegacyGenderNative(RE::StaticFunctionTag*, uint32_t a_legacygender, int a_raceid)
	{
		using Gender = SexLab::Gender;
		uint32_t g;
		switch (a_legacygender) {
		case 0:
			g = Gender::Male;
			break;
		case 1:
			g = Gender::Female;
			break;
		case 2:
			g = Gender::Crt_Male;
			break;
		case 3:
			g = Gender::Crt_Female;
			break;
		case -1:
			return 0b111;
		case -2:
			g = 0b00011;
			break;
		default:
			g = Gender::Male;
			break;
		}
		return g | (a_raceid << 8);
	}

	uint32_t DataKey::AddGenderToKey(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_gender)
	{
		using Gender = SexLab::Gender;
		switch (a_gender) {
		case 0:
			return a_key | Gender::Male;
		case 1:
			return a_key | Gender::Female;
		case 2:
			return a_key | Gender::Female;
		case 3:
			return a_key | Gender::Crt_Male;
		case 4:
			return a_key | Gender::Crt_Female;
		default:
			return a_key;
		}
	}

	uint32_t DataKey::RemoveGenderFromKey(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_gender)
	{
		using Gender = SexLab::Gender;
		switch (a_gender) {
		case 0:
			return a_key & (~Gender::Male);
		case 1:
			return a_key & (~Gender::Female);
		case 2:
			return a_key & (~Gender::Female);
		case 3:
			return a_key & (~Gender::Crt_Male);
		case 4:
			return a_key & (~Gender::Crt_Female);
		default:
			return a_key;
		}
	}

	void DataKey::NeutralizeCreatureGender(RE::StaticFunctionTag*, std::vector<uint32_t> a_keys)
	{
		using Gender = SexLab::Gender;
		for (auto&& k : a_keys) {
			k |= Gender::Crt_Male | Gender::Crt_Female;
		}
	}
}