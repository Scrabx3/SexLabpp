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

	enum class VoiceAnnotation
	{
		None = 0,
		Submissive = 1 << 0,
		Dominant = 1 << 1,
		Muffled = 1 << 7,
	};

	enum class Pitch
	{
		Unknown,
		Normal,
		High,
		Low,
	};

	struct VoiceSet
	{
		VoiceSet(const YAML::Node& a_node);
		VoiceSet(bool a_aslegacyextra);
		~VoiceSet() = default;

		bool IsValid(REX::EnumSet<VoiceAnnotation> a_annotations) const { return annotations == a_annotations; }
		RE::TESSound* GetOrgasm() const { return orgasm; }
		RE::TESSound* Get(uint32_t a_excitement) const;
		RE::TESSound* Get(LegacyVoice a_setting) const;

	public:
		void SetSound(bool front, RE::TESSound* a_sound);
		YAML::Node AsYaml() const;

	private:
		RE::TESSound* orgasm{ nullptr };
		std::vector<std::pair<RE::TESSound*, uint8_t>> data{};
		REX::EnumSet<VoiceAnnotation> annotations{ VoiceAnnotation::None };
	};

	class Voice :
		public Singleton<Voice>
	{
		static constexpr const char* VOICE_PATH{ CONFIGPATH("Voices\\Voices") };
		static constexpr const char* VOICE_PATH_PITCH{ CONFIGPATH("Voices\\Pitch") };
		static constexpr const char* VOICE_SETTING_PATH{ USERDATAPATH("Voices.yaml") };
		static constexpr const char* VOICE_SETTINGS_CACHES_PATH{ USERDATAPATH("Voices_NPC.yaml") };

	public:
		struct VoiceObject
		{
			VoiceObject(const YAML::Node& a_node);
			VoiceObject(RE::BSFixedString a_name) :
				name(a_name), fromfile(false), defaultset(false), extrasets({ { true } }) {}
			~VoiceObject() = default;

			RE::BSFixedString name;
			bool enabled{ true };
			bool fromfile{ true };

			RE::SEXES::SEX sex{ RE::SEXES::SEX::kNone };
			std::vector<RaceKey> races{};
			Pitch pitch{ Pitch::Unknown };

			TagData tags{};
			VoiceSet defaultset;
			std::vector<VoiceSet> extrasets{};

		public:
			YAML::Node AsYaml() const;
			const VoiceSet& GetApplicableSet(REX::EnumSet<VoiceAnnotation> a_annotation) const;
			bool operator==(const VoiceObject& a_rhs) const noexcept { return name == a_rhs.name; }
		};

		struct PitchOrVoice
		{
			PitchOrVoice(const VoiceObject* a_voice) :
				voice(a_voice) {}
			PitchOrVoice(Pitch a_pitch) :
				pitch(a_pitch) {}
			~PitchOrVoice() = default;

			Pitch pitch{ Pitch::Unknown };
			const VoiceObject* voice{ nullptr };
		};

	public:
		std::vector<RE::BSFixedString> GetAllVoiceNames(RaceKey a_race) const;
		const VoiceObject* GetVoice(RE::Actor* a_actor, const TagDetails& tags);
		const VoiceObject* GetVoice(const TagDetails& tags) const;
		const VoiceObject* GetVoice(RE::BSFixedString a_voice) const;
		const VoiceObject* GetVoice(RaceKey a_race) const;
		void SetVoiceObjectEnabled(RE::BSFixedString a_voice, bool a_enabled);

		RE::TESSound* PickSound(RE::BSFixedString a_voice, LegacyVoice a_legacysetting) const;
		RE::TESSound* PickSound(RE::BSFixedString a_voice, uint32_t a_excitement, REX::EnumSet<VoiceAnnotation> a_annotation) const;
		RE::TESSound* PickOrgasmSound(RE::BSFixedString a_voice, REX::EnumSet<VoiceAnnotation> a_annotation) const;

		std::vector<RE::Actor*> GetSavedActors() const;
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
		void LoadVoices();
		void LoadPitches();
		void LoadSettings();
		void LoadCache();
		void SaveSettings();
		void SaveCache();

	private:
		mutable std::shared_mutex _m{};
		std::vector<VoiceObject> voices{};
		std::map<RE::FormID, PitchOrVoice> saved_pitches{};
		std::map<RE::FormID, const VoiceObject*> saved_voices{};
	};

}	 // namespace Registry::Voice
