#include "RaceKey.h"

#include "Registry/Util/Scale.h"

namespace Registry
{
	// static inline std::vector<std::pair<std::string_view, RaceKey>> race_map{
	// 	{ "Human", { RaceKey::Human, RaceExtra::None } },
	// 	{ "Ash Hopper", { RaceKey::AshHopper, RaceExtra::None } },
	// 	{ "Bear", { RaceKey::Bear, RaceExtra::None } },
	// 	{ "Boar", { RaceKey::Boar, RaceExtra::BoarSingle } },
	// 	{ "Boar (Any)", { RaceKey::Boar, RaceExtra::None } },
	// 	{ "Boar (Mounted)", { RaceKey::Boar, RaceExtra::BoarMounted } },
	// 	{ "Canine", { RaceKey::Canine, RaceExtra::None } },
	// 	{ "Chaurus", { RaceKey::Chaurus, RaceExtra::None } },
	// 	{ "Chaurus Hunter", { RaceKey::ChaurusHunter, RaceExtra::None } },
	// 	{ "Chaurus Reaper", { RaceKey::ChaurusReaper, RaceExtra::None } },
	// 	{ "Chicken", { RaceKey::Chicken, RaceExtra::None } },
	// 	{ "Cow", { RaceKey::Cow, RaceExtra::None } },
	// 	{ "Deer", { RaceKey::Deer, RaceExtra::None } },
	// 	{ "Dog", { RaceKey::Canine, RaceExtra::Dog } },
	// 	{ "Dragon Priest", { RaceKey::DragonPriest, RaceExtra::None } },
	// 	{ "Dragon", { RaceKey::Dragon, RaceExtra::None } },
	// 	{ "Draugr", { RaceKey::Draugr, RaceExtra::None } },
	// 	{ "Dwarven Ballista", { RaceKey::DwarvenBallista, RaceExtra::None } },
	// 	{ "Dwarven Centurion", { RaceKey::DwarvenCenturion, RaceExtra::None } },
	// 	{ "Dwarven Sphere", { RaceKey::DwarvenSphere, RaceExtra::None } },
	// 	{ "Dwarven Spider", { RaceKey::DwarvenSpider, RaceExtra::None } },
	// 	{ "Falmer", { RaceKey::Falmer, RaceExtra::None } },
	// 	{ "Flame Atronach", { RaceKey::FlameAtronach, RaceExtra::None } },
	// 	{ "Fox", { RaceKey::Fox, RaceExtra::None } },
	// 	{ "Frost Atronach", { RaceKey::FrostAtronach, RaceExtra::None } },
	// 	{ "Gargoyle", { RaceKey::Gargoyle, RaceExtra::None } },
	// 	{ "Giant", { RaceKey::Giant, RaceExtra::None } },
	// 	{ "Goat", { RaceKey::Goat, RaceExtra::None } },
	// 	{ "Hagraven", { RaceKey::Hagraven, RaceExtra::None } },
	// 	{ "Horker", { RaceKey::Horker, RaceExtra::None } },
	// 	{ "Horse", { RaceKey::Horse, RaceExtra::None } },
	// 	{ "Ice Wraith", { RaceKey::IceWraith, RaceExtra::None } },
	// 	{ "Lurker", { RaceKey::Lurker, RaceExtra::None } },
	// 	{ "Mammoth", { RaceKey::Mammoth, RaceExtra::None } },
	// 	{ "Mudcrab", { RaceKey::Mudcrab, RaceExtra::None } },
	// 	{ "Netch", { RaceKey::Netch, RaceExtra::None } },
	// 	{ "Rabbit", { RaceKey::Hare, RaceExtra::None } },
	// 	{ "Riekling", { RaceKey::Riekling, RaceExtra::None } },
	// 	{ "Sabrecat", { RaceKey::Sabrecat, RaceExtra::None } },
	// 	{ "Seeker", { RaceKey::Seeker, RaceExtra::None } },
	// 	{ "Skeever", { RaceKey::Skeever, RaceExtra::None } },
	// 	{ "Slaughterfish", { RaceKey::Slaughterfish, RaceExtra::None } },
	// 	{ "Storm Atronach", { RaceKey::StormAtronach, RaceExtra::None } },
	// 	{ "Spider", { RaceKey::Spider, RaceExtra::None } },
	// 	{ "Large Spider", { RaceKey::LargeSpider, RaceExtra::None } },
	// 	{ "Giant Spider", { RaceKey::GiantSpider, RaceExtra::None } },
	// 	{ "Spriggan", { RaceKey::Spriggan, RaceExtra::None } },
	// 	{ "Troll", { RaceKey::Troll, RaceExtra::None } },
	// 	{ "Vampire Lord", { RaceKey::VampireLord, RaceExtra::None } },
	// 	{ "Werewolf", { RaceKey::Werewolf, RaceExtra::None } },
	// 	{ "Wispmother", { RaceKey::Wispmother, RaceExtra::None } },
	// 	{ "Wisp", { RaceKey::Wisp, RaceExtra::None } },
	// 	{ "Wolf", { RaceKey::Canine, RaceExtra::Wolf } }
	// };

