#include "Animation.h"

#include "Registry/Define/RaceKey.h"
#include "Util/Combinatorics.h"
#include "Library.h"

namespace Registry
{
	AnimPackage::AnimPackage(const fs::path a_file)
	{
		std::ifstream stream(a_file, std::ios::binary);
		stream.unsetf(std::ios::skipws);
		stream.exceptions(std::fstream::eofbit);
		stream.exceptions(std::fstream::badbit);
		stream.exceptions(std::fstream::failbit);

		uint8_t version;
		stream.read(reinterpret_cast<char*>(&version), 1);
		switch (version) {
		case 1:
		case 2:
			{
				Decode::Read(stream, name);
				Decode::Read(stream, author);

				hash.resize(Decode::HASH_SIZE);
				stream.read(hash.data(), Decode::HASH_SIZE);

				uint64_t scene_count;
				Decode::Read(stream, scene_count);
				scenes.reserve(scene_count);
				for (size_t i = 0; i < scene_count; i++) {
					scenes.push_back(
						std::make_unique<Scene>(stream, hash, version));
				}
			}
			break;
		default:
			throw std::runtime_error(fmt::format("Invalid version: {}", version).c_str());
		}
	}

	Scene::Scene(std::ifstream& a_stream, std::string_view a_hash, uint8_t a_version) :
		hash(a_hash), enabled(true)
	{
		id.resize(Decode::ID_SIZE);
		a_stream.read(id.data(), Decode::ID_SIZE);
		Decode::Read(a_stream, name);
		// --- Position Infos
		uint64_t info_count;
		Decode::Read(a_stream, info_count);
		positions.reserve(info_count);
		for (size_t i = 0; i < info_count; i++) {
			positions.emplace_back(a_stream, a_version);
		}

		enum legacySex : char
		{
			Male = 'M',
			Female = 'F',
			Futa = 'H',
			Creature = 'C',
		};
		std::vector<std::vector<legacySex>> sexes{};
		sexes.reserve(positions.size());
		for (auto&& position : positions) {
			std::vector<legacySex> vec{};
			if (position.race == RaceKey::Human) {
				if (position.sex.all(Sex::Male))
					vec.push_back(legacySex::Male);
				if (position.sex.all(Sex::Female))
					vec.push_back(legacySex::Female);
				if (position.sex.all(Sex::Futa))
					vec.push_back(legacySex::Futa);
			} else {
				vec.push_back(legacySex::Creature);
			}
			if (vec.empty())
				throw std::runtime_error(fmt::format("Some position has no associated sex in scene: {}", id).c_str());
			sexes.push_back(vec);
		}
		Combinatorics::ForEachCombination(sexes, [&](auto& it) {
			std::vector<char> gender_tag{};
			for (auto&& sex : it) {
				gender_tag.push_back(*sex);
			}
			RE::BSFixedString gTag1{ std::string{ gender_tag.begin(), gender_tag.end() } };
			RE::BSFixedString gTag2{ std::string{ gender_tag.rbegin(), gender_tag.rend() } };
			tags.AddTag(gTag1);
			if (gTag2 != gTag1) {
				tags.AddTag(gTag2);
			}
			return Combinatorics::CResult::Next;
		});
		// --- Stages
		std::string startstage(Decode::ID_SIZE, 'X');
		a_stream.read(startstage.data(), Decode::ID_SIZE);
		uint64_t stage_count;
		Decode::Read(a_stream, stage_count);
		stages.reserve(stage_count);
		for (size_t i = 0; i < stage_count; i++) {
			const auto& stage = stages.emplace_back(
				std::make_unique<Stage>(a_stream, a_version));

			tags.AddTag(stage->tags);
			if (stage->id == startstage) {
				start_animation = stage.get();
			}
		}
		if (!start_animation) {
			throw std::runtime_error(fmt::format("Start animation {} is not found in scene {}", startstage, id).c_str());
		}
		// --- Graph
		uint64_t graph_vertices;
		Decode::Read(a_stream, graph_vertices);
		if (graph_vertices != stage_count) {
			throw std::runtime_error(fmt::format("Invalid graph vertex count; expected {} but got {}", stage_count, graph_vertices).c_str());
		}
		std::string vertexid(Decode::ID_SIZE, 'X');
		for (size_t i = 0; i < graph_vertices; i++) {
			a_stream.read(vertexid.data(), Decode::ID_SIZE);
			const auto vertex = GetStageByKey(vertexid.data());
			if (!vertex) {
				throw std::runtime_error(fmt::format("Invalid vertex: {} in scene: {}", vertexid, id).c_str());
			}
			std::vector<const Stage*> edges{};
			uint64_t edge_count;
			Decode::Read(a_stream, edge_count);
			std::string edgeid(Decode::ID_SIZE, 'X');
			for (size_t n = 0; n < edge_count; n++) {
				a_stream.read(edgeid.data(), Decode::ID_SIZE);
				const auto edge = GetStageByKey(edgeid.data());
				if (!edge) {
					throw std::runtime_error(fmt::format("Invalid edge: {} for vertex: {} in scene: {}", edgeid, vertexid, id).c_str());
				}
				edges.push_back(edge);
			}
			graph.insert(std::make_pair(vertex, edges));
		}		
		// --- Misc
		a_stream.read(reinterpret_cast<char*>(&furnitures.furnitures), 4);
		a_stream.read(reinterpret_cast<char*>(&furnitures.allowbed), 1);
		furnitures.offset = Coordinate(a_stream);
		a_stream.read(reinterpret_cast<char*>(&is_private), 1);
	}

