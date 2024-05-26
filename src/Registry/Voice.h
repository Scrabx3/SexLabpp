#pragma once

#include <shared_mutex>

#include "Animation.h"
#include "Define/RaceKey.h"

namespace Registry
{
	enum class LegacyVoice
	{
		Hot,
		Mild,
		Medium
	};

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
		VoiceSet(const YAML::Node& a_node);
		VoiceSet(bool a_aslegacyextra);
		~VoiceSet() = default;

		bool IsValid(const Stage* a_stage, const PositionInfo* a_position, const std::vector<RE::BSFixedString>& a_context) const;
		RE::TESSound* Get(uint32_t a_strength) const;
		RE::TESSound* Get(LegacyVoice a_setting) const;

	public:
		void SetSound(bool front, RE::TESSound* a_sound);
		YAML::Node AsYaml() const;

	private:
		std::vector<std::pair<RE::TESSound*, uint8_t>> data{};
		std::vector<CONDITION> conditions{};
	};

	class Voice :
		public Singleton<Voice>
	{
		struct VoiceObject
		{
			VoiceObject(const YAML::Node& a_node);
			VoiceObject(RE::BSFixedString a_name) :
				name(a_name), fromfile(false), defaultset(false), extrasets({ { true } }){};
			~VoiceObject() = default;

			RE::BSFixedString name;
			bool enabled{ true };
			bool fromfile{ true };

			RE::SEXES::SEX sex{ RE::SEXES::SEX::kNone };
			std::vector<RaceKey> races{};

			TagData tags{};
			VoiceSet defaultset;
			RE::TESSound* orgasmvfx{ nullptr };
			std::vector<VoiceSet> extrasets{};

		public:
			YAML::Node AsYaml() const;
			bool operator==(const VoiceObject& a_rhs) const noexcept { return name == a_rhs.name; }
		};

	public:
		std::vector<RE::BSFixedString> GetAllVoiceNames() const;
		const VoiceObject* GetVoice(RE::Actor* a_actor, const TagDetails& tags) const;
		const VoiceObject* GetVoice(const TagDetails& tags) const;
		const VoiceObject* GetVoice(RE::BSFixedString a_voice) const;
		const VoiceObject* GetVoice(RaceKey a_race) const;
		void SetVoiceObjectEnabled(RE::BSFixedString a_voice, bool a_enabled);

		RE::TESSound* PickSound(RE::BSFixedString a_voice, LegacyVoice a_legacysetting) const;
		RE::TESSound* PickSound(RE::BSFixedString a_voice, uint32_t a_priority, const Stage* a_stage, const PositionInfo* a_info, const std::vector<RE::BSFixedString>& a_context) const;
		RE::TESSound* GetOrgasmSound(RE::BSFixedString a_voice, const Stage* a_stage, const PositionInfo* a_info, const std::vector<RE::BSFixedString>& a_context) const;

		const VoiceObject* GetSavedVoice(RE::FormID a_key) const;
		void SaveVoice(RE::FormID a_key, RE::BSFixedString a_voice);
		void ClearVoice(RE::FormID a_key);

	public:
		bool InitializeNew(RE::BSFixedString a_voice);
		void SaveYAML(RE::BSFixedString a_voice) const;

		void SetSound(RE::BSFixedString a_voice, LegacyVoice a_legacysetting, RE::TESSound* a_sound);
		void SetTags(RE::BSFixedString a_voice, const std::vector<RE::BSFixedString>& a_tags);
		void SetRace(RE::BSFixedString a_voice, const std::vector<RaceKey>& a_races);
		void SetSex(RE::BSFixedString a_voice, RE::SEXES::SEX a_sex);

	public:
		void Initialize();
		void Save();

	private:
		mutable std::shared_mutex _m{};
		std::vector<VoiceObject> voices{};
		std::map<RE::FormID, const VoiceObject*> saved_voices{};
	};

}	 // namespace Registry::Voice
