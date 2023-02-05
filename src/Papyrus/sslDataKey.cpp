#include "Papyrus/sslDataKey.h"

#include "SexLab/RaceKey.h"

namespace Papyrus
{
	int32_t DataKey::BuildDataKeyNative(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_ref, bool a_isvictim, std::string a_racekey)
	{
		if (!a_ref) {
			a_vm->TraceStack("Cannot build data key from a none reference", a_stackID);
			return 0;
		}
		return SexLab::DataKey::BuildKey(a_ref, a_isvictim, a_racekey);
	}

	int32_t DataKey::BuildCustomKey(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, uint32_t a_gender, std::string a_racekey)
	{
		if (a_gender < 0 || a_gender > 4) {
			a_vm->TraceStack("Custom keys require a gender", a_stackID);
			return 0;
		}
		return SexLab::DataKey::BuildCustomKey(a_gender, a_racekey);
	}

	std::vector<int32_t> DataKey::SortDataKeys(RE::StaticFunctionTag*, const std::vector<int32_t> a_keys)
	{
		std::vector<int32_t> ret{ a_keys.begin(), a_keys.end() };
		std::stable_sort(ret.begin(), ret.end(), SexLab::DataKey::IsLess);
		return ret;
	}

	bool DataKey::IsLess(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp)
	{
		return SexLab::DataKey::IsLess(a_key, a_cmp);
	}

	bool DataKey::Match(RE::StaticFunctionTag*, uint32_t a_key, uint32_t a_cmp)
	{
		return SexLab::DataKey::MatchKey(a_key, a_cmp);
	}

	bool DataKey::MatchArray(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<int32_t> a_keys, std::vector<int32_t> a_cmp)
	{
		if (a_keys.size() != a_cmp.size()) {
			a_vm->TraceStack("Cannot match two arrays of unequal size", a_stackID);
			return false;
		}
		for (size_t i = 0; i < a_keys.size(); i++)
			if (!SexLab::DataKey::MatchKey(a_keys[i], a_cmp[i]))
				return false;
		return true;
	}

	int32_t DataKey::GetGender(RE::StaticFunctionTag*, uint32_t a_key)
	{
		if (a_key & Key::Creature)
			return (a_key & Key::Female) ? 4 : 3;
		if (a_key & Key::Futa)
			return 2;
		if (a_key & Key::Female)
			return 1;
		return 0;
	}

	int32_t DataKey::GetLegacyGenderByKey(RE::StaticFunctionTag*, uint32_t a_key)
	{
		const auto ret = GetGender(nullptr, a_key);
		return ret > 1 ? (ret - 1) : ret;
	}

	int32_t DataKey::BuildByLegacyGender(RE::StaticFunctionTag*, int32_t a_legacygender, std::string a_racekey)
	{
		a_legacygender += a_legacygender >= 2 ? 1 : 0;
		return SexLab::DataKey::BuildCustomKey(a_legacygender, a_racekey);
	}

	std::string DataKey::GetRaceKey(RE::StaticFunctionTag*, uint32_t a_key)
	{
		const auto rawrace = a_key >> Key::CrtBits;
		return SexLab::RaceKey::GetRaceKey(rawrace);
	}

	int32_t DataKey::AddOverwrite(RE::StaticFunctionTag*, uint32_t a_key, bool a_female)
	{
		const auto add = a_female ? Key::Overwrite_Female : Key::Overwrite_Male;
		return a_key | add;
	}

	int32_t DataKey::RemoveOverWrite(RE::StaticFunctionTag*, uint32_t a_key, bool a_female)
	{
		const auto rem = a_female ? ~Key::Overwrite_Female : ~Key::Overwrite_Male;
		return a_key & rem;
	}

	void DataKey::NeutralizeCreatureGender(RE::StaticFunctionTag*, std::vector<int32_t> a_keys)
	{
		for (auto&& k : a_keys) {
			if (k & Key::Creature)
				k |= Key::Male | Key::Female;
		}
	}
}