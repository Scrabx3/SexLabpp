#include "Library.h"

#include "Decode.h"
#include "Define/RaceKey.h"

namespace Registry
{
  void Library::Initialize()
  {
		logger::info("Loading files..");
		const auto t1 = std::chrono::high_resolution_clock::now();

		const auto path = fs::path{ CONFIGPATH("Registry") };
		std::error_code ec{};
		if (!fs::exists(path, ec) || fs::is_empty(path, ec)) {
			const auto msg = ec ? fmt::format("An error occured while initializing SexLab animations.\nError: {}", ec.message()) :
														fmt::format("Unable to load SexLab animations. Folder path {} is empty or does not exist.", path.string());
			logger::critical("{}", msg);
			if (MessageBox(nullptr, fmt::format("{}\n\nExit game now?", msg).c_str(), "SexLab p+ Registry", 0x00000004) == 6)
				std::_Exit(EXIT_FAILURE);
			return;
		}

		std::vector<std::thread> threads;
		for (auto& file : fs::directory_iterator{ path }) {
			threads.emplace_back([this, file]() {
				try {
					auto package = Decoder::Decode(file);
					for (auto&& scene : package->scenes) {
						// for each scene, construct a list of every possible combination of fragments and add them to the list of hashes
						// the list then allows fast access to a slice of the library based on the hash fragments a given collection of actors represents
						std::vector<PositionHeader> headerFragments{ PositionHeader(0) };
						if (scene->furnituredata.allowbed) {
							headerFragments.push_back(PositionHeader::AllowBed);
						}
						// Build 2D Vector containing all possible Fragments for all Infos
						std::vector<std::vector<PositionFragmentation>> fragments;
						fragments.reserve(scene->positions.size());
						for (auto&& pinfo : scene->positions) {
							auto element = pinfo.MakeFragments();
							if (pinfo.extra.all(PositionInfo::Extra::Optional)) {
								element.push_back(PositionFragment::None);
							}
							fragments.push_back(element);
						}
						std::vector<std::vector<PositionFragmentation>::iterator> it;
						for (auto& subvec : fragments)
							it.push_back(subvec.begin());
						// Cycle through every combination of every vector
						assert(it.size() > 0 && it.size() == fragments.size());
						const auto K = it.size() - 1;
						while (it[0] != fragments[0].end()) {
							std::vector<PositionFragment> argFragment;
							argFragment.reserve(it.size());
							for (const auto& current : it) {
								if (*current == PositionFragment::None) {
									continue;
								}
								argFragment.push_back(current->get());
							}
							std::sort(argFragment.begin(), argFragment.end());
							for (const auto& argHeader : headerFragments) {
								auto key = ConstructHashKey(argFragment, argHeader);

								const std::unique_lock lock{ read_write_lock };
								const auto where = scenes.find(key);
								if (where == scenes.end()) {
									scenes.insert({ key, { scene.get() } });
								} else {
									auto& vec = where->second;
									if (std::find(vec.begin(), vec.end(), scene.get()) != vec.end()) {
										vec.push_back(scene.get());
									}
								}
							}
							// Next
							++it[K];
							for (auto i = K; i > 0 && it[i] == fragments[i].end(); i--) {
								it[i] = fragments[i].begin();
								++it[i - 1];
							}
						}
					}
					const std::unique_lock lock{ read_write_lock };
					for (auto&& scene : package->scenes) {
						scene_map.insert({ scene->id, scene.get() });
					}
					packages.push_back(std::move(package));
				} catch (const std::exception& e) {
					logger::critical("Unable to read registry file {}. The animation pack will NOT be added to the library. | Error: {}", file.path().filename().string(), e.what());
				}
			});
		}
		for (auto& thread : threads) {
			thread.join();
		}
		const auto t2 = std::chrono::high_resolution_clock::now();
		// auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::chrono::duration<double, std::milli> ms_double = t2 - t1;

		logger::info("Loaded {} Packages ({} scenes | {} categories) in {}ms", packages.size(), GetSceneCount(), scenes.size(), ms_double.count());
	}

	const Scene* Library::GetSceneByID(const std::string& a_id) const
	{
		const auto where = scene_map.find(a_id);
		return where != scene_map.end() ? where->second : nullptr;
	}

	const Scene* Library::GetSceneByID(const RE::BSFixedString& a_id) const
	{
		std::string id{ a_id.data() };
		Registry::ToLower(id);
		return GetSceneByID(id);
	}

	// std::vector<Scene*> Library::LookupAnimations(
	// 	std::vector<RE::Actor*>& a_actors,
	// 	[[maybe_unused]] const std::vector<std::string_view>& tags,
	// 	[[maybe_unused]] std::vector<RE::Actor*>& a_submissives) const
	// {
	// 	const auto t1 = std::chrono::high_resolution_clock::now();
	// 	// COMEBACK: Open thread to parse tags while constructing key here

