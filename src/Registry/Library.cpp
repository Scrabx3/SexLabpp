#include "Library.h"

#include "Decode.h"
#include "Define/RaceKey.h"
#include "Util/Combinator.h"

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
					std::vector<std::pair<LibraryKey, SceneEntry>> hashes;
					for (auto&& scene : package->scenes) {
						// for each scene, construct a list of every possible combination of fragments and add them to the list of hashes
						// the list then allows fast access to a slice of the library based on the hash fragments a given collection of actors represents
						std::vector<PositionHeader> headerFragments{ PositionHeader(0) };
						if (scene->furnituredata.allowbed) {
							headerFragments.push_back(PositionHeader::AllowBed);
						}
						// Build 2D Vector containing all possible Fragments for all Infos
						std::vector<std::vector<std::pair<PositionFragment, size_t>>> fragments;
						fragments.reserve(scene->positions.size());
						for (size_t i = 0; i < scene->positions.size(); i++) {
							auto frags_ = scene->positions[i].MakeFragments();
							std::vector<std::pair<PositionFragment, size_t>> fragPiece{};
							fragPiece.reserve(frags_.size());
							for (auto&& frag_it : frags_) {
								fragPiece.emplace_back(frag_it, i);
							}
							fragments.push_back(std::move(fragPiece));
						}
						std::vector<std::vector<std::pair<PositionFragment, size_t>>::iterator> it;
						for (auto& subvec : fragments)
							it.push_back(subvec.begin());
						// Cycle through every combination of every vector
						assert(it.size() > 0 && it.size() == fragments.size());
						const auto K = it.size() - 1;
						while (it[0] != fragments[0].end()) {
							auto copy = it;	 // work on copy to not mix up iterator order
							std::sort(copy.begin(), copy.end(), [](auto& a, auto& b) {
								return static_cast<FragmentUnderlying>(a->first) < static_cast<FragmentUnderlying>(b->first);
							});
							SceneEntry entry{};
							entry.scene = scene.get();
							std::vector<PositionFragment> argFragment;
							argFragment.reserve(copy.size());
							entry.order.reserve(copy.size());
							for (const auto& current : copy) {
								argFragment.push_back(current->first);
								entry.order.push_back(current->second);
							}
							for (const auto& argHeader : headerFragments) {
								auto key = ConstructHashKey(argFragment, argHeader);
								const auto pair = std::make_pair(key, entry);
								hashes.push_back(pair);
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
					for (auto&& entry : hashes) {
						const auto where = scenes.find(entry.first);
						if (where == scenes.end()) {
							scenes.insert({ entry.first, { std::move(entry.second) } });
						} else {
							where->second.push_back(std::move(entry.second));
						}
					}
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

	Scene* Library::GetSceneByID(const std::string& a_id)
	{
		const auto where = scene_map.find(a_id);
		return where != scene_map.end() ? where->second : nullptr;
	}

	Scene* Library::GetSceneByID(const RE::BSFixedString& a_id)
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

	// 	std::vector<std::pair<PositionFragment, size_t>> fragments;
	// 	for (size_t i = 0; i < a_actors.size(); i++) {
	// 		auto fragment = BuildFragment(a_actors[i], std::find(a_submissives.begin(), a_submissives.end(), a_actors[i]) != a_submissives.end());
	// 		fragments.emplace_back(fragment, i);
	// 	}
	// 	std::stable_sort(fragments.begin(), fragments.end(), [](auto& a, auto& b) {
	// 		return static_cast<FragmentUnderlying>(a.first) < static_cast<FragmentUnderlying>(b.first);
	// 	});
	// 	std::vector<PositionFragment> strippedFragments;
	// 	strippedFragments.reserve(fragments.size());
	// 	for (auto&& fragment : fragments) {
	// 		strippedFragments.push_back(fragment.first);
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
}
