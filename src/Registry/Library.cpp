#include "Library.h"

#include "Define/RaceKey.h"
#include "Util/Combinatorics.h"

namespace Registry
{
	void Library::Initialize() noexcept
	{
		logger::info("Loading files..");
		const auto t1 = std::chrono::high_resolution_clock::now();

		std::error_code ec{};
		if (!fs::exists(SCENEPATH, ec) || fs::is_empty(SCENEPATH, ec)) {
			const auto msg = ec ? std::format("An error occured while initializing SexLab animations: {}", ec.message()) :
														std::format("Unable to load SexLab animations. Folder {} is empty or does not exist.", SCENEPATH);
			logger::critical("{}", msg);
			const auto msgBox = std::format("{}\n\nExit game now?", msg);
			if (MESSAGEBOX(nullptr, msgBox.c_str(), "SexLab p+ Registry", 0x00000004) == 6)
				std::_Exit(EXIT_FAILURE);
			return;
		}
		std::vector<std::thread> threads{};
		for (auto& file : fs::directory_iterator{ SCENEPATH }) {
			if (file.path().extension() != ".slr") {
				continue;
			}
#ifndef SKYRIMVR
			threads.emplace_back([this, file]() {
#endif
				try {
					auto package = std::make_unique<AnimPackage>(file);
					for (auto&& scene : package->scenes) {
						const auto positionFragments = scene->MakeFragments();
						Combinatorics::ForEachCombination<PositionFragment>(positionFragments, [&](const std::vector<std::vector<PositionFragment>::const_iterator>& it) {
							std::vector<PositionFragment> argFragment{};
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
							} else if (!std::ranges::contains(where->second, scene.get())) {
								where->second.push_back(scene.get());
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
#ifndef SKYRIMVR
			});
#endif
		}
		for (auto& thread : threads) {
			thread.join();
		}
		const auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> ms = t2 - t1;
		logger::info("Loaded {} Packages ({} scenes | {} categories) in {}ms", packages.size(), GetSceneCount(), scenes.size(), ms.count());

		if (!fs::exists(FURNITUREPATH, ec) || fs::is_empty(FURNITUREPATH, ec)) {
			const auto msg = ec ? std::format("An error occured while attempting to read furniture info: {}", ec.message()) :
														std::format("Unable to load furnitures. Folder {} is empty or does not exist.", FURNITUREPATH);
			logger::critical("{}", msg);
		} else {
			const std::unique_lock lock{ read_write_lock };
			for (auto& file : fs::directory_iterator{ FURNITUREPATH }) {
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
			const auto t3 = std::chrono::high_resolution_clock::now();
			ms = t3 - t2;
			logger::info("Loaded {} Furnitures in {}ms", furnitures.size(), ms.count());
		}
		logger::info("Initialized Data");
	}

	const Scene* Library::GetSceneByID(const RE::BSFixedString& a_id) const
	{
		const auto where = scene_map.find(a_id);
		return where != scene_map.end() ? where->second : nullptr;
	}

	Scene* Library::GetSceneByID(const RE::BSFixedString& a_id)
	{
		const auto where = scene_map.find(a_id);
		return where != scene_map.end() ? where->second : nullptr;
	}

	const Scene* Library::GetSceneByName(const RE::BSFixedString& a_name) const
	{
		for (auto&& package : packages) {
			for (auto&& scene : package->scenes) {
				if (a_name == RE::BSFixedString(scene->name))
					return scene.get();
			}
		}
		return nullptr;
	}

	Scene* Library::GetSceneByName(const RE::BSFixedString& a_name)
	{
		for (auto&& package : packages) {
			for (auto&& scene : package->scenes) {
				if (a_name == RE::BSFixedString(scene->name))
					return scene.get();
			}
		}
		return nullptr;
	}

	std::vector<Scene*> Library::LookupScenes(std::vector<RE::Actor*>& a_actors, const std::vector<std::string_view>& a_tags, const std::vector<RE::Actor*>& a_submissives) const
	{
		const auto t1 = std::chrono::high_resolution_clock::now();
		FragmentHash hash;
		std::thread _hashbuilder{ [&]() {
			std::vector<PositionFragment> fragments;
			for (auto&& position : a_actors) {
				const auto submissive = std::ranges::contains(a_submissives, position);
				const auto fragment = MakeFragmentFromActor(position, submissive);
				fragments.push_back(fragment);
			}
			std::stable_sort(fragments.begin(), fragments.end());
			hash = CombineFragments(fragments);
		} };
		TagDetails tags{ a_tags };
		const auto tagstr = a_tags.empty() ? "[]"s : std::format("[{}]", [&] {
			return std::accumulate(std::next(a_tags.begin()), a_tags.end(), std::string(a_tags[0]), [](std::string a, std::string_view b) {
				return std::move(a) + ", " + b.data();
			});
		}());
		_hashbuilder.join();

		const std::shared_lock lock{ read_write_lock };
		const auto where = this->scenes.find(hash);
		if (where == this->scenes.end()) {
			logger::info("Invalid query: [{} | {} | {}]; No animations for given actors", a_actors.size(), hash.to_string(), tagstr);
			return {};
		}
		const auto& rawScenes = where->second;

		std::vector<Scene*> ret{};
		ret.reserve(rawScenes.size() / 2);
		std::copy_if(rawScenes.begin(), rawScenes.end(), std::back_inserter(ret), [&](Scene* a_scene) {
			return a_scene->IsEnabled() && !a_scene->IsPrivate() && a_scene->IsCompatibleTags(tags);
		});
		if (ret.empty()) {
			logger::info("Invalid query: [{} | {} | {}]; 0/{} animations use requested tags", a_actors.size(), hash.to_string(), tagstr, where->second.size());
			return {};
		}
		const auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> ms = t2 - t1;
		logger::info("Found {} scenes for query [{} | {} | {}] actors in {}ms", ret.size(), a_actors.size(), hash.to_string(), tagstr, ms.count());
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

	void Library::ForEachPackage(std::function<bool(const AnimPackage*)> a_visitor) const
	{
		std::shared_lock lock{ read_write_lock };
		for (auto&& package : packages) {
			if (a_visitor(package.get()))
				break;
		}
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
				const auto filepath = std::format("{}\\{}_{}.yaml", SCENESETTINGPATH, p->GetName().data(), p->GetHash());
				std::ofstream fout(filepath);
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
		if (!fs::exists(SCENESETTINGPATH))
			return;

		std::unique_lock lock{ read_write_lock };
		for (auto& file : fs::directory_iterator{ SCENESETTINGPATH }) {
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
