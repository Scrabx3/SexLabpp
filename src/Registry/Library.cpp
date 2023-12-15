#include "Library.h"

#include "Define/RaceKey.h"
#include "Util/Combinatorics.h"

namespace Registry
{
	void Library::Initialize() noexcept
	{
		logger::info("Loading files..");
		const auto t1 = std::chrono::high_resolution_clock::now();

		const auto scenepath = fs::path{ CONFIGPATH("Registry") };
		std::error_code ec{};
		if (!fs::exists(scenepath, ec) || fs::is_empty(scenepath, ec)) {
			const auto msg = ec ? fmt::format("An error occured while initializing SexLab animations: {}", ec.message()) :
														fmt::format("Unable to load SexLab animations. Folder {} is empty or does not exist.", scenepath.string());
			logger::critical("{}", msg);
			if (SKSE::WinAPI::MessageBox(nullptr, fmt::format("{}\n\nExit game now?", msg).c_str(), "SexLab p+ Registry", 0x00000004) == 6)
				std::_Exit(EXIT_FAILURE);
			return;
		}

		std::vector<std::thread> threads;
		for (auto& file : fs::directory_iterator{ scenepath }) {
			if (file.path().extension() != ".slr") {
				continue;
			}

			threads.emplace_back([this, file]() {
				try {
					auto package = std::make_unique<AnimPackage>(file);
					for (auto&& scene : package->scenes) {
						// For each scene, find all viable hash locks and sort them into the library
						const auto positionFragments = scene->MakeFragments();
						Combinatorics::ForEachCombination<PositionFragment>(positionFragments, [&](const std::vector<std::vector<PositionFragment>::const_iterator>& it) {
							// Create a copy of the current scene
							std::vector<PositionFragment> argFragment;
							argFragment.reserve(it.size());
							for (const auto& current : it) {
								if (*current == PositionFragment::None) {
									continue;
								}
								argFragment.push_back(*current);
							}
							std::stable_sort(argFragment.begin(), argFragment.end());
							auto key = CombineFragments(argFragment);
							const std::unique_lock lock{ read_write_lock };
							const auto where = scenes.find(key);
							if (where == scenes.end()) {
								scenes[key] = { scene.get() };
							} else {
								// A scene containing similar positions may create identical hashes in different iterations
								auto& vec = where->second;
								if (std::find(vec.begin(), vec.end(), scene.get()) == vec.end()) {
									vec.push_back(scene.get());
								}
							}
							return Combinatorics::CResult::Next;
						});
					}
					const std::unique_lock lock{ read_write_lock };
					for (auto&& scene : package->scenes) {
						scene_map.insert({ scene->id, scene.get() });
					}
					packages.push_back(std::move(package));
				} catch (const std::exception& e) {
					const auto filename = file.path().filename().string();
					logger::critical("Unable to read registry file {}. The animation pack will NOT be added to the library. | Error: {}", filename, e.what());
				}
			});
		}
		for (auto& thread : threads) {
			thread.join();
		}
		const auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> ms_double = t2 - t1;
		logger::info("Loaded {} Packages ({} scenes | {} categories) in {}ms", packages.size(), GetSceneCount(), scenes.size(), ms_double.count());

		const auto furniturepath = fs::path{ CONFIGPATH("Furniture") };
		if (!fs::exists(scenepath, ec) || fs::is_empty(scenepath, ec)) {
			const auto msg = ec ? fmt::format("An error occured while attempting to read furniture info: {}", ec.message()) :
														fmt::format("Unable to load furnitures. Folder {} is empty or does not exist.", scenepath.string());
			logger::critical("{}", msg);
		} else {
			const std::unique_lock lock{ read_write_lock };
			for (auto& file : fs::directory_iterator{ scenepath }) {
				if (auto ext = file.path().extension(); ext != ".yml" && ext != ".yaml") {
					continue;
				}
				const auto filename = file.path().filename().string();
				try {
					YAML::Node root = YAML::LoadFile(file.path().string());
					for (auto&& it : root) {
						furnitures.emplace(
							RE::BSFixedString(it.first.as<std::string>()),
							std::make_unique<FurnitureDetails>(it.second));
					}
					logger::info("Finished parsing file {}", filename);
				} catch (const std::exception& e) {
					logger::error("Unable to load furnitures in file {}; Error: {}", filename, e.what());
				}
			}
		}
		const auto t3 = std::chrono::high_resolution_clock::now();
		ms_double = t3 - t2;
		logger::info("Loaded {} Furnitures in {}ms", packages.size(), GetSceneCount(), scenes.size(), ms_double.count());
		logger::info("Initialized Data");
	}

	const Scene* Library::GetSceneByID(const RE::BSFixedString& a_id) const
	{
		const auto where = scene_map.find(a_id);
		return where != scene_map.end() ? where->second : nullptr;
	}

	Scene* Library::GetSceneByID_Mutable(const RE::BSFixedString& a_id) const
	{
		std::string id{ a_id.data() };
		Registry::ToLower(id);
		const auto where = scene_map.find(id);
		return where != scene_map.end() ? where->second : nullptr;
	}