	static inline const std::map<RaceKey, RE::BSFixedString> LegacyRaceKeys = {
		{ RaceKey::Human, "humans" },
		{ RaceKey::AshHopper, "ashhoppers" },
		{ RaceKey::Bear, "bears" },
		{ RaceKey::Boar, "boarsany" },
		{ RaceKey::BoarMounted, "boarsmounted" },
		{ RaceKey::BoarSingle, "boars" },
		{ RaceKey::Canine, "canines" },
		{ RaceKey::Chaurus, "chaurus" },
		{ RaceKey::ChaurusHunter, "chaurushunters" },
		{ RaceKey::ChaurusReaper, "chaurusreapers" },
		{ RaceKey::Chicken, "chickens" },
		{ RaceKey::Cow, "cows" },
		{ RaceKey::Deer, "deers" },
		{ RaceKey::Dog, "dogs" },
		{ RaceKey::Dragon, "dragons" },
		{ RaceKey::DragonPriest, "dragonpriests" },
		{ RaceKey::Draugr, "draugrs" },
		{ RaceKey::DwarvenBallista, "dwarvenballistas" },
		{ RaceKey::DwarvenCenturion, "dwarvencenturions" },
		{ RaceKey::DwarvenSphere, "dwarvenspheres" },
		{ RaceKey::DwarvenSpider, "dwarvenspiders" },
		{ RaceKey::Falmer, "falmers" },
		{ RaceKey::FlameAtronach, "flameatronach" },
		{ RaceKey::Fox, "foxes" },
		{ RaceKey::FrostAtronach, "frostatronach" },
		{ RaceKey::Gargoyle, "gargoyles" },
		{ RaceKey::Giant, "giants" },
		{ RaceKey::GiantSpider, "giantspiders" },
		{ RaceKey::Goat, "goats" },
		{ RaceKey::Hagraven, "hagravens" },
		{ RaceKey::Hare, "rabbits" },
		{ RaceKey::Horker, "horkers" },
		{ RaceKey::Horse, "horses" },
		{ RaceKey::IceWraith, "icewraiths" },
		{ RaceKey::LargeSpider, "largespiders" },
		{ RaceKey::Lurker, "lurkers" },
		{ RaceKey::Mammoth, "mammoths" },
		{ RaceKey::Mudcrab, "mudcrabs" },
		{ RaceKey::Netch, "netches" },
		{ RaceKey::Riekling, "rieklings" },
		{ RaceKey::Sabrecat, "sabrecats" },
		{ RaceKey::Seeker, "seekers" },
		{ RaceKey::Skeever, "skeevers" },
		{ RaceKey::Slaughterfish, "slaughterfishes" },
		{ RaceKey::Spider, "spiders" },
		{ RaceKey::Spriggan, "spriggans" },
		{ RaceKey::StormAtronach, "stormatronach" },
		{ RaceKey::Troll, "trolls" },
		{ RaceKey::VampireLord, "vampirelords" },
		{ RaceKey::Werewolf, "werewolves" },
		{ RaceKey::Wisp, "wisps" },
		{ RaceKey::Wispmother, "wispmothers" },
		{ RaceKey::Wolf, "wolves" },
	};

