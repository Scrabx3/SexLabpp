#pragma once

namespace Registry
{
	/* 
		Canine can animate both dog and wolf but dog can only animte dog/wolf only wolf
		BoarAny can animate both BoarSingle and BoarMounted but BoarMounted can only animateBoarMounted/BoarSingle only BoarSingle
	*/

	struct RaceKey
	{
		enum Value : uint8_t
		{
			Human = 0,
			AshHopper,
			Bear,
			Boar,
			BoarMounted,
			BoarSingle,
			Canine,
			Chaurus,
			ChaurusHunter,
			ChaurusReaper,
			Chicken,
			Cow,
			Deer,
			Dog,
			Dragon,
			DragonPriest,
			Draugr,
			DwarvenBallista,
			DwarvenCenturion,
			DwarvenSphere,
			DwarvenSpider,
			Falmer,
			FlameAtronach,
			Fox,
			FrostAtronach,
			Gargoyle,
			Giant,
			GiantSpider,
			Goat,
			Hagraven,
			Hare,
			Horker,
			Horse,
			IceWraith,
			LargeSpider,
			Lurker,
			Mammoth,
			Mudcrab,
			Netch,
			Riekling,
			Sabrecat,
			Seeker,
			Skeever,
			Slaughterfish,
			Spider,
			Spriggan,
			StormAtronach,
			Troll,
			VampireLord,
			Werewolf,
			Wisp,
			Wispmother,
			Wolf,

			None = static_cast<std::underlying_type_t<RaceKey>>(-1),
		};

		constexpr RaceKey() = default;
		constexpr RaceKey(Value a_value);
		RaceKey(RE::Actor* a_actor);
		RaceKey(const RE::BSFixedString& a_raceStr);
		RaceKey(const RE::TESRace* a_race, float a_scale = 0.0f, RE::SEXES::SEX a_sex = RE::SEXES::SEX::kMale);

		_NODISCARD RaceKey GetMetaRace() const;
		_NODISCARD RE::BSFixedString AsString() const;
		_NODISCARD bool IsCompatibleWith(RaceKey a_other) const;

		template <typename... T>
		_NODISCARD bool IsAnyOf(T... a_values) const
			requires(std::same_as<T, Value>&&...)
		{
			return a_values == value || ...;
		}
		_NODISCARD bool Is(Value a_value) const { return value == a_value; }
		_NODISCARD bool IsValid() const { return value != Value::None; }

	public:
		_NODISCARD static std::vector<RE::BSFixedString> GetAllRaceKeys(bool a_ignoreAmbiguous);

	public:
		constexpr bool operator==(const RaceKey& a_rhs) const { return value == a_rhs.value; }
		constexpr bool operator!=(const RaceKey& a_rhs) const { return value != a_rhs.value; }
		constexpr bool operator<(const RaceKey& a_rhs) const { return value < a_rhs.value; }

		constexpr bool operator==(const Value& a_rhs) const { return value == a_rhs; }
		constexpr bool operator!=(const Value& a_rhs) const { return value != a_rhs; }

		operator Value() const { return value; }

	public:
		Value value{ Value::None };
	};

}	 // namespace Registry