	std::vector<Scene*> Library::LookupScenes(std::vector<RE::Actor*>& a_actors, const std::vector<std::string_view>& a_tags, const std::vector<RE::Actor*>& a_submissives) const
	{
		const auto t1 = std::chrono::high_resolution_clock::now();
		FragmentHash hash;
		std::thread _hashbuilder{ [&]() {
			std::vector<PositionFragment> fragments;
			for (auto&& position : a_actors) {
				const auto submissive = std::find(a_submissives.begin(), a_submissives.end(), position) != a_submissives.end();
				const auto fragment = MakeFragmentFromActor(position, submissive);
				fragments.push_back(fragment);
			}
			std::stable_sort(fragments.begin(), fragments.end());
			hash = CombineFragments(fragments);
		} };	// Thread this because building details can be quite expensive
		TagDetails tags{ a_tags };
		_hashbuilder.join();

		const std::shared_lock lock{ read_write_lock };
		const auto where = this->scenes.find(hash);
		if (where == this->scenes.end()) {
			logger::info("Invalid query: [{} | {} <{}>]; No animations for given actors", a_actors.size(), fmt::join(a_tags, ", "sv), a_tags.size());
			return {};
		}
		const auto& rawScenes = where->second;

		std::vector<Scene*> ret;
		ret.reserve(rawScenes.size() / 2);
		std::copy_if(rawScenes.begin(), rawScenes.end(), std::back_inserter(ret), [&](Scene* a_scene) {
			return a_scene->IsEnabled() && !a_scene->IsPrivate() && a_scene->IsCompatibleTags(tags);
		});
		if (ret.empty()) {
			logger::info("Invalid query: [{} | {} <{}>]; 0/{} animations use given tags", a_actors.size(), fmt::join(a_tags, ", "sv), a_tags.size(), where->second.size());
			return {};
		}
		const auto t2 = std::chrono::high_resolution_clock::now();
		// auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::chrono::duration<double, std::milli> ms_double = t2 - t1;
		logger::info("Found {} scenes for query [{} | {} <{}>] actors in {}ms", ret.size(), a_actors.size(), fmt::join(a_tags, ", "sv), a_tags.size(), ms_double.count());
		return ret;
	}

	size_t Library::GetSceneCount() const
	{
		const std::shared_lock lock{ read_write_lock };
		size_t ret = 0;
		for (auto&& package : packages) {
			ret += package->scenes.size();
		}
		return ret;
	}

	std::vector<Scene*> Library::GetByTags(int32_t a_positions, const std::vector<std::string_view>& a_tags) const
	{
		TagDetails tags{ a_tags };
		std::vector<Scene*> ret{};
		ret.reserve(scene_map.size() >> 5);
		const std::shared_lock lock{ read_write_lock };
		for (auto&& [key, scene] : scene_map) {
			if (!scene->IsEnabled() || scene->IsPrivate())
				continue;
			if (scene->positions.size() != a_positions)
				continue;
			if (!scene->IsCompatibleTags(tags))
				continue;
			ret.push_back(scene);
		}
		return ret;
	}

	void Library::ForEachScene(std::function<bool(const Scene*)> a_visitor) const
	{
		std::shared_lock lock{ read_write_lock };
		for (auto&& [key, scene] : scene_map) {
			if (a_visitor(scene))
				break;
		}
	}

	void Library::Save()
	{
		std::shared_lock lock{ read_write_lock };
		std::vector<std::thread> threads{};
		for (auto&& p : packages) {
			threads.emplace_back([&]() {
				YAML::Node data{};
				for (auto&& scene : p->scenes) {
					auto node = data[scene->id];
					scene->Save(node);
				}
				const auto path = fmt::format("Registry\\UserData\\Scenes\\{}_{}.yaml", p->GetName(), p->GetHash());
				std::ofstream fout(CONFIGPATH(path));
				fout << data;
			});
		}
		for (auto&& thread : threads) {
			thread.join();
		}
		logger::info("Finished saving registry settings");
	}

	void Library::Load()
	{
		const auto path = fs::path{ CONFIGPATH("Registry\\UserData\\Scenes") };
		if (!fs::exists(path))
			return;

		std::unique_lock lock{ read_write_lock };
		for (auto& file : fs::directory_iterator{ path }) {
			if (const auto ext = file.path().extension(); ext != ".yaml" && ext != ".yml")
				continue;
			const auto filename = file.path().filename().string();
			try {
				const auto root = YAML::LoadFile(file.path().string());
				for (auto&& [key, scene] : scene_map) {
					const auto node = root[scene->id];
					if (!node.IsDefined())
						continue;
					scene->Load(node);
				}
				logger::info("Loaded scene settings from file {}", filename);
			} catch (const std::exception& e) {
				logger::error("Error while loading scene settings from file {}: {}", filename, e.what());
			}
		}
		logger::info("Finished loading registry settings");
	}
	
	const FurnitureDetails* Library::GetFurnitureDetails(const RE::TESObjectREFR* a_ref) const
	{
		if (a_ref->Is(RE::FormType::ActorCharacter)) {
			return nullptr;
		}
		const auto ref = a_ref->GetObjectReference();
		if (const auto tesmodel = ref ? ref->As<RE::TESModel>() : nullptr) {
			std::shared_lock lock{ read_write_lock };
			const auto where = furnitures.find(tesmodel->model);
			if (where != furnitures.end()) {
				return where->second.get();
			}
		}
		switch (BedHandler::GetBedType(a_ref)) {
		case FurnitureType::BedSingle:
			return &offset_bedsingle;
		case FurnitureType::BedDouble:
			return &offset_beddouble;
		case FurnitureType::BedRoll:
			return &offset_bedroll;
		}
		return nullptr;
	}

}