	RaceKey RaceHandler::GetRaceKey(RE::Actor* a_actor)
	{
		const auto base = a_actor->GetActorBase();
		const auto race = a_actor->GetRace();
		const auto sex = base->GetSex();
		if (!race || !base || sex == RE::SEXES::kNone) {
			logger::critical("Actor {:X} (Base: {:X}) has no race/base/sex", a_actor->GetFormID(), base ? base->GetFormID() : 0);
			return RaceKey::None;
		}
		return GetRaceKey(race, Scale::GetSingleton()->GetScale(a_actor), sex);
	}

	RaceKey RaceHandler::GetRaceKey(const RE::TESRace* a_race, float a_scale, RE::SEXES::SEX a_sex)
	{
		const std::string_view rootTMP{ a_race->rootBehaviorGraphNames[a_sex].data() };
		const auto root{ rootTMP.substr(rootTMP.rfind('\\') + 1) };
		if (root == "0_Master.hkx") {
			return RaceKey::Human;
		}

		static const std::unordered_map<std::string_view, RaceKey> behaviorfiles{
			{ "WolfBehavior.hkx", RaceKey::Wolf },
			{ "DogBehavior.hkx", RaceKey::Dog },
			{ "ChickenBehavior.hkx", RaceKey::Chicken },
			{ "HareBehavior.hkx", RaceKey::Hare },
			{ "AtronachFlameBehavior.hkx", RaceKey::FlameAtronach },
			{ "AtronachFrostBehavior.hkx", RaceKey::FrostAtronach },
			{ "AtronachStormBehavior.hkx", RaceKey::StormAtronach },
			{ "BearBehavior.hkx", RaceKey::Bear },
			{ "ChaurusBehavior.hkx", RaceKey::Chaurus },
			{ "H-CowBehavior.hkx", RaceKey::Cow },
			{ "DeerBehavior.hkx", RaceKey::Deer },
			{ "CHaurusFlyerBehavior.hkx", RaceKey::ChaurusHunter },
			{ "VampireBruteBehavior.hkx", RaceKey::Gargoyle },
			{ "BenthicLurkerBehavior.hkx", RaceKey::Lurker },
			{ "BoarBehavior.hkx", RaceKey::Boar },
			{ "BCBehavior.hkx", RaceKey::DwarvenBallista },
			{ "HMDaedra.hkx", RaceKey::Seeker },
			{ "NetchBehavior.hkx", RaceKey::Netch },
			{ "RieklingBehavior.hkx", RaceKey::Riekling },
			{ "ScribBehavior.hkx", RaceKey::AshHopper },
			{ "DragonBehavior.hkx", RaceKey::Dragon },
			{ "Dragon_Priest.hkx", RaceKey::DragonPriest },
			{ "DraugrBehavior", RaceKey::Draugr },
			{ "SCBehavior.hkx", RaceKey::DwarvenSphere },
			{ "DwarvenSpiderBehavior.hkx", RaceKey::DwarvenSpider },
			{ "SteamBehavior.hkx", RaceKey::DwarvenCenturion },
			{ "FalmerBehavior.hkx", RaceKey::Falmer },
			{ "FrostBiteSpiderBehavior.hkx", RaceKey::Spider },
			{ "GiantBehavior.hkx", RaceKey::Giant },
			{ "GoatBehavior.hkx", RaceKey::Goat },
			{ "HavgravenBehavior.hkx", RaceKey::Hagraven },
			{ "HorkerBehavior.hkx", RaceKey::Horker },
			{ "HorseBehavior.hkx", RaceKey::Horse },
			{ "IceWraithBehavior.hkx", RaceKey::IceWraith },
			{ "MammothBehavior.hkx", RaceKey::Mammoth },
			{ "MudcrabBehavior.hkx", RaceKey::Mudcrab },
			{ "SabreCatBehavior.hkx", RaceKey::Sabrecat },
			{ "SkeeverBehavior.hkx", RaceKey::Skeever },
			{ "SlaughterfishBehavior.hkx", RaceKey::Slaughterfish },
			{ "SprigganBehavior.hkx", RaceKey::Spriggan },
			{ "TrollBehavior.hkx", RaceKey::Troll },
			{ "VampireLord.hkx", RaceKey::VampireLord },
			{ "WerewolfBehavior.hkx", RaceKey::Werewolf },
			{ "WispBehavior.hkx", RaceKey::Wispmother },
			{ "WitchlightBehavior.hkx", RaceKey::Wisp },
		};

		const auto where = behaviorfiles.find(root);
		if (where == behaviorfiles.end()) {
			logger::error("Unrecognized Behavior: {} (Used by Race {:X})", root, a_race->GetFormID());
			return RaceKey::None;
		}
		switch (where->second) {
		case RaceKey::Boar:
			if (a_race->HasKeyword(GameForms::DLC2RieklingMountedKeyword)) {
				return RaceKey::BoarMounted;
			} else {
				return RaceKey::BoarSingle;
			}
			break;
		case RaceKey::Chaurus:
			if (auto scale = a_scale == 0.0 ? a_race->data.height[a_sex] : a_scale; scale < 1.0) {
				return RaceKey::Chaurus;
			} else {
				return RaceKey::ChaurusReaper;
			}
		case RaceKey::Spider:
			if (auto scale = a_scale == 0.0 ? a_race->data.height[a_sex] : a_scale; scale < 0.9) {
				return RaceKey::Spider;
			} else if (scale < 1.5) {
				return RaceKey::LargeSpider;
			} else {
				return RaceKey::GiantSpider;
			}
		case RaceKey::Wolf:
			if (AsLower(std::string{ a_race->formEditorID }).find("fox") != std::string::npos) {
				return RaceKey::Fox;
			} else {
				return RaceKey::Wolf;
			}
		default:
			return where->second;
		}
	}

