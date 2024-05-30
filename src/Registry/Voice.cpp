#include "Voice.h"

namespace Registry
{
	std::vector<RE::BSFixedString> Voice::GetAllVoiceNames() const
	{
		std::shared_lock lock{ _m };
		std::vector<RE::BSFixedString> ret{};
		ret.reserve(voices.size());
		for (auto&& v : voices) {
			ret.push_back(v.name);
		}
		return ret;
	}

	const Voice::VoiceObject* Voice::GetVoice(RE::Actor* a_actor, const TagDetails& tags) const
	{
		std::shared_lock lock{ _m };
		if (auto saved = GetSavedVoice(a_actor->GetFormID()))
			return saved;

		auto base = a_actor->GetActorBase();
		auto sex = base ? base->GetSex() : RE::SEXES::kMale;
		auto race = RaceHandler::GetRaceKey(a_actor);
		if (race == RaceKey::None) {
			logger::error("Actor {} has invalid racekey", a_actor->GetFormID());
			return nullptr;
		}
		std::vector<const VoiceObject*> ret{};
		for (auto&& voice : voices) {
			if (voice.sex != RE::SEXES::kNone && voice.sex != sex)
				continue;
			auto where = std::ranges::find_if(voice.races, [&](auto rk) { return RaceHandler::IsCompatibleRaceKey(rk, race); });
			if (where == voice.races.end())
				continue;
			if (!tags.MatchTags(voice.tags))
				continue;
			ret.push_back(&voice);
		}
		if (ret.empty()) {
			return nullptr;
		}
		auto v = ret[Random::draw<size_t>(0, ret.size() - 1)];
		saved_voices[a_actor->formID] = v;
		return v;
	}

	const Voice::VoiceObject* Voice::GetVoice(const TagDetails& tags) const
	{
		std::shared_lock lock{ _m };
		std::vector<const VoiceObject*> ret{};
		for (auto&& voice : voices) {
			if (!tags.MatchTags(voice.tags))
				continue;
			ret.push_back(&voice);
		}
		return ret.empty() ? nullptr : ret[Random::draw<size_t>(0, ret.size() - 1)];
	}

	const Voice::VoiceObject* Voice::GetVoice(RaceKey a_race) const
	{
		std::shared_lock lock{ _m };
		std::vector<const VoiceObject*> ret{};
		for (auto&& voice : voices) {
			auto where = std::ranges::find_if(voice.races, [&](auto rk) { return RaceHandler::IsCompatibleRaceKey(rk, a_race); });
			if (where == voice.races.end())
				continue;
			ret.push_back(&voice);
		}
		return ret.empty() ? nullptr : ret[Random::draw<size_t>(0, ret.size() - 1)];
	}

	const Voice::VoiceObject* Voice::GetVoice(RE::BSFixedString a_voice) const
	{
		std::shared_lock lock{ _m };
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		return v == voices.end() ? nullptr : v._Ptr;
	}

