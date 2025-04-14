#include "RaceKey.h"

#include "Registry/Util/Scale.h"

namespace Registry
{
	static inline const std::map<RaceKey, RE::BSFixedString> LegacyRaceKeys = {
		{ Human, "Humans" },
		{ AshHopper, "AshHoppers" },
		{ Bear, "Bears" },
		{ Boar, "BoarsAny" },
		{ BoarMounted, "BoarsMounted" },
		{ BoarSingle, "Boars" },
		{ Canine, "Canines" },
		{ Chaurus, "Chaurus" },
		{ ChaurusHunter, "ChaurusHunters" },
		{ ChaurusReaper, "ChaurusReapers" },
		{ Chicken, "Chickens" },
		{ Cow, "Cows" },
		{ Deer, "Deers" },
		{ Dog, "Dogs" },
		{ Dragon, "Dragons" },
		{ DragonPriest, "DragonPriests" },
		{ Draugr, "Draugrs" },
		{ DwarvenBallista, "DwarvenBallistas" },
		{ DwarvenCenturion, "DwarvenCenturions" },
		{ DwarvenSphere, "DwarvenSpheres" },
		{ DwarvenSpider, "DwarvenSpiders" },
		{ Falmer, "Falmers" },
		{ FlameAtronach, "FlameAtronach" },
		{ Fox, "Foxes" },
		{ FrostAtronach, "FrostAtronach" },
		{ Gargoyle, "Gargoyles" },
		{ Giant, "Giants" },
		{ GiantSpider, "GiantSpiders" },
		{ Goat, "Goats" },
		{ Hagraven, "Hagravens" },
		{ Hare, "Rabbits" },
		{ Horker, "Horkers" },
		{ Horse, "Horses" },
		{ IceWraith, "IceWraiths" },
		{ LargeSpider, "LargeSpiders" },
		{ Lurker, "Lurkers" },
		{ Mammoth, "Mammoths" },
		{ Mudcrab, "Mudcrabs" },
		{ Netch, "Netches" },
		{ Riekling, "Rieklings" },
		{ Sabrecat, "Sabrecats" },
		{ Seeker, "Seekers" },
		{ Skeever, "Skeevers" },
		{ Slaughterfish, "Slaughterfishes" },
		{ Spider, "Spiders" },
		{ Spriggan, "Spriggans" },
		{ StormAtronach, "StormAtronach" },
		{ Troll, "Trolls" },
		{ VampireLord, "VampireLords" },
		{ Werewolf, "Werewolves" },
		{ Wisp, "Wisps" },
		{ Wispmother, "Wispmothers" },
		{ Wolf, "Wolves" },
	};

	RaceKey::RaceKey(Value a_value) :
		value(a_value) {}

	RaceKey::RaceKey(RE::Actor* a_actor) :
		RaceKey(a_actor->GetRace(), Scale::GetSingleton()->GetScale(a_actor), a_actor->GetActorBase()->GetSex()) {}

	RaceKey::RaceKey(const RE::BSFixedString& a_raceStr) :
		value(magic_enum::enum_cast<Value>(a_raceStr, magic_enum::case_insensitive).value_or(None))
	{
		if (value != None)
			return;
		const auto where = std::ranges::find_if(LegacyRaceKeys, [&](const auto& pair) { return pair.second == a_raceStr; });
		value = (where == LegacyRaceKeys.end()) ? None : where->first;
	}