	PositionInfo::PositionInfo(std::ifstream& a_stream, uint8_t a_version)
	{
		a_stream.read(reinterpret_cast<char*>(&race), 1);
		a_stream.read(reinterpret_cast<char*>(&sex), 1);
		Decode::Read(a_stream, scale);
		a_stream.read(reinterpret_cast<char*>(&extra), 1);

		if (a_version > 1) {
			uint64_t extra_custom;
			Decode::Read(a_stream, extra_custom);
			custom.reserve(extra_custom);
			for (size_t j = 0; j < extra_custom; j++) {
				RE::BSFixedString tag;
				Decode::Read(a_stream, tag);
				custom.push_back(tag);
			}
		} else {
			custom = {};
		}
	}

	Stage::Stage(std::ifstream& a_stream, uint8_t a_version)
	{
		id.resize(Decode::ID_SIZE);
		a_stream.read(id.data(), Decode::ID_SIZE);

		uint64_t position_count;
		Decode::Read(a_stream, position_count);
		positions.reserve(position_count);
		for (size_t i = 0; i < position_count; i++) {
			positions.emplace_back(a_stream, a_version);
		}

		Decode::Read(a_stream, fixedlength);
		Decode::Read(a_stream, navtext);
		tags = TagData{ a_stream };
	}

	Position::Position(std::ifstream& a_stream, uint8_t a_version) :
		event(Decode::Read<decltype(event)>(a_stream)),
		climax(Decode::Read<uint8_t>(a_stream) > 0),
		offset(Transform(a_stream)),
		strips(decltype(strips)::enum_type(Decode::Read<uint8_t>(a_stream))),
		schlong(a_version >= 3 ? Decode::Read<decltype(schlong)>(a_stream) : 0) {}

	void Position::Save(YAML::Node& a_node) const
	{
		auto transform = a_node["transform"];
		offset.Save(transform);
		if (schlong != 0) {
			a_node["schlong"] = static_cast<int32_t>(schlong);
		}
	}

	void Position::Load(const YAML::Node& a_node)
	{
		if (auto transform = a_node["transform"]; transform.IsDefined()) {
			offset.Load(transform);
		}
		if (auto schlongnode = a_node["schlong"]; schlongnode.IsDefined()) {
			schlong = static_cast<int8_t>(schlongnode.as<int32_t>());
		}
	}