	RaceKey RaceHandler::GetRaceKey(const RE::BSFixedString& a_racestring)
	{
		for (auto&& [racekey, racestring] : LegacyRaceKeys) {
			if (a_racestring == racestring) {
				return racekey;
			}
		}
		return RaceKey::None;
	}

	RaceKey RaceHandler::GetVariantKey(RaceKey a_racekey)
	{
		switch (a_racekey) {
		case RaceKey::Wolf:
		case RaceKey::Dog:
			return RaceKey::Canine;
		case RaceKey::BoarMounted:
		case RaceKey::BoarSingle:
			return RaceKey::Boar;
		default:
			return RaceKey::None;
		}
	}

	bool RaceHandler::HasRaceKey(RE::Actor* a_actor, const RE::BSFixedString& a_racekey)
	{
		const auto key = GetRaceKey(a_actor);
		if (key == RaceKey::None) {
			return false;
		}
		// key returns the lowest instance of a representing race,
		// i.e. no canine or boars as these group 2 racekeys together
		if (a_racekey == "canines") {
			return key == RaceKey::Dog || key == RaceKey::Wolf;
		} else if (a_racekey == "boarsany") {
			return key == RaceKey::BoarMounted || key == RaceKey::BoarSingle;
		}
		const auto where = LegacyRaceKeys.find(key);
		return where != LegacyRaceKeys.end() && where->second == a_racekey;
	}

	bool RaceHandler::HasRaceKey(RE::Actor* a_actor, RaceKey a_racekey)
	{
		const auto key = GetRaceKey(a_actor);
		switch (a_racekey) {
		case RaceKey::None:
			return false;
		case RaceKey::Canine:
			return key == RaceKey::Dog || key == RaceKey::Wolf;
		case RaceKey::Boar:
			return key == RaceKey::BoarSingle || key == RaceKey::BoarMounted;
		default:
			return key == a_racekey;
		}
	}