	RaceKey::RaceKey(const RE::TESRace* a_race, float a_scale, RE::SEXES::SEX a_sex)
	{
		const std::string_view rootTMP{ a_race->rootBehaviorGraphNames[a_sex].data() };
		const auto root{ rootTMP.substr(rootTMP.rfind('\\') + 1) };
		static const std::map<std::string_view, RaceKey> behaviorfiles{
			{ "0_Master.hkx", Human },
			{ "WolfBehavior.hkx", Wolf },
			{ "DogBehavior.hkx", Dog },
			{ "ChickenBehavior.hkx", Chicken },
			{ "HareBehavior.hkx", Hare },
			{ "AtronachFlameBehavior.hkx", FlameAtronach },
			{ "AtronachFrostBehavior.hkx", FrostAtronach },
			{ "AtronachStormBehavior.hkx", StormAtronach },
			{ "BearBehavior.hkx", Bear },
			{ "ChaurusBehavior.hkx", Chaurus },
			{ "H-CowBehavior.hkx", Cow },
			{ "DeerBehavior.hkx", Deer },
			{ "CHaurusFlyerBehavior.hkx", ChaurusHunter },
			{ "VampireBruteBehavior.hkx", Gargoyle },
			{ "BenthicLurkerBehavior.hkx", Lurker },
			{ "BoarBehavior.hkx", Boar },
			{ "BCBehavior.hkx", DwarvenBallista },
			{ "HMDaedra.hkx", Seeker },
			{ "NetchBehavior.hkx", Netch },
			{ "RieklingBehavior.hkx", Riekling },
			{ "ScribBehavior.hkx", AshHopper },
			{ "DragonBehavior.hkx", Dragon },
			{ "Dragon_Priest.hkx", DragonPriest },
			{ "DraugrBehavior.hkx", Draugr },
			{ "SCBehavior.hkx", DwarvenSphere },
			{ "DwarvenSpiderBehavior.hkx", DwarvenSpider },
			{ "SteamBehavior.hkx", DwarvenCenturion },
			{ "FalmerBehavior.hkx", Falmer },
			{ "FrostbiteSpiderBehavior.hkx", Spider },
			{ "GiantBehavior.hkx", Giant },
			{ "GoatBehavior.hkx", Goat },
			{ "HavgravenBehavior.hkx", Hagraven },
			{ "HorkerBehavior.hkx", Horker },
			{ "HorseBehavior.hkx", Horse },
			{ "IceWraithBehavior.hkx", IceWraith },
			{ "MammothBehavior.hkx", Mammoth },
			{ "MudcrabBehavior.hkx", Mudcrab },
			{ "SabreCatBehavior.hkx", Sabrecat },
			{ "SkeeverBehavior.hkx", Skeever },
			{ "SlaughterfishBehavior.hkx", Slaughterfish },
			{ "SprigganBehavior.hkx", Spriggan },
			{ "TrollBehavior.hkx", Troll },
			{ "VampireLord.hkx", VampireLord },
			{ "WerewolfBehavior.hkx", Werewolf },
			{ "WispBehavior.hkx", Wispmother },
			{ "WitchlightBehavior.hkx", Wisp },
		};
		const auto where = behaviorfiles.find(root);
		if (where == behaviorfiles.end()) {
			logger::error("Unrecognized Behavior: {} (Used by Race {:X})", root, a_race->GetFormID());
			throw std::runtime_error("Unrecognized Behavior: " + std::string{ root });
		}
		switch (where->second) {
		case Boar:
			if (a_race->HasKeyword(GameForms::DLC2RieklingMountedKeyword)) {
				value = BoarMounted;
			} else {
				value = BoarSingle;
			}
			break;
		case Chaurus:
			if ((a_scale == 0.0 ? a_race->data.height[a_sex] : a_scale) < 1.0) {
				value = Chaurus;
			} else {
				value = ChaurusReaper;
			}
			break;
		case Spider:
			if (const auto scale = (a_scale == 0.0) ? a_race->data.height[a_sex] : a_scale; scale < 0.9) {
				value = Spider;
			} else if (scale < 1.5) {
				value = LargeSpider;
			} else {
				value = GiantSpider;
			}
			break;
		case Wolf:
			if (Util::CastLower(std::string{ a_race->formEditorID }).find("fox") != std::string::npos) {
				value = Fox;
			} else {
				value = Wolf;
			}
			break;
		default:
			value = where->second;
			break;
		}
	}

	RE::BSFixedString RaceKey::AsString() const
	{
		const auto where = LegacyRaceKeys.find(value);
		return where != LegacyRaceKeys.end() ? where->second : "";
	}

	bool RaceKey::IsCompatibleWith(RaceKey a_other) const
	{
		switch (value) {
		case Value::Canine:
			return a_other.IsAnyOf(Canine, Dog, Wolf);
		case Value::Boar:
			return a_other.IsAnyOf(Boar, BoarSingle, BoarMounted);
		case Value::Dog:
		case Value::Wolf:
			return a_other.IsAnyOf(Canine, value);
		case Value::BoarSingle:
		case Value::BoarMounted:
			return a_other.IsAnyOf(Boar, value);
		default:
			return a_other.value == value;
		}
	}

	RaceKey RaceKey::GetMetaRace() const
	{
		switch (value) {
		case Value::Wolf:
		case Value::Dog:
			return Value::Canine;
		case Value::BoarMounted:
		case Value::BoarSingle:
			return Value::Boar;
		default:
			return Value::None;
		}
	}

	std::vector<RE::BSFixedString> RaceKey::GetAllRaceKeys(bool a_ignoreAmbiguous)
	{
		auto raceKeys = magic_enum::enum_entries<Value>();
		std::vector<RE::BSFixedString> ret;
		for (auto [enumVal, name] : raceKeys) {
			if (a_ignoreAmbiguous && (enumVal == Value::Boar || enumVal == Value::Canine)) {
				continue;
			}
			ret.push_back(name);
		}
		return ret;
	}

}	 // namespace SexLab::RaceKey