	void Stage::Save(YAML::Node& a_node) const
	{
		bool skip = std::find_if(positions.begin(), positions.end(), [](auto& position) { 
			return position.offset.HasChanges(); 
		}) == positions.end();
		if (skip)
			return;

		for (size_t i = 0; i < positions.size(); i++) {
			auto node = a_node[i];
			positions[i].Save(node);
		}
	}

	void Stage::Load(const YAML::Node& a_node)
	{
		for (size_t i = 0; i < positions.size(); i++) {
			if (auto node = a_node[i]; node.IsDefined()) {
				positions[i].Load(node);
			}
		}
	}

	void Scene::Save(YAML::Node& a_node) const
	{
		a_node["enabled"] = this->enabled;
		for (auto&& stage : stages) {
			auto node = a_node[stage->id];
			stage->Save(node);
		}
	}

	void Scene::Load(const YAML::Node& a_node)
	{
		if (const auto enable = a_node["enabled"]; enable.IsDefined())
			this->enabled = enable.as<bool>();

		for (auto&& stage : stages) {
			if (auto node = a_node[stage->id]; node.IsDefined()) {
				stage->Load(node);
			}
		}
	}


	bool PositionInfo::CanFillPosition(RE::Actor* a_actor) const
	{
		auto fragment = stl::enumeration(MakeFragmentFromActor(a_actor, false));
		if (CanFillPosition(fragment.get(), false))
			return true;
		if (CanFillPosition(fragment.get(), true))
			return true;

		fragment.set(PositionFragment::Submissive);
		return CanFillPosition(fragment.get(), false) || CanFillPosition(fragment.get(), true);
	}

	bool PositionInfo::CanFillPosition(PositionFragment a_fragment, bool a_skipdead) const
	{
		const auto fragment = stl::enumeration(a_fragment);
		if (fragment.all(PositionFragment::Futa)) {
			if (!this->sex.all(Sex::Futa))
				return false;
		} else {
			if (fragment.all(PositionFragment::Male) && !this->sex.all(Sex::Male))
				return false;
			if (fragment.all(PositionFragment::Female) && !this->sex.all(Sex::Female))
				return false;
		}

		if (!a_skipdead && fragment.all(PositionFragment::Unconscious) != this->extra.all(Extra::Unconscious))
			return false;
		if (fragment.all(PositionFragment::Submissive) != this->extra.all(Extra::Submissive))
			return false;

		if (fragment.all(PositionFragment::Human)) {
			if (this->extra.all(Extra::Vamprie) && !fragment.all(PositionFragment::Vampire))
				return false;
		} else {
			const auto race_frag = RaceKeyAsFragment(race);
			if (!fragment.all(race_frag)) {
				if (race == RaceKey::Canine) {
					if (!fragment.all(RaceKeyAsFragment(RaceKey::Dog)) && 
						!fragment.all(RaceKeyAsFragment(RaceKey::Wolf)) && 
						!fragment.all(RaceKeyAsFragment(RaceKey::Fox)))
						return false;
				}
				else if (race == RaceKey::Boar) {
					if (!fragment.all(RaceKeyAsFragment(RaceKey::BoarMounted)) &&
						!fragment.all(RaceKeyAsFragment(RaceKey::BoarSingle)))
						return false;
				}
				else {
					return false;
				}
			}
		}
		return true;
	}

	bool PositionInfo::CanFillPosition(const PositionInfo& a_other) const
	{
		return race == a_other.race && sex.any(a_other.sex.get()) && extra.all(a_other.extra.get());
	}

