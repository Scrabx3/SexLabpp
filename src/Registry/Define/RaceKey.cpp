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

	RaceKey RaceHandler::GetRaceKey(RE::Actor* a_actor)
	{
		const auto base = a_actor->GetActorBase();
		const auto race = a_actor->GetRace();
		const auto sex = base->GetSex();
		if (!race || !base || sex == RE::SEXES::kNone) {
			logger::critical("Actor {:X} (Base: {:X}) has no race/base/sex", a_actor->GetFormID(), base ? base->GetFormID() : 0);
			return RaceKey::None;
		}
		const std::string_view rootTMP{ race->rootBehaviorGraphNames[sex].data() };
		const RE::BSFixedString root{ rootTMP.substr(rootTMP.rfind('\\') + 1) };
		if (root == "0_Master.hkx") {
			return RaceKey::Human;
		}

		static const std::unordered_map<RE::BSFixedString, RaceKey> behaviorfiles{
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
			logger::error("Unrecognized Race: {:X}", race->GetFormID());
			return RaceKey::None;
		}
		switch (where->second) {
		case RaceKey::Boar:
			if (race->HasKeywordString("DLC2RieklingMountedKeyword")) {
				return RaceKey::BoarMounted;
			} else {
				return RaceKey::BoarSingle;
			}
			break;
		case RaceKey::Chaurus:
			if (race->data.height[sex] < 1.0) {
				return RaceKey::Chaurus;
			} else {
				return RaceKey::ChaurusReaper;
			}
		case RaceKey::Spider:
			if (auto scale = Scale::GetSingleton()->GetScale(a_actor); scale < 0.9) {
				return RaceKey::Spider;
			} else if (scale < 1.5) {
				return RaceKey::LargeSpider;
			} else {
				return RaceKey::GiantSpider;
			}
		case RaceKey::Wolf:
			if (SexLab::AsLower(std::string{ race->formEditorID }).find("fox") != std::string::npos) {
				return RaceKey::Fox;
			} else {
				return RaceKey::Wolf;
			}
		default:
			return where->second;
		}
	}

	// std::optional<RaceKey> MapStringToRaceKey(const std::string_view a_string)
	// {
	// 	for (auto&& [str, key] : race_map) {
	// 		if (SexLab::IsEqualString(str, a_string)) {
	// 			return key;
	// 		}
	// 	}
	// 	std::nullopt;
	// }

	// std::optional<std::string_view> MapRaceKeyToString(const RaceKey a_key)
	// {
	// 	for (auto&& [str, key] : race_map) {
	// 		if (key == a_key) {
	// 			return str;
	// 		}
	// 	}
	// 	std::nullopt;
	// }


	// std::pair<int, RaceExtra> GetRaceID(std::string a_racekey)
	// {
	// 	SexLab::ToLower(a_racekey);
	// 	RaceExtra extra;
	// 	if (a_racekey == "wolves") {
	// 		a_racekey = "canines";
	// 		extra = RaceExtra::Wolf;
	// 	} else if (a_racekey == "dogs") {
	// 		a_racekey = "canines";
	// 		extra = RaceExtra::Dog;
	// 	} else {
	// 		extra = RaceExtra::None;
	// 	}
	// 	const auto w = std::find(RaceKeys.begin(), RaceKeys.end(), a_racekey);
	// 	if (w == RaceKeys.end())
	// 		return { 0, RaceExtra::None };

	// 	return { static_cast<int32_t>(w - RaceKeys.begin()), extra };
	// }

	// std::string GetRaceKey(std::pair<int, RaceExtra> a_raceid)
	// {
	// 	switch (a_raceid.second) {
	// 	case RaceExtra::Dog:
	// 		return "dogs"s;
	// 	case RaceExtra::Wolf:
	// 		return "wolves"s;
	// 	default:
	// 		return GetRaceKey(a_raceid.first);
	// 	}
	// }

	// std::string GetRaceKey(const uint32_t a_rawraceid)
	// {
	// 	if (a_rawraceid < RaceKeys.size())
	// 		return RaceKeys[a_rawraceid];

	// 	return ""s;
	// }

}	 // namespace SexLab::RaceKey
