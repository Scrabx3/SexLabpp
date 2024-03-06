#include "Stats.h"

#include "Define/Sex.h"

namespace Registry::Statistics
{
	ActorStats::ActorStats(RE::Actor* owner) :
		_stats(StatisticID::Total)
	{
		const auto base = owner->GetActorBase();
		if (const auto relations = base ? base->relationships : nullptr) {
			Registry::Sexuality s = Sexuality::None;
			for (auto&& rs : *relations) {
				if (!rs)
					continue;
				if (!rs->level.all(RE::BGSRelationship::RELATIONSHIP_LEVEL::kLover))
					continue;
				const auto other = rs->npc1 == base ? rs->npc2 : rs->npc1;
				if (!other)
					continue;
				bool samesex = base->GetSex() == other->GetSex();
				switch (s) {
				case Sexuality::Hetero:
					if (samesex) {
						s = Sexuality::Bi;
						break;
					}
					continue;	 // Next iteration
				case Sexuality::Homo:
					if (!samesex) {
						s = Sexuality::Bi;
						break;
					}
					continue;
				default:
					s = samesex ? Sexuality::Homo : Sexuality::Hetero;
					continue;
				}
				break;
			}
			if (s != Sexuality::Bi && s != Sexuality::None) {
				const auto chance = s == Sexuality::Hetero ?
															Settings::iPercentageHetero + (Settings::iPercentageHomo / 2) :
															Settings::iPercentageHomo + (Settings::iPercentageHetero / 2);
				if (chance >= Random::draw<int32_t>(0, 99)) {
					s = Sexuality::Bi;
				}
			}
			switch (s) {
			case Sexuality::Bi:
				_stats[StatisticID::Sexuality] = Random::draw(Settings::iPercentageHomo, Settings::iPercentageHetero);
				break;
			case Sexuality::Hetero:
				_stats[StatisticID::Sexuality] = Random::draw(Settings::iPercentageHetero, 100);
				break;
			case Sexuality::Homo:
				_stats[StatisticID::Sexuality] = Random::draw(1, Settings::iPercentageHomo);
				break;
			default:
				_stats[StatisticID::Sexuality] = Random::draw<int32_t>(1, 100);
				break;
			}
		} else {
			_stats[StatisticID::Sexuality] = Random::draw<int32_t>(1, 100);
		}
	}

	void ActorStats::SetStatistic(StatisticID key, int32_t value)
	{
		_stats[key] = value;
	}

	int32_t ActorStats::GetStatistic(StatisticID key) const
	{
		return _stats[key];
	}

	float ActorStats::GetCustomFlt(const RE::BSFixedString& key, float retfail) const
	{
		return GetCustom(key, retfail);
	}

	RE::BSFixedString ActorStats::GetCustomStr(const RE::BSFixedString& key, const RE::BSFixedString& retfail) const
	{
		return GetCustom(key, retfail);
	}

	void ActorStats::SetCustomFlt(const RE::BSFixedString& key, float value)
	{
		_custom[key] = value;
	}

	void ActorStats::SetCustomStr(const RE::BSFixedString& key, RE::BSFixedString value)
	{
		_custom[key] = value;
	}

	bool ActorStats::HasCustom(const RE::BSFixedString& key) const
	{
		return _custom.contains(key);
	}

}	 // namespace Registry::Statistics