	std::vector<PositionFragment> PositionInfo::MakeFragments() const
	{
		std::vector<stl::enumeration<PositionFragment>> fragments{};
		const auto addVariance = [&](PositionFragment a_variancebit) {
			const auto count = fragments.size();
			for (size_t i = 0; i < count; i++) {
				auto copy = fragments[i];
				copy.set(a_variancebit);
				fragments.push_back(copy);
			}
		};
		const auto setFragmentBit = [&](PositionFragment a_bit) {
			for (auto&& fragment : fragments) {
				fragment.set(a_bit);
			}
		};
		const auto addRaceVariance = [&](RaceKey a_racekey) {
			const auto val = RaceKeyAsFragment(a_racekey);
			const auto count = fragments.size();
			for (size_t i = 0; i < count; i++) {
				auto copy = fragments[i];
				copy.reset(PositionFragment::CrtBit0,
					PositionFragment::CrtBit1,
					PositionFragment::CrtBit2,
					PositionFragment::CrtBit3,
					PositionFragment::CrtBit4,
					PositionFragment::CrtBit5);
				copy.set(val);
				fragments.push_back(copy);
			}
		};
		const auto setRaceBit = [&](RaceKey a_racekey) {
			const auto val = RaceKeyAsFragment(a_racekey);
			setFragmentBit(val);
		};

		if (this->sex.all(Sex::Male))
			fragments.emplace_back(PositionFragment::Male);
		if (this->sex.all(Sex::Female))
			fragments.emplace_back(PositionFragment::Female);
		if (this->sex.all(Sex::Futa))
			fragments.emplace_back(PositionFragment::Futa);
		if (this->extra.all(Extra::Unconscious))
			setFragmentBit(PositionFragment::Unconscious);
		if (this->extra.all(Extra::Submissive))
			setFragmentBit(PositionFragment::Submissive);

		switch (this->race) {
		case RaceKey::Human:
			{
				setFragmentBit(PositionFragment::Human);
				if (this->extra.all(Extra::Vamprie)) {
					setFragmentBit(PositionFragment::Vampire);
				} else {
					addVariance(PositionFragment::Vampire);
				}
			}
			break;
		case RaceKey::Boar:
			setRaceBit(RaceKey::BoarMounted);
			addRaceVariance(RaceKey::BoarSingle);
			break;
		case RaceKey::Canine:
			setRaceBit(RaceKey::Dog);
			addRaceVariance(RaceKey::Wolf);
			break;
		default:
			setRaceBit(this->race);
			break;
		}
		std::vector<PositionFragment> ret{};
		for (auto&& it : fragments)
			ret.push_back(it.get());
		return ret;
	}

	bool PositionInfo::HasExtraCstm(const RE::BSFixedString& a_extra) const
	{
		return std::ranges::find(custom, a_extra) != custom.end();
	}

	std::string PositionInfo::ConcatExtraCstm() const
	{
		std::string ret{};
		for (auto&& it : custom) {
			ret += it.c_str();
		}
		return ret;
	}

	PapyrusSex PositionInfo::GetSexPapyrus() const
	{
		if (race == Registry::RaceKey::Human) {
			return PapyrusSex(sex.underlying());
		} else {
			stl::enumeration<PapyrusSex> ret{ PapyrusSex::None };
			if (sex.all(Registry::Sex::Male))
				ret.set(PapyrusSex::CrtMale);
			if (sex.all(Registry::Sex::Female))
				ret.set(PapyrusSex::CrtFemale);

			return ret == PapyrusSex::None ? ret.get() : PapyrusSex::CrtMale;
		}
	}

	stl::enumeration<FurnitureType> Scene::FurnitureData::GetCompatibleFurnitures() const
	{
		auto ret = furnitures;
		if (allowbed) {
			ret.set(
				FurnitureType::BedDouble,
				FurnitureType::BedSingle,
				FurnitureType::BedRoll);
		}
		return ret;
	}

	Stage* Scene::GetStageByKey(const RE::BSFixedString& a_key)
	{
		if (a_key.empty()) {
			return start_animation;
		}

		const auto where = std::find_if(stages.begin(), stages.end(), [&](const std::unique_ptr<Stage>& it) { return a_key == it->id.data(); });
		return where == stages.end() ? nullptr : where->get();
	}

