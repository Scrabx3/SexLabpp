#include "Papyrus/sslDataKey.h"

namespace SLPP
{
	uint32_t DataKey::BuildDataKeyNative(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_ref, bool a_isvictim, uint32_t a_raceid)
	{
		if (!a_ref) {
			a_vm->TraceStack("Cannot build data key from a none reference", a_stackID);
			return 0;
		}
		return SexLab::DataKey::BuildKey(a_ref, a_isvictim, a_raceid);
	}

	std::vector<uint32_t> DataKey::SortDataKeys(RE::StaticFunctionTag*, std::vector<uint32_t> a_keys)
	{
		return SexLab::DataKey::SortKeys(a_keys);
	}

	bool DataKey::IsLess(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp)
	{
		return SexLab::DataKey::IsLess(a_key, a_cmp);
	}

	bool DataKey::Match(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp)
	{
		return SexLab::DataKey::MatchKey(a_key, a_cmp);
	}

	bool DataKey::MatchArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<uint32_t> a_key, std::vector<uint32_t> a_cmp)
	{
		if (a_key.size() != a_cmp.size()) {
			a_vm->TraceStack("Cannot match two arrays of unequal size", a_stackID);
			return false;
		}
		return SexLab::DataKey::MatchArray(a_key, a_cmp);
	}
	
	uint32_t DataKey::GetLegacyGenderByKey(RE::StaticFunctionTag*, uint32_t a_key)
	{
		if (a_key & Key::Male)
			return 0;
		else if (a_key & Key::Female)
			return 1;
		else if (a_key & Key::Crt_Male)
			return 2;
		else if (a_key & Key::Crt_Female)
			return 3;
		return 0;
	}

	uint32_t DataKey::BuildByLegacyGenderNative(RE::StaticFunctionTag*, uint32_t a_legacygender, int a_raceid)
	{
		uint32_t g;
		switch (a_legacygender) {
		case 0:
			g = Key::Male;
			break;
		case 1:
			g = Key::Female;
			break;
		case 2:
			g = Key::Crt_Male;
			break;
		case 3:
			g = Key::Crt_Female;
			break;
		case -1:
			return 0b111;
		case -2:
			g = 0b00011;
			break;
		default:
			g = Key::Male;
			break;
		}
		return g | (a_raceid << 8);
	}

	uint32_t DataKey::AddGenderToKey(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_gender)
	{
		switch (a_gender) {
		case 0:
			return a_key | Key::Male;
		case 1:
			return a_key | Key::Female;
		case 2:
			return a_key | Key::Female;
		case 3:
			return a_key | Key::Crt_Male;
		case 4:
			return a_key | Key::Crt_Female;
		case 5:
			return a_key | Key::Overwrite_Male;
		case 6:
			return a_key | Key::Overwrite_Female;
		default:
			return a_key;
		}
	}

	uint32_t DataKey::RemoveGenderFromKey(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_gender)
	{
		switch (a_gender) {
		case 0:
			return a_key & (~Key::Male);
		case 1:
			return a_key & (~Key::Female);
		case 2:
			return a_key & (~Key::Female);
		case 3:
			return a_key & (~Key::Crt_Male);
		case 4:
			return a_key & (~Key::Crt_Female);
		case 5:
			return a_key & (~Key::Overwrite_Male);
		case 6:
			return a_key & (~Key::Overwrite_Female);
		default:
			return a_key;
		}
	}

	void DataKey::NeutralizeCreatureGender(RE::StaticFunctionTag*, std::vector<uint32_t> a_keys)
	{
		for (auto&& k : a_keys) {
			k |= Key::Crt_Male | Key::Crt_Female;
		}
	}
}