#pragma once

#include "Registry/Define/Tags.h"

namespace Registry
{
	class Expression :
		public Singleton<Expression>
	{
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

			RE::BSFixedString id{ "" };
			TagData tags{};
			std::vector<std::array<float, Total>> data[RE::SEXES::kTotal]{};
			bool enabled{ true };
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
		void Initialize();
		void Save();

	private:
		std::vector<Profile> _profiles;
	};

}	 // namespace Registry