	const Stage* Scene::GetStageByKey(const RE::BSFixedString& a_key) const
	{
		if (a_key.empty()) {
			return start_animation;
		}

		const auto where = std::find_if(stages.begin(), stages.end(), [&](const std::unique_ptr<Stage>& it) { return a_key == it->id.data(); });
		return where == stages.end() ? nullptr : where->get();
	}

	bool Scene::HasCreatures() const
	{
		for (auto&& info : positions) {
			if (!info.IsHuman())
				return true;
		}
		return false;
	}

	uint32_t Scene::CountSubmissives() const
	{
		uint32_t ret = 0;
		for (auto&& info : positions) {
			if (info.IsSubmissive()) {
				ret++;
			}
		}
		return ret;
	}
	
	const PositionInfo* Scene::GetNthPosition(size_t n) const
	{
		return &positions[n];
	}

	uint32_t Scene::CountPositions() const
	{
		return static_cast<uint32_t>(positions.size());
	}

	bool Scene::IsEnabled() const
	{
		return enabled;
	}

	bool Scene::IsPrivate() const
	{
		return is_private;
	}

	std::vector<std::vector<PositionFragment>> Scene::MakeFragments() const
	{
		std::vector<std::vector<PositionFragment>> ret;
		ret.reserve(this->positions.size());
		for (auto&& pinfo : this->positions) {
			ret.push_back(pinfo.MakeFragments());
		}
		return ret;
	}

	bool Scene::IsCompatibleTags(const TagData& a_tags) const
	{
		return this->tags.HasTags(a_tags, true);
	}
	bool Scene::IsCompatibleTags(const TagDetails& a_details) const
	{
		return a_details.MatchTags(tags);
	}

	bool Scene::UsesFurniture() const
	{
		return this->furnitures.furnitures != FurnitureType::None;
	}

	bool Scene::IsCompatibleFurniture(const RE::TESObjectREFR* a_reference) const
	{
		const auto details = Library::GetSingleton()->GetFurnitureDetails(a_reference);
		return IsCompatibleFurniture(details);
	}

	bool Scene::IsCompatibleFurniture(const FurnitureDetails* a_details) const
	{
		if (!a_details) {
			return !UsesFurniture();
		}
		const auto types = furnitures.GetCompatibleFurnitures();
		return a_details->HasType(types.get());
	}

	bool Scene::IsCompatibleFurniture(FurnitureType a_furniture) const
	{
		if (a_furniture == FurnitureType::None) {
			return !UsesFurniture();
		}
		const auto types = furnitures.GetCompatibleFurnitures();
		return types.any(a_furniture);
	}

	bool Scene::Legacy_IsCompatibleSexCount(int32_t a_males, int32_t a_females) const
	{
		if (a_males < 0 && a_females < 0) {
			return true;
		}

		bool ret = false;
		tags.ForEachExtra([&](const std::string_view a_tag) {
			if (a_tag.find_first_not_of("MFC") != std::string_view::npos) {
				return false;
			}
			if (a_males == -1 || std::count(a_tag.begin(), a_tag.end(), 'M') == a_males) {
				if (a_females == -1 || std::count(a_tag.begin(), a_tag.end(), 'F') == a_females) {
					ret = true;
					return true;
				}
			}
			return false;
		});
		return ret;
	}

