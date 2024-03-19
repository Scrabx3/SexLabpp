#include "sslActorStats.h"

#include "Registry/Define/RaceKey.h"
#include "Registry/Define/Sex.h"
#include "Registry/Stats.h"

namespace Papyrus::ActorStats
{
	std::vector<RE::Actor*> GetAllTrackedActors(RE::StaticFunctionTag*)
	{
		return Registry::Statistics::StatisticsData::GetSingleton()->GetTrackedActors();
	}
	
	std::vector<RE::Actor*> GetAllTrackedUniqueActorsSorted(RE::StaticFunctionTag*)
	{
		auto tracked =  Registry::Statistics::StatisticsData::GetSingleton()->GetTrackedActors();
		std::erase_if(tracked, [](RE::Actor* act) {
			const auto base = act->GetActorBase();
			return !base || !base->IsUnique();
		});
		std::ranges::sort(tracked, [](RE::Actor* a, RE::Actor* b) {
			if (a->IsPlayerRef())
				return true;
			if (b->IsPlayerRef())
				return false;

			return std::strcmp(a->GetDisplayFullName(), b->GetDisplayFullName()) < 0;
		});
		return tracked;
	}

	void SetStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id, float a_value)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		using StatID = Registry::Statistics::ActorStats::StatisticID;
		if (id < 0 || id >= StatID::Total) {
			a_vm->TraceStack("Invalid Stat ID", a_stackID);
			return;
		}
		Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).SetStatistic(StatID(id), a_value);
	}

	float GetStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		using StatID = Registry::Statistics::ActorStats::StatisticID;
		if (id < 0 || id >= StatID::Total) {
			a_vm->TraceStack("Invalid Stat ID", a_stackID);
			return 0;
		}
		return Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).GetStatistic(StatID(id));
	}

	int GetSexuality(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto& stats = Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor);
		const auto value = stats.GetStatistic(stats.Sexuality);
		return MapSexuality(nullptr, value);
	}

	void SetSexuality(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int mapping)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		auto& stats = Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor);
		switch (mapping) {
		case 0:
			{
				float value = static_cast<float>(100 - (Settings::iPercentageHetero / 2));
				stats.SetStatistic(stats.Sexuality, value);
			}
			break;
		case 1:
			{
				float value = static_cast<float>(Settings::iPercentageHomo / 2);
				stats.SetStatistic(stats.Sexuality, value);
			}
			break;
		case 2:
			{
				float range = 100.0f - Settings::iPercentageHetero - Settings::iPercentageHomo;
				float value = Settings::iPercentageHomo + range / 2;
				stats.SetStatistic(stats.Sexuality, value);
			}
			break;
		}
	}

	int MapSexuality(RE::StaticFunctionTag*, float a_sexuality)
	{
		enum
		{
			Hetero = 0,
			Homo = 1,
			Bi = 2
		};
		if (a_sexuality < Settings::iPercentageHomo)
			return Homo;
		if (a_sexuality < 1 - Settings::iPercentageHetero)
			return Bi;
		return Hetero;
	}

	std::vector<RE::BSFixedString> GetAllCustomStatIDs(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		return Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).GetEveryCustomID();
	}

	bool HasCustomStat(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return false;
		}
		return Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).HasCustom(a_stat);
	}

	void SetCustomStatFlt(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat, float a_value)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).SetCustomFlt(a_stat, a_value);
	}

	void SetCustomStatStr(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat, RE::BSFixedString a_value)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).SetCustomStr(a_stat, a_value);
	}

	float GetCustomStatFlt(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat, float a_default)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return a_default;
		}
		const auto ret = Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).GetCustomFlt(a_stat);
		return ret ? *ret : a_default;
	}

	RE::BSFixedString GetCustomStatStr(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat, RE::BSFixedString a_default)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return a_default;
		}
		const auto ret = Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).GetCustomStr(a_stat);
		return ret ? *ret : a_default;
	}

	void DeleteCustomStat(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_stat)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Statistics::StatisticsData::GetSingleton()->GetStatistics(a_actor).RemoveCustomStat(a_stat);
	}

	std::vector<RE::Actor*> GetAllEncounters(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::Actor*> ret{};
		Registry::Statistics::StatisticsData::GetSingleton()->ForEachEncounter([&](Registry::Statistics::ActorEncounter& enc) {
			const auto partner = enc.GetPartner(a_actor);
			if (!partner)
				return false;
			const auto actor = RE::TESForm::LookupByID<RE::Actor>(partner->id);
			if (!actor)
				return false;
			ret.push_back(actor);
			return false;
		});
		return ret;
	}

	std::vector<RE::Actor*> GetAllEncounteredVictims(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::Actor*> ret{};
		Registry::Statistics::StatisticsData::GetSingleton()->ForEachEncounter([&](Registry::Statistics::ActorEncounter& enc) {
			const auto partner = enc.GetPartner(a_actor);
			if (!partner || enc.GetTimesVictim(partner->id) <= 0)
				return false;
			if (const auto actor = RE::TESForm::LookupByID<RE::Actor>(partner->id))
				ret.push_back(actor);
			return false;
		});
		return ret;
	}

	std::vector<RE::Actor*> GetAllEncounteredAssailants(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::Actor*> ret{};
		Registry::Statistics::StatisticsData::GetSingleton()->ForEachEncounter([&](Registry::Statistics::ActorEncounter& enc) {
			const auto partner = enc.GetPartner(a_actor);
			if (!partner || enc.GetTimesAssailant(partner->id) <= 0)
				return false;
			if (const auto actor = RE::TESForm::LookupByID<RE::Actor>(partner->id))
				ret.push_back(actor);
			return false;
		});
		return ret;
	}

	RE::Actor* GetMostRecentEncounter(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int a_encountertype)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return nullptr;
		}
		return Registry::Statistics::StatisticsData::GetSingleton()->GetMostRecentEncounter(a_actor, Registry::Statistics::ActorEncounter::EncounterType(a_encountertype));
	}

	void AddEncounter(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner, int a_encountertype)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		const auto type = Registry::Statistics::ActorEncounter::EncounterType(a_encountertype);
		Registry::Statistics::StatisticsData::GetSingleton()->AddEncounter(a_actor, a_partner, type);
	}

	float GetLastEncounterTime(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_partner);
		if (!enc)
			return 0.0;
		return enc->GetLastTimeMet();
	}

	int GetTimesMet(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_partner);
		return enc ? enc->GetTimesMet() : 0;
	}

	int GetTimesVictimzed(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_assailant)
	{
		if (!a_actor || !a_assailant) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_assailant);
		return enc ? enc->GetTimesVictim(a_actor->formID) : 0;
	}

	int GetTimesAssaulted(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_victim)
	{
		if (!a_actor || !a_victim) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_victim);
		return enc ? enc->GetTimesAssailant(a_actor->formID) : 0;
	}

	int GetTimesSubmissive(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_partner);
		return enc ? enc->GetTimesSubmissive(a_actor->formID) : 0;
	}


	int GetTimesDominant(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, RE::Actor* a_partner)
	{
		if (!a_actor || !a_partner) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0;
		}
		const auto enc = Registry::Statistics::StatisticsData::GetSingleton()->GetEncounter(a_actor, a_partner);
		return enc ? enc->GetTimesDominant(a_actor->formID) : 0;
	}

	void ResetStatistics(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		Registry::Statistics::StatisticsData::GetSingleton()->DeleteStatistics(a_actor->GetFormID());
	}

	std::vector<RE::BSFixedString> GetEveryStatisticID(RE::StaticFunctionTag*)
	{
		const auto stats = Registry::Statistics::StatisticsData::GetSingleton();
		std::vector<RE::BSFixedString> ret{};
		stats->ForEachStatistic([&](Registry::Statistics::ActorStats& stats) {
      const auto keys = stats.GetEveryCustomID();
			for (auto&& key : keys) {
				if (key == Purity || key == Lewdness || key == Foreplay)
					continue;
				if (std::ranges::contains(ret, key))
					continue;
				ret.push_back(key);
			}
			return false;
		});
		return ret;
	}

	float GetLegacyStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return 0.0;
		}
		const auto statdata = Registry::Statistics::StatisticsData::GetSingleton();
    const auto& stats = statdata->GetStatistics(a_actor);
		switch (LegacyStatistics(id)) {
		case LegacyStatistics::L_Foreplay:
			{
				auto ret = stats.GetCustomFlt(Foreplay);
				return ret ? *ret : 0;
			}
		case LegacyStatistics::XP_Vaginal:
			return stats.GetStatistic(stats.XP_Vaginal);
		case LegacyStatistics::XP_Anal:
			return stats.GetStatistic(stats.XP_Anal);
		case LegacyStatistics::XP_Oral:
			return stats.GetStatistic(stats.XP_Oral);
		case LegacyStatistics::L_Pure:
      {
				auto ret = stats.GetCustomFlt(Purity);
        return ret ? *ret : 0;
			}
		case LegacyStatistics::L_Lewd:
			{
				auto ret = stats.GetCustomFlt(Lewdness);
				return ret ? *ret : 0;
			}
		case LegacyStatistics::Times_Males:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return (static_cast<int32_t>(it.sex) & static_cast<int32_t>(Registry::Sex::Male)) && it.race == Registry::RaceKey::Human;
			}));
		case LegacyStatistics::Times_Females:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return (static_cast<int32_t>(it.sex) & static_cast<int32_t>(Registry::Sex::Female)) && it.race == Registry::RaceKey::Human;
			}));
		case LegacyStatistics::Times_Creatures:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return it.race != Registry::RaceKey::Human;
			}));
		case LegacyStatistics::Times_Masturbation:
			return stats.GetStatistic(stats.TimesMasturbated);
		case LegacyStatistics::Times_Aggressor:
			return stats.GetStatistic(stats.TimesDominant);
		case LegacyStatistics::Times_Victim:
			return stats.GetStatistic(stats.TimesSubmissive);
		case LegacyStatistics::SexCount:
			return stats.GetStatistic(stats.TimesTotal);
		case LegacyStatistics::PlayerSex:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return it.id == 0x14;
			}));
		case LegacyStatistics::Sexuality:
			{
				const auto sex = stats.GetStatistic(stats.Sexuality);
				const auto retF = [&sex](int32_t start, int32_t range, float range_legacy) {
					const auto perc = (sex - static_cast<float>(start)) / static_cast<float>(range);
					return start + perc * range_legacy;
				};
				constexpr auto rHomo = 35.0f, rBi = 30.0f, rHetero = 35.0f;
				static_assert(rHomo + rBi + rHetero == 100.0f);
				if (sex < Settings::iPercentageHomo) {
					return retF(0, Settings::iPercentageHomo, rHomo);
				}
				const auto threshBi = 1 - Settings::iPercentageHetero;
				if (sex < threshBi) {
					const auto rangeBi = 1 - Settings::iPercentageHetero - Settings::iPercentageHomo;
					return retF(Settings::iPercentageHomo, rangeBi, rBi);
				}
				return retF(threshBi, Settings::iPercentageHetero, rHetero);
			}
		case LegacyStatistics::TimeSpent:
			return stats.GetStatistic(stats.SecondsInScene);
		case LegacyStatistics::LastSex_RealTime:
			{
				constexpr auto seconds_in_day = 86400.0f;
				const auto timescale = std::max(RE::Calendar::GetSingleton()->GetTimescale(), 1.0f);
				const auto gametime = stats.GetStatistic(stats.LastUpdate_GameTime);
				return (gametime / timescale) * seconds_in_day;
			}
		case LegacyStatistics::LastSex_GameTime:
			return stats.GetStatistic(stats.LastUpdate_GameTime);
		case LegacyStatistics::Times_VaginalCount:
			return stats.GetStatistic(stats.TimesVaginal);
		case LegacyStatistics::Times_AnalCount:
			return stats.GetStatistic(stats.TimesAnal);
		case LegacyStatistics::Times_OralCount:
			return stats.GetStatistic(stats.TimesOral);
		default:
			a_vm->TraceStack(fmt::format("Invalid id {}", id).c_str(), a_stackID);
      return 0.0;
		}
	}

	std::vector<float> GetAllLegycSkills(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		constexpr auto count = static_cast<int>(LegacyStatistics::Total);
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return std::vector<float>(count);
		}
		std::vector<float> ret{};
		ret.reserve(count);
		for (int i = 0; i < count; i++) {
			ret.push_back(GetLegacyStatistic(a_vm, a_stackID, nullptr, a_actor, i));
		}
		return ret;
	}

	void SetLegacyStatistic(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int id, float a_value)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return;
		}
		const auto statdata = Registry::Statistics::StatisticsData::GetSingleton();
		auto& stats = statdata->GetStatistics(a_actor);
		switch (LegacyStatistics(id)) {
		case LegacyStatistics::L_Foreplay:
			stats.SetCustomFlt(Foreplay, a_value);
			break;
		case LegacyStatistics::XP_Vaginal:
			stats.SetStatistic(stats.XP_Vaginal, a_value);
			break;
		case LegacyStatistics::XP_Anal:
			stats.SetStatistic(stats.XP_Anal, a_value);
			break;
		case LegacyStatistics::XP_Oral:
			stats.SetStatistic(stats.XP_Oral, a_value);
			break;
		case LegacyStatistics::L_Pure:
			stats.SetCustomFlt(Purity, a_value);
			break;
		case LegacyStatistics::L_Lewd:
			stats.SetCustomFlt(Lewdness, a_value);
			break;
		case LegacyStatistics::Times_Males:
		case LegacyStatistics::Times_Females:
		case LegacyStatistics::Times_Creatures:
			// Encounter Statistics
			break;
		case LegacyStatistics::Times_Masturbation:
			stats.SetStatistic(stats.TimesMasturbated, a_value);
			break;
		case LegacyStatistics::Times_Aggressor:
			stats.SetStatistic(stats.TimesDominant, a_value);
			break;
		case LegacyStatistics::Times_Victim:
			stats.SetStatistic(stats.TimesSubmissive, a_value);
			break;
		case LegacyStatistics::SexCount:
			stats.SetStatistic(stats.TimesTotal, a_value);
			break;
		case LegacyStatistics::PlayerSex:
			// Encounter Statistic
			break;
		case LegacyStatistics::Sexuality:
			{
				constexpr auto rHomo = 35.0f, rBi = 30.0f, rHetero = 35.0f;
				auto setF = [&](int32_t start, int32_t range, float range_legacy) mutable {
					float perc = (a_value - static_cast<float>(start)) / range_legacy;
					const auto value = start + perc * static_cast<float>(range);
					stats.SetStatistic(stats.Sexuality, value);
				};
				if (a_value < rHomo) {
					setF(0, Settings::iPercentageHomo, rHomo);
				}
				const auto threshBi = 1 - Settings::iPercentageHetero;
				if (a_value < rHomo + rBi) {
					setF(Settings::iPercentageHomo, threshBi, rHomo + rBi);
				}
				setF(threshBi, Settings::iPercentageHetero, rHetero);
			}
			break;
		case LegacyStatistics::TimeSpent:
			stats.SetStatistic(stats.SecondsInScene, a_value);
			break;
		case LegacyStatistics::LastSex_RealTime:
			{
				constexpr auto seconds_in_day = 86400.0f;
				const auto timescale = std::max(RE::Calendar::GetSingleton()->GetTimescale(), 1.0f);
				const auto value = (a_value / seconds_in_day) * timescale;
				stats.SetStatistic(stats.LastUpdate_GameTime, value);
			}
			break;
		case LegacyStatistics::LastSex_GameTime:
			stats.SetStatistic(stats.LastUpdate_GameTime, a_value);
			break;
		case LegacyStatistics::Times_VaginalCount:
			stats.SetStatistic(stats.TimesVaginal, a_value);
			break;
		case LegacyStatistics::Times_AnalCount:
			stats.SetStatistic(stats.TimesAnal, a_value);
			break;
		case LegacyStatistics::Times_OralCount:
			stats.SetStatistic(stats.TimesOral, a_value);
			break;
		default:
			a_vm->TraceStack(fmt::format("Invalid id {}", id).c_str(), a_stackID);
			return;
		}
	}


} // namespace Papyrus::ActorStats
