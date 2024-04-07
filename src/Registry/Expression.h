#pragma once

#include "Registry/Define/Tags.h"

namespace Registry
{
	class Expression :
		public Singleton<Expression>
	{
		static inline const auto EXPRESSION_PATH{ CONFIGPATH("Expressions") };

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

		public:
			Profile() = default;
			Profile(const YAML::Node& a_src);
			Profile(const nlohmann::json& a_src);
			~Profile() = default;

			YAML::Node AsYAML() const;

		public:
			RE::BSFixedString id{ "" };
			TagData tags{};
			std::vector<std::array<float, Total>> data[RE::SEXES::kTotal]{};
			bool enabled{ true };
			bool isdefault{ false};
		};
		static Profile GetDefaultAfraid();
		static Profile GetDefaultAngry();
		static Profile GetDefaultHappy();
		static Profile GetDefaultJoy();
		static Profile GetDefaultPained();
		static Profile GetDefaultPleasure();
		static Profile GetDefaultSad();
		static Profile GetDefaultShy();

	public:
		const Profile* GetProfile(const RE::BSFixedString& a_id) const;
		Profile* GetProfile(const RE::BSFixedString& a_id);
		bool RenameProfile(const RE::BSFixedString& a_id, const RE::BSFixedString& a_newid);

		bool ForEachProfile(std::function<bool(Profile&)> a_func);

		void Initialize();
		void Save(bool verbose = true);

	private:
#define PROFILE_DEFAULT(f) []() { auto ret = f(); return std::pair{ret.id, ret}; }()
		std::map<RE::BSFixedString, Profile, FixedStringCompare> _profiles{
			PROFILE_DEFAULT(GetDefaultAfraid),
			PROFILE_DEFAULT(GetDefaultAngry),
			PROFILE_DEFAULT(GetDefaultHappy),
			PROFILE_DEFAULT(GetDefaultJoy),
			PROFILE_DEFAULT(GetDefaultPained),
			PROFILE_DEFAULT(GetDefaultPleasure),
			PROFILE_DEFAULT(GetDefaultSad),
			PROFILE_DEFAULT(GetDefaultShy)
		};
#undef PROFILE_DEFAULT
	};

}	 // namespace Registry