	bool Scene::Legacy_IsCompatibleSexCountCrt(int32_t a_males, int32_t a_females) const
	{
		enum
		{
			Male = 0,
			Female = 1,
			Either = 2,
		};

		bool ret = false;
		tags.ForEachExtra([&](const std::string_view a_tag) {
			if (a_tag.find_first_not_of("MFC") != std::string_view::npos) {
				return false;
			}
			const auto crt_total = std::count(a_tag.begin(), a_tag.end(), 'C');
			if (crt_total != a_males + a_females) {
				return true;
			}

			std::vector<int> count;
			for (auto&& position : positions) {
				if (position.race == Registry::RaceKey::Human)
					continue;
				if (position.sex.none(Registry::Sex::Female)) {
					count[Male]++;
				} else if (position.sex.none(Registry::Sex::Male)) {
					count[Female]++;
				} else {
					count[Either]++;
				}
			}
			if (count[Male] <= a_males && count[Male] + count[Either] >= a_males) {
				count[Either] -= a_males - count[Male];
				ret = count[Female] + count[Either] == a_females;
			}
			return true;
		});
		return ret;
	}

	std::optional<std::vector<RE::Actor*>> Scene::SortActors(const std::vector<std::pair<RE::Actor*, PositionFragment>>& a_positions, bool a_skipdead) const
	{
		if (a_positions.size() != this->positions.size())
			return std::nullopt;

		std::vector<std::vector<std::pair<size_t, RE::Actor*>>> compatibles{};
		compatibles.resize(a_positions.size());
		for (size_t i = 0; i < a_positions.size(); i++) {
			for (size_t n = 0; n < this->positions.size(); n++) {
				if (this->positions[n].CanFillPosition(a_positions[i].second, a_skipdead)) {
					compatibles[i].emplace_back(n, a_positions[i].first);
				}
			}
			if (compatibles[i].empty()) {
				logger::info("Actor {:X} has no compatible positions for scene {} ({})", a_positions[i].first->formID, this->name, this->id);
				return std::nullopt;
			}
		}
		std::vector<RE::Actor*> ret;
		Combinatorics::ForEachCombination(compatibles, [&](auto it) {
			std::vector<RE::Actor*> result(it.size(), nullptr);
			for (auto&& current : it) {
				const auto& [scene_idx, actor] = *current;
				if (result[scene_idx] != nullptr) {
					return Combinatorics::CResult::Next;
				}
				result[scene_idx] = actor;
			}
			assert(!std::ranges::contains(result, nullptr));
			ret = result;
			return Combinatorics::CResult::Stop;
		});
		if (ret.empty()) {
			return std::nullopt;
		}
		return ret;
	}

	std::optional<std::vector<RE::Actor*>> Scene::SortActorsFallback(std::vector<std::pair<RE::Actor*, PositionFragment>> a_positions) const
	{
		if (auto ret = SortActors(a_positions))
			return ret;
		if (auto ret = SortActors(a_positions, true))
			return ret;

		for (auto&& [actor, fragment] : a_positions) {
			auto e = stl::enumeration(fragment);
			if (e.all(PositionFragment::Human, PositionFragment::Female) && e.none(PositionFragment::Male)) {
				e.reset(PositionFragment::Female);
				e.set(PositionFragment::Male);
			}
			fragment = e.get();
			if (auto ret = SortActors(a_positions))
				return ret;
			if (auto ret = SortActors(a_positions, true))
				return ret;
		}
		return std::nullopt;
	}

	size_t Scene::GetNumLinkedStages(const Stage* a_stage) const
	{
		const auto where = graph.find(a_stage);
		if (where == graph.end())
			return 0;
		
		return where->second.size();
	}

	const Stage* Scene::GetNthLinkedStage(const Stage* a_stage, size_t n) const
	{
		const auto where = graph.find(a_stage);
		if (where == graph.end())
			return 0;

		if (n < 0 || n >= where->second.size())
			return 0;
		
		return where->second[n];
	}

	RE::BSFixedString Scene::GetNthAnimationEvent(const Stage* a_stage, size_t n) const
	{
		std::string ret{ hash };
		return ret + a_stage->positions[n].event.data();
	}

	std::vector<RE::BSFixedString> Scene::GetAnimationEvents(const Stage* a_stage) const
	{
		std::vector<RE::BSFixedString> ret{};
		ret.reserve(a_stage->positions.size());
		for (auto&& position : a_stage->positions) {
			std::string event{ hash };
			ret.push_back(event + position.event.data());
		}
		return ret;
	}