	bool RaceHandler::IsCompatibleRaceKey(RaceKey a_racekey1, RaceKey a_racekey2)
	{
		switch (a_racekey1) {
		case RaceKey::Canine:
			return a_racekey2 == RaceKey::Canine || a_racekey2 == RaceKey::Dog || a_racekey2 == RaceKey::Wolf;
		case RaceKey::Boar:
			return a_racekey2 == RaceKey::Boar || a_racekey2 == RaceKey::BoarSingle || a_racekey2 == RaceKey::BoarMounted;
		case RaceKey::Dog:
		case RaceKey::Wolf:
			return a_racekey2 == RaceKey::Canine || a_racekey1 == a_racekey2;
		case RaceKey::BoarSingle:
		case RaceKey::BoarMounted:
			return a_racekey2 == RaceKey::Boar || a_racekey1 == a_racekey2;
		default:
			return a_racekey1 == a_racekey2;
		}
	}

	RE::BSFixedString RaceHandler::AsString(RaceKey a_racekey)
	{
		const auto where = LegacyRaceKeys.find(a_racekey);
		if (where == LegacyRaceKeys.end())
		 	return "";
		return where->second;
	}

	std::vector<RE::BSFixedString> RaceHandler::GetAllRaceKeys(bool a_ignoreambiguous)
	{
		std::vector<RE::BSFixedString> ret {
			AsString(RaceKey::AshHopper),
			AsString(RaceKey::Bear),
			AsString(RaceKey::BoarMounted),
			AsString(RaceKey::BoarSingle),
			AsString(RaceKey::Chaurus),
			AsString(RaceKey::ChaurusHunter),
			AsString(RaceKey::ChaurusReaper),
			AsString(RaceKey::Chicken),
			AsString(RaceKey::Cow),
			AsString(RaceKey::Deer),
			AsString(RaceKey::Dog),
			AsString(RaceKey::Dragon),
			AsString(RaceKey::DragonPriest),
			AsString(RaceKey::Draugr),
			AsString(RaceKey::DwarvenBallista),
			AsString(RaceKey::DwarvenCenturion),
			AsString(RaceKey::DwarvenSphere),
			AsString(RaceKey::DwarvenSpider),
			AsString(RaceKey::Falmer),
			AsString(RaceKey::FlameAtronach),
			AsString(RaceKey::Fox),
			AsString(RaceKey::FrostAtronach),
			AsString(RaceKey::Gargoyle),
			AsString(RaceKey::Giant),
			AsString(RaceKey::GiantSpider),
			AsString(RaceKey::Goat),
			AsString(RaceKey::Hagraven),
			AsString(RaceKey::Hare),
			AsString(RaceKey::Horker),
			AsString(RaceKey::Horse),
			AsString(RaceKey::IceWraith),
			AsString(RaceKey::LargeSpider),
			AsString(RaceKey::Lurker),
			AsString(RaceKey::Mammoth),
			AsString(RaceKey::Mudcrab),
			AsString(RaceKey::Netch),
			AsString(RaceKey::Riekling),
			AsString(RaceKey::Sabrecat),
			AsString(RaceKey::Seeker),
			AsString(RaceKey::Skeever),
			AsString(RaceKey::Slaughterfish),
			AsString(RaceKey::Spider),
			AsString(RaceKey::Spriggan),
			AsString(RaceKey::StormAtronach),
			AsString(RaceKey::Troll),
			AsString(RaceKey::VampireLord),
			AsString(RaceKey::Werewolf),
			AsString(RaceKey::Wisp),
			AsString(RaceKey::Wispmother),
			AsString(RaceKey::Wolf),
		};
		if (!a_ignoreambiguous) {
			ret.push_back(AsString(RaceKey::Boar));
			ret.push_back(AsString(RaceKey::Canine));
		}
		return ret;
	}

}	 // namespace SexLab::RaceKey
