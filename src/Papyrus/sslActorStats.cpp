#include "sslActorStats.h"

#include "Registry/Define/RaceKey.h"
#include "Registry/Define/Sex.h"
#include "Registry/Stats.h"

namespace Papyrus::ActorStats
{
	static inline constexpr auto Purity{ "SSLLEGACY_Purity" };
	static inline constexpr auto Lewdness{ "SSLLEGACY_Lewd" };
	static inline constexpr auto Foreplay{ "SSLLEGACY_Foreplay" };

	enum class LegacyStatistics
	{
		L_Foreplay,
		XP_Vaginal,
		XP_Anal,
		XP_Oral,
		L_Pure,
		L_Lewd,
		Times_Males,
		Times_Females,
		Times_Creatures,
		Times_Masturbation,
		Times_Aggressor,
		Times_Victim,
		SexCount,   // how often the actor had intercourse in general
		PlayerSex,  // how often the actor had intercourse with the player
		Sexuality,
		TimeSpent,  // time spent in scenes, there is no reference in what scale this is measured
		LastSex_RealTime,
		LastSex_GameTime,
		Times_VaginalCount,
		Times_AnalCount,
		Times_OralCount,

		Total
	};

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
    const auto stats = statdata->GetStatistics(a_actor);
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
				return it.sex == Registry::Sex::Male && it.race == Registry::RaceKey::Human;
			}));
		case LegacyStatistics::Times_Females:
			return static_cast<float>(statdata->GetNumberEncounters(a_actor, [](auto& it) {
				return it.sex == Registry::Sex::Female && it.race == Registry::RaceKey::Human;
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
			return stats.GetStatistic(stats.Sexuality);
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
		auto stats = statdata->GetStatistics(a_actor);
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
		case LegacyStatistics::Times_Masturbation:
		case LegacyStatistics::Times_Aggressor:
		case LegacyStatistics::Times_Victim:
		case LegacyStatistics::SexCount:
		case LegacyStatistics::PlayerSex:
			break;
		case LegacyStatistics::Sexuality:
			stats.SetStatistic(stats.Sexuality, a_value);
		case LegacyStatistics::TimeSpent:
		case LegacyStatistics::LastSex_RealTime:
		case LegacyStatistics::LastSex_GameTime:
		case LegacyStatistics::Times_VaginalCount:
		case LegacyStatistics::Times_AnalCount:
		case LegacyStatistics::Times_OralCount:
			break;
		default:
			a_vm->TraceStack(fmt::format("Invalid id {}", id).c_str(), a_stackID);
			return;
		}
	}


} // namespace Papyrus::ActorStats