	size_t Scene::GetNumStages() const
	{
		return stages.size();
	}

	const std::vector<const Stage*> Scene::GetAllStages() const
	{
		std::vector<const Stage*> ret{};
		ret.reserve(stages.size());
		for (auto&& stage : stages) {
			ret.push_back(stage.get());
		}
		return ret;
	}

	Scene::NodeType Scene::GetStageNodeType(const Stage* a_stage) const
	{
		if (a_stage == start_animation)
			return NodeType::Root;
		
		const auto where = graph.find(a_stage);
		if (where == graph.end())
			return NodeType::None;
		
		return where->second.size() == 0 ? NodeType::Sink : NodeType::Default;
	}

	std::vector<const Stage*> Scene::GetLongestPath(const Stage* a_src) const
	{
		if (GetStageNodeType(a_src) == NodeType::Sink)
			return { a_src };

		std::set<const Stage*> visited{};
		std::function<std::vector<const Stage*>(const Stage*)> DFS = [&](const Stage* src) -> std::vector<const Stage*> {
			if (visited.contains(src))
				return {};
			visited.insert(src);

			std::vector<const Stage*> longest_path{ src };
			const auto& neighbours = this->graph.find(src);
			assert(neighbours != this->graph.end());
			for (auto&& n : neighbours->second) {
				const auto cmp = DFS(n);
				if (cmp.size() + 1 > longest_path.size()) {
					longest_path.assign(cmp.begin(), cmp.end());
					longest_path.insert(longest_path.begin(), src);
				}
			}
			return longest_path;
		};
		return DFS(a_src);
	}

	std::vector<const Stage*> Scene::GetShortestPath(const Stage* a_src) const
	{
		if (GetStageNodeType(a_src) == NodeType::Sink)
			return { a_src };

		std::function<std::vector<const Stage*>(const Stage*)> BFS = [&](const Stage* src) -> std::vector<const Stage*> {
			std::set<const Stage*> visited{ src };
			std::map<const Stage*, const Stage*> pred{ { src, nullptr } };
			std::queue<const Stage*> queue{ { src } };
			while (!queue.empty()) {
				const auto it = queue.front();
				const auto neighbours = graph.find(it);
				assert(neighbours != this->graph.end());
				for (auto&& n : neighbours->second) {
					if (visited.contains(n))
						continue;
					if (GetStageNodeType(n) == NodeType::Sink) {
						std::vector<const Stage*> ret{};
						auto p = pred.at(it);
						while (p != nullptr) {
							ret.push_back(p);
							p = pred.at(p);
						}
						return { ret.rbegin(), ret.rend() };
					}
					pred.emplace(n, it);
					visited.insert(n);
					queue.push(n);
				}
				queue.pop();
			}
			return { src };
		};
		return BFS(a_src);
	}

	void Scene::ForEachStage(std::function<bool(Stage*)> a_visitor)
	{
		for (auto&& stage : stages) {
			if (a_visitor(stage.get())) {
				return;
			}
		}
	}

	std::vector<const Stage*> Scene::GetEndingStages() const
	{
		std::vector<const Stage*> ret{};
		for (auto&& [vert, edges] : graph) {
			if (edges.empty()) {
				ret.push_back(vert);
			}
		}
		return ret;
	}

	std::vector<const Stage*> Scene::GetClimaxStages() const
	{
		std::vector<const Stage*> ret{};
		for (auto&& stage : stages) {
			for (auto&& position : stage->positions) {
				if (position.climax) {
					ret.push_back(stage.get());
					break;
				}
			}
		}
		return ret;
	}

	std::vector<const Stage*> Scene::GetFixedLengthStages() const
	{
		std::vector<const Stage*> ret{};
		for (auto&& stage : stages) {
			if (stage->fixedlength)
				ret.push_back(stage.get());
		}
		return ret;
	}

}	 // namespace Registry
