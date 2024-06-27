#pragma once

#include "Registry/Define/Tags.h"

namespace Registry
{
	class Expression :
		public Singleton<Expression>
	{
		enum class DefaultExpression
		{
			Afraid,
			Angry,
			Happy,
			Joy,
			Pained,
			Pleasure,
			Sad,
			Shy
		};

	public:
		struct Profile
		{
			enum
			{
				Phoneme = 0,
				Modifier = 16,
				MoodType = 30,
				MoodValue = 31,

				Total = 32
			};

			enum class Scaling
			{
				Linear,
				Square,
				Cubic,

				Total
			};

		public:
			Profile(const RE::BSFixedString& a_id) :
				id(a_id) { assert(!a_id.empty()); };
			Profile(DefaultExpression a_default);
			Profile(const YAML::Node& a_src);
			Profile(const nlohmann::json& a_src);
			~Profile() = default;

			std::array<float, Total> GetData(RE::SEXES::SEX a_sex, float a_strength) const;
			void Save() const;

		public:
			RE::BSFixedString id;
			uint8_t version{ 0 };
			bool enabled{ true };
			mutable bool has_edits{ false };

			TagData tags{};
			Scaling scaling{ Scaling::Linear };
			std::vector<std::array<float, Total>> data[RE::SEXES::kTotal]{};
		};

	public:
		const Profile* GetProfile(const RE::BSFixedString& a_id) const;
		bool ForEachProfile(std::function<bool(const Profile&)> a_func);

		void UpdateValues(RE::BSFixedString a_id, bool a_female, int a_level, std::vector<float> a_values);
		void UpdateTags(RE::BSFixedString a_id, const TagData& a_newtags);
		void SetScaling(RE::BSFixedString a_id, Profile::Scaling a_scaling);
		void SetEnabled(RE::BSFixedString a_id, bool a_enabled);

		bool CreateProfile(const RE::BSFixedString& a_id);
		void Initialize();
		void Save();

	private:
		std::map<RE::BSFixedString, Profile, FixedStringCompare> _profiles{};
	};

}	 // namespace Registry
