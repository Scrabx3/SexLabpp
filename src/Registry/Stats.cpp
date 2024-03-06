#include "Stats.h"

#include "Registry/Define/Sex.h"

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

	void ActorStats::Save(SKSE::SerializationInterface* a_intfc)
	{
		for (auto&& stat : _stats) {
			if (!a_intfc->WriteRecordData(stat)) {
				logger::error("Failed to save statistic");
				continue;
			}
		}
		if (!a_intfc->WriteRecordData(_custom.size())) {
			logger::error("Failed to save number of custom statistics ({})", _custom.size());
			return;
		}
		for (auto&& [id, stat] : _custom) {
			if (!stl::write_string(a_intfc, id)) {
				logger::error("Failed to save custom statistic {}", id);
				continue;
			}
			if (std::holds_alternative<float>(stat)) {
				a_intfc->WriteRecordData(0);
				if (!a_intfc->WriteRecordData(stat)) {
					logger::error("Failed to save custom statistic {}", id);
					continue;
				}
			} else {
				a_intfc->WriteRecordData(1);
				auto save = std::get<RE::BSFixedString>(stat);
				if (!stl::write_string(a_intfc, save)) {
					logger::error("Failed to save custom statistic {}", id);
					continue;
				}
			}
		}
	}

	void ActorStats::Load(SKSE::SerializationInterface* a_intfc)
	{
		for (size_t i = 0; i < StatisticID::Total; i++) {
			a_intfc->ReadRecordData(_stats[i]);
		}
		size_t numRegs;
		a_intfc->ReadRecordData(numRegs);

		_custom.clear();
		std::string key;
		size_t alternative;
		for (size_t i = 0; i < numRegs; i++) {
			stl::read_string(a_intfc, key);
			a_intfc->ReadRecordData(alternative);
			if (alternative == 0) {
				float obj;
				a_intfc->ReadRecordData(obj);
				_custom[key] = obj;
			} else {
				std::string obj;
				stl::read_string(a_intfc, obj);
				_custom[key] = obj;
			}
		}
	}

	ActorStats& StatisticsData::GetStatistics(RE::Actor* a_key)
	{
		auto where = _data.find(a_key->GetFormID());
		if (where == _data.end()) {
			return _data[a_key->GetFormID()] = ActorStats(a_key);
		}
		return where->second;
	}

	void StatisticsData::DeleteStatistics(RE::FormID a_key)
	{
		_data.erase(a_key);
	}

	void StatisticsData::Save(SKSE::SerializationInterface* a_intfc)
	{
		if (!a_intfc->WriteRecordData(_data.size())) {
			logger::error("Failed to save number of saved statistics ({})", _data.size());
			return;
		}
		for (auto&& [id, data] : _data) {
			if (!a_intfc->WriteRecordData(id)) {
				logger::error("Failed to save reg ({:X})", id);
				continue;
			}
			data.Save(a_intfc);
		}
		logger::info("Saved {} statistics", _data.size());
	}

	void StatisticsData::Load(SKSE::SerializationInterface* a_intfc)
	{
		_data.clear();
		std::size_t numRegs;
		a_intfc->ReadRecordData(numRegs);

		RE::FormID formID;
		for (size_t i = 0; i < numRegs; i++) {
			a_intfc->ReadRecordData(formID);
			if (!a_intfc->ResolveFormID(formID, formID)) {
				logger::warn("Error reading formID ({:X})", formID);
				continue;
			}
			ActorStats stats{};
			stats.Load(a_intfc);
			_data[formID] = stats;
		}
		logger::info("Loaded {} statistics", _data.size());
	}

	void StatisticsData::Revert(SKSE::SerializationInterface*)
	{
		_data.clear();
	}


}	 // namespace Registry::Statistics
