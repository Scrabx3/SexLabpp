#pragma once

#include "Animation.h"
#include "Define/RaceKey.h"

namespace Registry
{
	struct VoiceSet
	{
		struct CONDITION
		{
			enum class ConditionType : uint8_t
			{
				Tag,
				Context,
				PositionExtra,
				Submissive
			};

			union Condition
			{
				const char* _string;
				bool _bool;
			};
		public:
			CONDITION(ConditionType a_type, Condition a_condition) :
				type(a_type), condition(a_condition) {}
			~CONDITION() = default;

			ConditionType type;
			Condition condition;
		};

	public:
		VoiceSet(const std::vector<std::pair<RE::TESSound*, uint8_t>>& a_data, const std::vector<CONDITION>& a_conditions = {}) :
			data(a_data), conditions(a_conditions) { assert(!a_data.empty()); }
		VoiceSet(const YAML::Node& a_node);
		VoiceSet() = default;
		~VoiceSet() = default;

		bool IsValid(Stage* a_stage, PositionInfo* a_position, const std::vector<RE::BSFixedString>& a_context) const;
		RE::TESSound* Get(uint32_t a_strength) const;

	private:
		std::vector<std::pair<RE::TESSound*, uint8_t>> data;
		std::vector<CONDITION> conditions;
	};

	class Voice :
		public Singleton<Voice>
	{
		struct VoiceObject
		{
			enum class Defaults
			{
				FemaleClassic,
				FemaleBreathy,
				FemaleYoung,
				FemaleStimulated,
				FemaleQuiet,
				FemaleExcitable,
				FemaleAverage,
				FemaleMature,
				MaleNeutral,
				MaleCalm,
				MaleRough,
				MaleAverage,

				ChaurusVoice01,
				DogVoice01,
				DraugrVoice01,
				FalmerVoice01,
				GiantVoice01,
				HorseVoice01,
				SprigganVoice01,
				TrollVoice01,
				WerewolfVoice01,
				WolfVoice01,

				Total
			};

			VoiceObject(const YAML::Node& a_node);
			VoiceObject(Defaults a_default);
			~VoiceObject() = default;

			RE::BSFixedString name;
			bool enabled;

			RE::SEXES::SEX sex;
			std::vector<RaceKey> races;

			TagData tags;
			VoiceSet defaultset;
			std::vector<VoiceSet> extrasets;

			constexpr bool operator==(const VoiceObject& a_rhs) const noexcept { return name == a_rhs.name; }
		};

	public:
		RE::BSFixedString GetVoice(RE::Actor* a_actor, const TagDetails& tags) const;
		RE::TESSound* PickSound(RE::BSFixedString a_voice, uint32_t a_priority, Stage* a_stage, PositionInfo* a_info, const std::vector<RE::BSFixedString>& a_context) const;

		void Initialize();
		void Save();

	private:
		std::vector<VoiceObject> voices;
	};

}	 // namespace Registry::Voice