	// 	std::vector<std::pair<PositionFragmentation, size_t>> fragments;
	// 	for (size_t i = 0; i < a_actors.size(); i++) {
	// 		auto fragment = MakePositionFragment(a_actors[i], std::find(a_submissives.begin(), a_submissives.end(), a_actors[i]) != a_submissives.end());
	// 		fragments.emplace_back(fragment, i);
	// 	}
	// 	std::stable_sort(fragments.begin(), fragments.end(), [](auto& a, auto& b) {
	// 		return a.first.underlying() < b.first.underlying();
	// 	});
	// 	std::vector<PositionFragment> strippedFragments;
	// 	strippedFragments.reserve(fragments.size());
	// 	for (auto&& fragment : fragments) {
	// 		strippedFragments.push_back(fragment.first.get());
	// 	}
	// 	const auto hash = ConstructHashKey(strippedFragments, PositionHeader::None);

	// 	const std::shared_lock lock{ read_write_lock };
	// 	const auto rawScenes = this->scenes.at(hash);
		
	// 	// TODO: validate scale of the given actors with the specific position if enabled


	// 	const auto t2 = std::chrono::high_resolution_clock::now();
	// 	// auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	// 	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	// 	// logger::info("Found {} animations for {} actors in {}ms", a_actors.size(), GetSceneCount(), scenes.size(), ms_double.count());
	// }

	std::vector<RE::Actor*> Library::SortByScene(const std::vector<std::pair<RE::Actor*, PositionFragment>>& a_positions, const Scene* a_scene) const
	{
		assert(a_positions.size() == a_scene->positions.size());
		std::vector<std::vector<std::pair<size_t, RE::Actor*>>> entries;
		entries.reserve(a_positions.size());
		for (size_t i = 0; i < a_positions.size(); i++) {
			for (size_t n = 0; n < a_scene->positions.size(); n++) {
				if (a_scene->positions[i].CanFillPosition(a_positions[i].second)) {
					entries[i].emplace_back(n, a_positions[i].first);
				}
			}
		}

		// Go through every of entries combination and pick the first which consists exclusively of unique elements
		std::vector<std::vector<std::pair<size_t, RE::Actor*>>::iterator> it;
		for (auto& subvec : entries)
	    it.push_back(subvec.begin());
		const auto last_idx = it.size() - 1;
		while (it[0] != entries[0].end()) {
			std::vector<RE::Actor*> result{ it.size(), nullptr };
			for (auto&& current : it) {
				if (std::find(result.begin(), result.end(), current->second) != result.end()) {
					goto NEXT;
				}
				result[current->first] = current->second;
			}
			assert(std::find(result.begin(), result.end(), nullptr) == result.end());
			return result;

NEXT:
			++it[last_idx];
			for (auto i = last_idx; i > 0 && it[i] == entries[i].end(); i--) {
				it[i] = entries[i].begin();
				++it[i - 1];
			}
		}
		return {};
	}

	LibraryKey Library::ConstructHashKey(const std::vector<PositionFragment>& fragments, PositionHeader a_extra) const
	{
		assert(fragments.size() <= 5);
		LibraryKey ret{};
		ret |= static_cast<std::underlying_type<PositionFragment>::type>(fragments[0]);
		size_t i = 1;
		for (; i < fragments.size(); i++) {
			ret <<= PositionFragmentSize;
			ret |= static_cast<std::underlying_type<PositionFragment>::type>(fragments[i]);
		}
		for (size_t n = i; n < 5; n++) {
			ret <<= PositionFragmentSize;
		}
		ret <<= PositionHeaderSize;
		ret |= static_cast<std::underlying_type<PositionHeader>::type>(a_extra);
		return ret;
	}

	LibraryKey Library::ConstructHashKeyUnsorted(std::vector<PositionFragment>& a_fragments, PositionHeader a_extra) const {
		std::sort(a_fragments.begin(), a_fragments.end(), [](auto a, auto b) { return static_cast<uint64_t>(a) < static_cast<uint64_t>(b); });
		return ConstructHashKey(a_fragments, a_extra);
	}

	size_t Library::GetSceneCount() const
	{
		size_t ret = 0;
		for (auto&& package : packages) {
			ret += package->scenes.size();
		}
		return ret;
	}

	// std::vector<Scene*> Library::GetByTags(int32_t a_positions, const std::vector<RE::BSFixedString>& a_tags) const
	// {
	// 	std::vector<Scene*> ret{};
	// 	ret.reserve(scene_map.size() >> 4);
	// 	for (auto&& [key, scene] : scene_map) {
	// 		if (scene->positions.size() != a_positions)
	// 			continue;

			
	// 	}
	// 	return ret;
	// }
}