	void Voice::SetVoiceObjectEnabled(RE::BSFixedString a_voice, bool a_enabled)
	{
		std::unique_lock lock{ _m };
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return;
		}
		v->enabled = a_enabled;
	}

	RE::TESSound* Voice::PickSound(RE::BSFixedString a_voice, uint32_t a_priority, const Stage* a_stage, const PositionInfo* a_info, const std::vector<RE::BSFixedString>& a_context) const
	{
		std::shared_lock lock{ _m };
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

	RE::TESSound* Voice::PickSound(RE::BSFixedString a_voice, LegacyVoice a_legacysetting) const
	{
		std::shared_lock lock{ _m };
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return nullptr;
		}
		bool maybe_victim = a_legacysetting == LegacyVoice::Medium;
		return maybe_victim && !v->extrasets.empty() ? v->extrasets.front().Get(a_legacysetting) : v->defaultset.Get(a_legacysetting);
	}

	RE::TESSound* Voice::GetOrgasmSound(RE::BSFixedString a_voice, const Stage* a_stage, const PositionInfo* a_info, const std::vector<RE::BSFixedString>& a_context) const
	{
		std::shared_lock lock{ _m };
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return nullptr;
		}
		if (v->orgasmvfx)
			return v->orgasmvfx;
		return PickSound(a_voice, 100, a_stage, a_info, a_context);
	}

	const Voice::VoiceObject* Voice::GetSavedVoice(RE::FormID a_key) const
	{
		std::shared_lock lock{ _m };
		auto w = saved_voices.find(a_key);
		return w == saved_voices.end() ? nullptr : w->second;
	}

	void Voice::SaveVoice(RE::FormID a_key, RE::BSFixedString a_voice)
	{
		std::unique_lock lock{ _m };
		saved_voices.insert_or_assign(a_key, GetVoice(a_voice));
	}

	void Voice::ClearVoice(RE::FormID a_key)
	{
		std::unique_lock lock{ _m };
		saved_voices.erase(a_key);
	}

	bool Voice::InitializeNew(RE::BSFixedString a_voice)
	{
		std::unique_lock lock{ _m };
		if (std::ranges::contains(voices, a_voice)) {
			logger::error("Voice {} has already been initialized", a_voice);
			return false;
		}
		voices.emplace_back(a_voice);
		return true;
	}

	void Voice::SetSound(RE::BSFixedString a_voice, LegacyVoice a_legacysetting, RE::TESSound* a_sound)
	{
		std::unique_lock lock{ _m };
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return;
		}
		if (v->fromfile) {
			logger::error("From File initialized Voice Objects are read only. {}", a_voice);
			return;
		}
		assert(!v->extrasets.empty());
		switch (a_legacysetting) {
		case LegacyVoice::Mild:
			v->defaultset.SetSound(true, a_sound);
			break;
		case LegacyVoice::Medium:
			v->extrasets.front().SetSound(true, a_sound);
			break;
		case LegacyVoice::Hot:
			v->defaultset.SetSound(false, a_sound);
			v->extrasets.front().SetSound(false, a_sound);
			break;
		}
	}

	void Voice::SetTags(RE::BSFixedString a_voice, const std::vector<RE::BSFixedString>& a_tags)
	{
		std::unique_lock lock{ _m };
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return;
		}
		if (v->fromfile) {
			logger::error("From File initialized Voice Objects are read only. {}", a_voice);
			return;
		}
		v->tags = a_tags;
	}

	void Voice::SetRace(RE::BSFixedString a_voice, const std::vector<RaceKey>& a_races)
	{
		std::unique_lock lock{ _m };
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return;
		}
		if (v->fromfile) {
			logger::error("From File initialized Voice Objects are read only. {}", a_voice);
			return;
		}
		if (!a_races.empty() && !std::ranges::contains(a_races, RaceKey::Human)) {
			v->tags.AddTag("Creature");
		} else {
			v->tags.RemoveTag("Creature");
		}
		v->races = a_races;
	}

	void Voice::SetSex(RE::BSFixedString a_voice, RE::SEXES::SEX a_sex)
	{
		std::unique_lock lock{ _m };
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return;
		}
		if (v->fromfile) {
			logger::error("From File initialized Voice Objects are read only. {}", a_voice);
			return;
		}
		switch (a_sex) {
		case RE::SEXES::kFemale:
			v->tags.RemoveTag("Male");
			v->tags.AddTag("Female");
			break;
		case RE::SEXES::kMale:
			v->tags.AddTag("Male");
			v->tags.RemoveTag("Female");
			break;
		default:
			v->tags.AddTag("Female");
			v->tags.AddTag("Male");
			break;
		}
		v->sex = a_sex;
	}

	void Voice::SaveYAML(RE::BSFixedString a_voice) const
	{
		auto v = std::ranges::find_if(voices, [&](auto& v) { return v.name == a_voice; });
		if (v == voices.end()) {
			logger::error("No such voice registered: {}", a_voice);
			return;
		}
		if (v->fromfile) {
			return;
		}
		auto path = fmt::format("{}\\{}.yaml", VOICEPATH, v->name);
		if (fs::exists(path)) {
			return;
		}
		auto yaml = v->AsYaml();
		std::ofstream fout{ path };
		fout << yaml;
	}

	void Voice::Initialize()
	{
		std::unique_lock lock{ _m };
		logger::info("Loading Voices");

		if (fs::exists(VOICEPATH)) {
			for (auto& file : fs::directory_iterator{ VOICEPATH }) {
				if (const auto ext = file.path().extension(); ext != ".yaml" && ext != ".yml")
					continue;
				const auto filename = file.path().filename().string();
				try {
					const auto root = YAML::LoadFile(file.path().string());
					auto v = VoiceObject{ root };
					if (auto w = std::ranges::find(voices, v); w != voices.end()) {
						*w = std::move(v);
					} else {
						voices.push_back(std::move(v));
					}
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
					const RE::BSFixedString str = it.first.as<std::string>();
					auto where = std::ranges::find(voices, str, [](auto& it) { return it.name; });
					if (where == voices.end())
						continue;

					where->enabled = it.second.as<bool>();
				}
			} catch (const std::exception& e) {
				logger::error("Error while loading voice settings: {}", e.what());
			}
		}
		logger::info("Loaded {} Voices", voices.size());

		if (fs::exists(VOICE_NPCPATH)) {
			try {
				const auto root = YAML::LoadFile(VOICE_NPCPATH);
				for (auto&& it : root) {
					const auto str = it.first.as<std::string>();
					auto id = FormFromString<RE::FormID>(str);
					if (id == 0)
						continue;
					auto v = it.second.as<std::string>();
					auto vobj = GetVoice(v);
					if (!vobj) {
						logger::error("Actor {} uses unknown Voice {}", id, v);
						continue;
					}
					saved_voices.insert_or_assign(id, vobj);
				}
			} catch (const std::exception& e) {
				logger::error("Error while loading npc voices: {}", e.what());
			}
		}
		logger::info("Loaded {} NPC Voices", saved_voices.size());
	}

	void Voice::Save()
	{
		std::shared_lock lock{ _m };
		{
			YAML::Node root{};
			if (fs::exists(VOICESETTINGPATH))
				try {
					root = YAML::LoadFile(VOICESETTINGPATH);
				} catch (const std::exception& e) {
					logger::error("Error while loading voice settings: {}. The file will be re-generated", e.what());
				}
			for (auto&& it : voices) {
				root[it.name.data()] = it.enabled;
			}
			std::ofstream fout(VOICESETTINGPATH);
			fout << root;
			logger::info("Finished saving voice settings");
		}
		{
			YAML::Node root{};
			if (fs::exists(VOICE_NPCPATH))
				try {
					root = YAML::LoadFile(VOICE_NPCPATH);
				} catch (const std::exception& e) {
					logger::error("Error while loading npc voices: {}. The file will be re-generated", e.what());
				}
			for (auto&& [id, voice] : saved_voices) {
				auto form = RE::TESForm::LookupByID<RE::Actor>(id);
				if (form) {
					if (auto base = form->GetActorBase()) {
						if (!base->IsUnique())
							continue;
					}
					auto str = FormToString(form);
					root[str] = voice->name.data();
				}
			}

			std::ofstream fout(VOICE_NPCPATH);
			fout << root;
			logger::info("Finished saving npc voices");
		}
	}

	Voice::VoiceObject::VoiceObject(const YAML::Node& a_node) :
		name(a_node["Name"].as<std::string>()),
		enabled(true),
		sex([&]() {
			auto node = a_node["Actor"]["Sex"];
			if (!node.IsDefined())
				return RE::SEXES::kNone;
			auto str = node.as<std::string>();
			ToLower(str);
			return str == "female" ? RE::SEXES::kFemale :
						 str == "male"	 ? RE::SEXES::kMale :
															 RE::SEXES::kNone;
		}()),
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
			auto arg = node.IsScalar() ? std::vector{ node.as<std::string>() } : node.as<std::vector<std::string>>();
			return { arg };
		}()),
		defaultset(a_node),
		orgasmvfx([&]() { auto&node = a_node["Orgasm"]; return node.IsDefined() ? FormFromString<RE::TESSound*>(node.as<std::string>()) : nullptr; }()),
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

	YAML::Node Voice::VoiceObject::AsYaml() const
	{
		YAML::Node ret{};
		ret["Name"] = name.data();
		switch (sex) {
		case RE::SEXES::kFemale:
			ret["Actor"]["Sex"] = "Female";
			break;
		case RE::SEXES::kMale:
			ret["Actor"]["Sex"] = "Male";
			break;
		default:
			ret["Actor"]["Sex"] = "Any";
			break;
		}
		if (races.empty()) {
			ret["Actor"]["Race"].push_back("Human");
		} else {
			for (auto&& r : races) {
				auto str = RaceHandler::AsString(r);
				ret["Actor"]["Race"].push_back(str.data());
			}
		}
		for (auto&& t : tags.AsVector()) {
			ret["Tags"].push_back(t.data());
		}
		ret["Voices"] = defaultset.AsYaml()["Voices"];
		if (orgasmvfx)
			ret["Orgasm"] = FormToString(orgasmvfx);
		for (auto&& e : extrasets) {
			ret["Extra"].push_back(e.AsYaml());
		}
		return ret;
	}

	VoiceSet::VoiceSet(const YAML::Node& a_node)
	{
		auto v = a_node["Voices"];
		if (v.IsMap()) {
			for (auto&& it : v) {
				auto sound = FormFromString<RE::TESSound*>(it.first.as<std::string>());
				data.emplace_back(sound, static_cast<uint8_t>(it.second.as<uint32_t>()));
			}
		} else {
			const auto max = static_cast<float>(v.size());
			for (float i = 0; i < max; i++)
			{
				auto sound = FormFromString<RE::TESSound*>(v[i].as<std::string>());
				if (!sound)
					continue;
				data.emplace_back(sound, static_cast<uint8_t>((i / max) * 100.0f));
			}
		}
		if (data.empty()) {
			throw std::exception("Need at least 1 Voice per Set");
		}
		std::sort(data.begin(), data.end(), [](auto& a, auto& b) {
			return a.second < b.second;
		});

		auto convec = a_node["Conditions"];
		if (!convec.IsDefined())
			return;

		for (auto&& con : convec) {
			auto key = con.first.as<std::string>();
			ToLower(key);
			if (key == "submissive") {
				CONDITION::Condition ct;
				ct._bool = con.second.as<bool>();
				conditions.emplace_back(CONDITION::ConditionType::Submissive, ct);
				continue;
			}
			auto contype =
				key == "context"	? CONDITION::ConditionType::Context :
				key == "position" ? CONDITION::ConditionType::PositionExtra :
														// key == "tags" ?
														CONDITION::ConditionType::Tag;
			if (con.second.IsScalar()) {
				auto str = con.second.as<std::string>().c_str();
				CONDITION::Condition ct;
				ct._bool = _strdup(str);
				conditions.emplace_back(contype, ct);
			} else {
				for (auto&& it : con.second) {
					auto str = it.as<std::string>().c_str();
					CONDITION::Condition ct;
					ct._bool = _strdup(str);
					conditions.emplace_back(contype, ct);
				}
			}
		}
	}

	VoiceSet::VoiceSet(bool a_aslegacyextra) :
		data({ { nullptr, uint8_t(0) }, { nullptr, uint8_t(75) } })
	{
		if (a_aslegacyextra) {
			CONDITION::Condition carg{};
			carg._string = "Aggressive";
			CONDITION c(CONDITION::ConditionType::Context, carg);
			conditions.push_back(std::move(c));
		}
	}

	bool VoiceSet::IsValid(const Stage* a_stage, const PositionInfo* a_position, const std::vector<RE::BSFixedString>& a_context) const
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
				logger::error("Unrecognized Condition Type {}", static_cast<std::underlying_type_t<decltype(c.type)>>(c.type));
				break;
			}
		}
		return true;
	}

	RE::TESSound* VoiceSet::Get(uint32_t a_priority) const
	{
		for (size_t i = data.size() - 1; i >= 0; i--) {
			auto& [voice, value] = data[i];
			if (value <= a_priority)
				return voice;
		}
		return nullptr;
	}

	RE::TESSound* VoiceSet::Get(LegacyVoice a_setting) const
	{
		switch (a_setting) {
		case LegacyVoice::Hot:
			return data.back().first;
		default:
			return data.front().first;
		}
	}

	void VoiceSet::SetSound(bool front, RE::TESSound* a_sound)
	{
		auto& obj = front ? data.front() : data.back();
		obj.first = a_sound;
	}

	YAML::Node VoiceSet::AsYaml() const
	{
		YAML::Node ret{};
		for (auto&& [v, prio] : data) {
			if (!v)
				continue;
			auto key = FormToString(v);
			if (ret["Voices"][key].IsDefined())
				continue;
			ret["Voices"][key] = static_cast<int32_t>(prio);
		}
		for (auto&& c : conditions) {
			switch (c.type) {
			case CONDITION::ConditionType::Tag:
				ret["Conditions"]["Tags"].push_back(c.condition._string);
				break;
			case CONDITION::ConditionType::Context:
				ret["Conditions"]["Context"].push_back(c.condition._string);
				break;
			case CONDITION::ConditionType::PositionExtra:
				ret["Conditions"]["Position"].push_back(c.condition._string);
				break;
			case CONDITION::ConditionType::Submissive:
				ret["Conditions"]["Submissive"].push_back(c.condition._bool);
				break;
			default:
				logger::error("Unrecognized Condition Type {}", static_cast<std::underlying_type_t<decltype(c.type)>>(c.type));
				break;
			}
		}
		return ret;
	}

}	 // namespace Registry
