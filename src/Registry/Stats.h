#pragma once

namespace Registry::Statistics
{
	struct ActorStats
	{
		enum StatisticID
		{
			LastUpdate_GameTime,
			SecondsInScene,

			XP_Anal,
			XP_Vaginal,
			XP_Oral,

			PartnersMale,
			PartnersFemale,
			PartnersFuta,
			PartnersCreature,
			TimesOral,
			TimesVaginal,
			TimesAnal,
			TimesMasturbated,
			TimesSubmissive,
			TimesDominant,
			TimesTotal,

			SadoMasochismus,
			Sexuality,
			Arousal,

			Total
		};
		ActorStats() :
			_stats(StatisticID::Total) {}
		ActorStats(RE::Actor* owner);
		~ActorStats() = default;

		void SetStatistic(StatisticID key, int32_t value);
		int32_t GetStatistic(StatisticID key) const;

		bool HasCustom(const RE::BSFixedString& key) const;
		float GetCustomFlt(const RE::BSFixedString& key, float retfail) const;
		RE::BSFixedString GetCustomStr(const RE::BSFixedString& key, const RE::BSFixedString& retfail) const;
		void SetCustomFlt(const RE::BSFixedString& key, float value);
		void SetCustomStr(const RE::BSFixedString& key, RE::BSFixedString value);

		void Save(SKSE::SerializationInterface* a_intfc);
		void Load(SKSE::SerializationInterface* a_intfc);

	private:
		template <class T>
		T GetCustom(const RE::BSFixedString& key, T retfail) const
		{
			const auto it = _custom.find(key);
			if (it == _custom.end())
				return retfail;

			auto& ret = it->second;
			return std::holds_alternative<T>(ret) ? std::get<T>(ret) : retfail;
		}

		std::vector<int32_t> _stats{};
		std::map<RE::BSFixedString, std::variant<float, RE::BSFixedString>, FixedStringCompare> _custom{};
	};

	class StatisticsData :
		public Singleton<StatisticsData>
	{
	public:
		ActorStats& GetStatistics(RE::Actor* a_key);
		void DeleteStatistics(RE::FormID a_key);

		void Save(SKSE::SerializationInterface* a_intfc);
		void Load(SKSE::SerializationInterface* a_intfc);
		void Revert(SKSE::SerializationInterface* a_intfc);

	private:
		std::map<RE::FormID, ActorStats> _data;
	};

}	 // namespace Registry::Statistics
