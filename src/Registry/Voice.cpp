#include "Voice.h"

namespace Registry
{

	RE::BSFixedString Voice::GetVoice(RE::Actor* a_actor, const TagDetails& tags) const
  {
		auto base = a_actor->GetActorBase();
		auto sex = base ? base->GetSex() : RE::SEXES::kMale;
		auto race = RaceHandler::GetRaceKey(a_actor);
		std::vector<const VoiceObject*> ret;
		for (auto &&voice : voices)
		{
			if (voice.sex != RE::SEXES::kNone && voice.sex != sex)
				continue;
			auto where = std::ranges::find_if(voice.races, [&](auto rk) { return RaceHandler::IsCompatibleRaceKey(rk, race); });
			if (where == voice.races.end())
				continue;
			if (!tags.MatchTags(voice.tags))
				continue;
			ret.push_back(&voice);
		}
		return ret[Random::draw<size_t>(0, ret.size() - 1)]->name;
	}

	RE::TESSound* Voice::PickSound(RE::BSFixedString a_voice, uint32_t a_priority, Stage* a_stage, PositionInfo* a_info, const std::vector<RE::BSFixedString>& a_context) const
	{
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return nullptr;
		}
		for (auto&& vset : v->extrasets) {
			if (vset.IsValid(a_stage, a_info, a_context)) {
				return vset.Get(a_priority);
			}
		}
		return v->defaultset.Get(a_priority);
	}

	void Voice::Initialize()
	{
		for (size_t i = 0; i < static_cast<size_t>(VoiceObject::Defaults::Total); i++) {
			voices.emplace_back(VoiceObject::Defaults(i));
		}

		if (fs::exists(VOICEPATH)) {
			for (auto& file : fs::directory_iterator{ VOICEPATH }) {
				if (const auto ext = file.path().extension(); ext != ".yaml" && ext != ".yml")
					continue;
				const auto filename = file.path().filename().string();
				try {
					const auto root = YAML::LoadFile(file.path().string());
					auto v = VoiceObject{ root };
					if (std::ranges::contains(voices, v)) {
						logger::error("Already added Voice with following ID: {}", v.name);
						continue;
					}
					voices.push_back(std::move(v));
					logger::info("Loaded scene voice {}", filename);
				} catch (const std::exception& e) {
					logger::error("Error while loading scene settings from file {}: {}", filename, e.what());
				}
			}
		}

		if (fs::exists(VOICESETTINGPATH)) {
			try {
				const auto root = YAML::LoadFile(VOICESETTINGPATH);
				for (auto&& it : root) {
					auto where = std::ranges::find(voices, it.first);
					if (where == voices.end())
						continue;

					where->enabled = it.second.as<bool>();
				}
			} catch (const std::exception& e) {
				logger::error("Error while loading voice settings: {}", e.what());
			}
		}

		logger::info("Finished loading registry settings");
	}

	void Voice::Save()
  {
    if (!fs::exists(VOICESETTINGPATH))
      return;

		try {
			auto root = YAML::LoadFile(VOICESETTINGPATH);
			for (auto&& it : voices) {
				root[it.name.data()] = it.enabled;
			}
			std::ofstream fout(VOICESETTINGPATH);
			fout << root;
			logger::info("Finished saving voice settings");
		} catch (const std::exception& e) {
			logger::error("Error while saving voice settings: {}", e.what());
		}
	}

	Voice::VoiceObject::VoiceObject(const YAML::Node& a_node) :
		name(a_node["Name"].as<std::string>()),
		enabled(true),
		sex(a_node["Actor"]["Sex"].as<std::string>() == "Female" ? RE::SEXES::kFemale : RE::SEXES::kMale),
		races([&]() -> decltype(races) {
			auto node = a_node["Actor"]["Race"];
			if (node.IsScalar())
				return { RaceHandler::GetRaceKey(node.as<std::string>()) };

			decltype(races) ret;
			for (auto&& it : node) {
				auto arg = it.as<std::string>();
				ret.push_back(RaceHandler::GetRaceKey(arg));
			}
			return ret;
		}()),
		tags([&]() -> decltype(tags) {
			auto node = a_node["Tags"];
			auto arg = node.IsScalar() ? std::vector<std::string>{ node.as<std::string>() } : node.as<std::vector<std::string>>();
			return { arg };
		}()),
		defaultset(a_node),
		extrasets([&]() {
			decltype(extrasets) ret{};
			ret.emplace_back(a_node);
			auto extra = a_node["Extra"];
			if (extra.IsDefined())
				for (auto&& it : extra) {
					ret.emplace_back(it);
				}
			return ret;
		}())
	{}

	Voice::VoiceObject::VoiceObject(Defaults a_default) :
		enabled(true), races({ RaceKey::Human })
	{
		using CType = VoiceSet::CONDITION::ConditionType;
		auto carg = VoiceSet::CONDITION::Condition();
		carg._bool = true;

		const auto datahandler = RE::TESDataHandler::GetSingleton();
		switch (a_default) {
		case Defaults::FemaleClassic:
			name = "Classic (Female)";
			sex = RE::SEXES::kFemale;
			tags = std::vector{ "Female", "Classic", "Normal" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x67548, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x67546, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x67547, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x67546, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::FemaleBreathy:
			name = "Breathy (Female)";
			sex = RE::SEXES::kFemale;
			tags = std::vector{ "Female", "Breathy", "Loud", "Rough" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x6754B, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x67549, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x6754A, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x67549, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::FemaleYoung:
			name = "Young (Female)";
			sex = RE::SEXES::kFemale;
			tags = std::vector{ "Female", "Young", "Loud" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x6754E, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x6754C, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x6754D, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x6754C, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::FemaleStimulated:
			name = "Stimulated (Female)";
			sex = RE::SEXES::kFemale;
			tags = std::vector{ "Female", "Stimulated", "Loud", "Excited" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x67551, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x6754F, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x67550, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x6754F, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::FemaleQuiet:
			name = "Quiet (Female)";
			sex = RE::SEXES::kFemale;
			tags = std::vector{ "Female", "Quiet", "Timid" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x67554, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x67552, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x67553, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x67552, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::FemaleExcitable:
			name = "Excitable (Female)";
			sex = RE::SEXES::kFemale;
			tags = std::vector{ "Female", "Excitable", "Excited", "Loud" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x67557, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x67555, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x67556, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x67555, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::FemaleAverage:
			name = "Average (Female)";
			sex = RE::SEXES::kFemale;
			tags = std::vector{ "Female", "Average", "Normal", "Harsh" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x6755A, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x67558, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x67559, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x67558, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::FemaleMature:
			name = "Mature (Female)";
			sex = RE::SEXES::kFemale;
			tags = std::vector{ "Female", "Mature", "Old", "Harsh", "Rough" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x6755D, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x6755B, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x6755C, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x6755B, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::MaleNeutral:
			name = "Neutral (Male)";
			sex = RE::SEXES::kMale;
			tags = std::vector{ "Male", "Neutral", "Quiet", "Normal" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x67560, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x6755E, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x6755F, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x6755E, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::MaleCalm:
			name = "Calm (Male)";
			sex = RE::SEXES::kMale;
			tags = std::vector{ "Male", "Calm", "Quiet"};
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x67563, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x67561, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x67562, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x67561, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::MaleRough:
			name = "Rough (Male)";
			sex = RE::SEXES::kMale;
			tags = std::vector{ "Male", "Rough", "Harsh", "Loud", "Old" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x67566, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x67564, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x67565, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x67564, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::MaleAverage:
			name = "Average (Male)";
			sex = RE::SEXES::kMale;
			tags = std::vector{ "Male", "Average", "Normal", "Quiet" };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x67569, "SexLab.esm"), 0 },
					{ datahandler->LookupForm<RE::TESSound>(0x67567, "SexLab.esm"), 75 } }
			};
			extrasets = {
				VoiceSet({ { datahandler->LookupForm<RE::TESSound>(0x67568, "SexLab.esm"), 0 },
									 { datahandler->LookupForm<RE::TESSound>(0x67567, "SexLab.esm"), 75 } },
					{ { CType::Submissive, carg } })
			};
			break;
		case Defaults::ChaurusVoice01:
			name = "Chaurus 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Chaurus, RaceKey::ChaurusReaper, RaceKey::ChaurusReaper };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C090, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::DogVoice01:
			name = "Dog 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Dog };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C091, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::DraugrVoice01:
			name = "Draugr 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Draugr };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C08C, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::FalmerVoice01:
			name = "Falmer 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Falmer };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C08F, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::GiantVoice01:
			name = "Giant 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Giant };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C08E, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::HorseVoice01:
			name = "Horse 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Horse };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C08D, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::SprigganVoice01:
			name = "Spriggan 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Spriggan };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C08B, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::TrollVoice01:
			name = "Troll 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Troll };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C089, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::WerewolfVoice01:
			name = "Werewolf 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Werewolf };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8C08A, "SexLab.esm"), 0 } }
			};
			break;
		case Defaults::WolfVoice01:
			name = "Wolf 1 (Creature)";
			sex = RE::SEXES::kNone;
			races = { RaceKey::Wolf };
			defaultset = VoiceSet{
				{ { datahandler->LookupForm<RE::TESSound>(0x8B5BB, "SexLab.esm"), 0 } }
			};
			break;
		default:
			assert(false);
			throw std::exception(fmt::format("Unrecognized Default Settting {}", a_default).c_str());
		}
	}

	VoiceSet::VoiceSet(const YAML::Node& a_node)
	{
		auto voicevec = a_node["Voices"];
		auto max = voicevec.size();
		size_t i = 0;
		for (auto&& it : voicevec) {
			if (it.IsMap()) {
				auto sound = FormFromString<RE::TESSound*>(it.first.as<std::string>());
				data.emplace_back(sound, static_cast<uint8_t>(it.second.as<uint32_t>()));
			} else {
				auto sound = FormFromString<RE::TESSound*>(it.as<std::string>());
				data.emplace_back(sound, (i++ / max) * 100);
			}
		}
		
		auto convec = a_node["Conditions"];
		if (!convec.IsDefined())
			return;

		for (auto&& con : convec) {
			auto key = con.first.as<std::string>();
			ToLower(key);
			if (key == "submissive") {
				conditions.emplace_back(CONDITION::ConditionType::Submissive, con.second.as<bool>());
				continue;
			}
			auto contype =
				key == "context"	? CONDITION::ConditionType::Context :
				key == "position" ? CONDITION::ConditionType::PositionExtra :
														// key == "tags" ?
														CONDITION::ConditionType::Tag;
			if (con.second.IsScalar()) {
				auto str = con.second.as<std::string>().c_str();
				conditions.emplace_back(contype, _strdup(str));
			} else {
				for (auto&& it : con.second) {
					auto str = it.as<std::string>().c_str();
					conditions.emplace_back(contype, _strdup(str));
				}
			}
		}
	}

	bool VoiceSet::IsValid(Stage* a_stage, PositionInfo* a_position, const std::vector<RE::BSFixedString>& a_context) const
	{
		for (auto&& c : conditions) {
			switch (c.type) {
			case CONDITION::ConditionType::Tag:
				if (!a_stage->tags.HasTag(c.condition._string))
					return false;
				break;
			case CONDITION::ConditionType::Context:
				if (!std::ranges::contains(a_context, RE::BSFixedString(c.condition._string)))
					return false;
				break;
			case CONDITION::ConditionType::PositionExtra:
				if (!a_position->HasExtraCstm(c.condition._string))
					return false;
				break;
			case CONDITION::ConditionType::Submissive:
				if (a_position->IsSubmissive() != c.condition._bool)
					return false;
				break;
			default:
				logger::error("Unrecognized Condition Type {}", c.type);
				break;
			}
		}
		return true;
	}

	RE::TESSound* VoiceSet::Get(uint32_t a_priority) const
	{
		for (int i = data.size() - 1; i >= 0; i--)
		{
			auto&[voice, value] = data[i];
			if (value <= a_priority)
				return voice;
		}
		return nullptr;		
	}

}	 // namespace Registry
