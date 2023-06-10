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
						std::vector<LibraryHeaderFragment> headerFragments{ LibraryHeaderFragment(0) };
						if (scene->furnituredata.allowbed) {
							headerFragments.push_back(LibraryHeaderFragment::AllowBed);
						}
						// Build 2D Vector containing all possible Fragments for all Infos
						using FragPair = std::pair<LibraryFragment, size_t>;
						std::vector<std::vector<FragPair>> fragments;
						fragments.reserve(scene->positions.size());
						for (size_t i = 0; i < scene->positions.size(); i++) {
							auto& pos_it = scene->positions[i];
							auto tmpFrags = BuildFragmentsFromInfo(pos_it);
							std::vector<FragPair> fragPiece{};
							fragPiece.reserve(tmpFrags.size());
							for (auto&& vec_it : tmpFrags) {
								fragPiece.emplace_back(vec_it, i);
							}
							fragments.push_back(std::move(fragPiece));
						}
						std::vector<std::vector<std::pair<LibraryFragment, size_t>>::iterator> it;
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
							std::vector<LibraryFragment> argFragment;
							argFragment.reserve(copy.size());
							entry.order.reserve(copy.size());
							for (const auto& current : it) {
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

		// std::vector<Scene*> Library::LookupAnimations(
		// 	std::vector<RE::Actor*>& a_actors,
		// 	[[maybe_unused]] const std::vector<std::string_view>& tags,
		// 	[[maybe_unused]] std::vector<RE::Actor*>& a_submissives) const
		// {
		// 	const auto t1 = std::chrono::high_resolution_clock::now();
		// 	// COMEBACK: Open thread to parse tags while constructing key here

		// 	std::vector<std::pair<LibraryFragment, size_t>> fragments;
		// 	for (size_t i = 0; i < a_actors.size(); i++) {
		// 		auto fragment = BuildFragment(a_actors[i], std::find(a_submissives.begin(), a_submissives.end(), a_actors[i]) != a_submissives.end());
		// 		fragments.emplace_back(fragment, i);
		// 	}
		// 	std::stable_sort(fragments.begin(), fragments.end(), [](auto& a, auto& b) {
		// 		return static_cast<FragmentUnderlying>(a.first) < static_cast<FragmentUnderlying>(b.first);
		// 	});
		// 	std::vector<LibraryFragment> strippedFragments;
		// 	strippedFragments.reserve(fragments.size());
		// 	for (auto&& fragment : fragments) {
		// 		strippedFragments.push_back(fragment.first);
		// 	}
		// 	const auto hash = ConstructHashKey(strippedFragments, LibraryHeaderFragment::None);

		// 	const std::shared_lock lock{ read_write_lock };
		// 	const auto rawScenes = this->scenes.at(hash);
		// 	// TODO: validate scale of the given actors with the specific position if enabled


		// 	const auto t2 = std::chrono::high_resolution_clock::now();
		// 	// auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		// 	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
		// 	// logger::info("Found {} animations for {} actors in {}ms", a_actors.size(), GetSceneCount(), scenes.size(), ms_double.count());
		// }

		stl::enumeration<LibraryFragment, FragmentUnderlying> Library::BuildFragment(RE::Actor* a_actor, bool a_submissive) const
	{
		auto base = a_actor->GetActorBase();
		if (!base) {
			logger::error("Invalid Actor {:X} (0): Missing base object", a_actor->formID);
			return { LibraryFragment::None };
		}

		auto sex = base ? base->GetSex() : RE::SEXES::kNone;
		stl::enumeration<LibraryFragment, FragmentUnderlying> ret{};
		switch (sex) {
		case RE::SEXES::kFemale:
			ret.set(Registry::IsFuta(a_actor) ? LibraryFragment::Futa : LibraryFragment::Female);
			break;
		case RE::SEXES::kMale:
			ret.set(LibraryFragment::Male);
			break;
		default:
			logger::error("Invalid Actor {:X} ({:X}): Missing sex", a_actor->formID, (base ? base->formID : 0));
			return { LibraryFragment::None };
		}

		const auto racekey = RaceHandler::GetRaceKey(a_actor);
		switch (racekey) {
		case RaceKey::None:
			logger::error("Invalid Actor {:X} ({:X}): Missing Racekey", a_actor->formID, base->formID);
			break;
		case RaceKey::Human:
			{
				ret.set(LibraryFragment::Human);
				if (a_actor->HasKeyword(GameForms::Vampire)) {
					ret.set(LibraryFragment::Vampire);
				}
				// COMEBACK: bound extra
			}
			break;
		default:
			{
				const auto val = LibraryFragment(static_cast<FragmentUnderlying>(racekey) << 3);
				ret.set(val);
			}
			break;
		}

		if (a_actor->IsDead() || a_actor->AsActorState()->IsUnconscious()) {
			ret.set(LibraryFragment::Unconscious);
		} else if (a_submissive) {
			ret.set(LibraryFragment::Submissive);
		}

		return ret;
	}

	std::vector<LibraryFragment> Library::BuildFragmentsFromInfo(const PositionInfo& a_position) const
	{
		using Fragment = stl::enumeration<LibraryFragment, FragmentUnderlying>;
		std::vector<Fragment> fragments;
		const auto addVariance = [&](LibraryFragment a_variancebit) {
			const auto count = fragments.size();
			for (size_t i = 0; i < count; i++) {
				auto copy = fragments[i];
				copy.set(a_variancebit);
				fragments.push_back(copy);
			}
		};
		const auto setFragmentBit = [&](LibraryFragment a_bit) {
			for (auto&& fragment : fragments) {
				fragment.set(a_bit);
			}
		};
		const auto addRaceVariance = [&](RaceKey a_type) {
			const auto val = LibraryFragment(static_cast<FragmentUnderlying>(a_type) << 3);
			const auto count = fragments.size();
			for (size_t i = 0; i < count; i++) {
				auto copy = fragments[i];
				copy.reset(LibraryFragment::CrtBit0,
					LibraryFragment::CrtBit1,
					LibraryFragment::CrtBit2,
					LibraryFragment::CrtBit3,
					LibraryFragment::CrtBit4,
					LibraryFragment::CrtBit5);
				copy.set(val);
				fragments.push_back(copy);
			}
		};
		const auto setRaceBit = [&](RaceKey a_type) {
			const auto val = LibraryFragment(static_cast<FragmentUnderlying>(a_type) << 3);
			setFragmentBit(val);
		};

		if (a_position.sex.all(Sex::Male)) {
			fragments.emplace_back(LibraryFragment::Male);
		}
		if (a_position.sex.all(Sex::Female)) {
			fragments.emplace_back(LibraryFragment::Female);
		}
		if (a_position.sex.all(Sex::Futa)) {
			fragments.emplace_back(LibraryFragment::Futa);
		}

		if (a_position.extra.all(PositionInfo::Extra::Unconscious)) {
			setFragmentBit(LibraryFragment::Unconscious);
		} else if (a_position.extra.all(PositionInfo::Extra::Submissive)) {
			setFragmentBit(LibraryFragment::Submissive);
		}

		switch (a_position.race) {
		case RaceKey::Human:
			{
				setFragmentBit(LibraryFragment::Human);
				if (a_position.extra.all(PositionInfo::Extra::Vamprie)) {
					setFragmentBit(LibraryFragment::Vampire);
				} else {
					addVariance(LibraryFragment::Vampire);
				}
				// COMEBACK: bound extra
			}
			break;
		case RaceKey::Boar:
			setRaceBit(RaceKey::BoarMounted);
			addRaceVariance(RaceKey::BoarSingle);
			break;
		case RaceKey::Canine:
			setRaceBit(RaceKey::Dog);
			addRaceVariance(RaceKey::Wolf);
		default:
			setRaceBit(a_position.race);
			break;
		}
		std::vector<LibraryFragment> ret;
		for (auto&& fragment : fragments) {
			ret.push_back(fragment.get());
		}
		return ret;
	}

	LibraryKey Library::ConstructHashKey(const std::vector<LibraryFragment>& fragments, LibraryHeaderFragment a_extra) const
	{
		assert(fragments.size() <= 5);
		LibraryKey ret{};
		ret |= static_cast<std::underlying_type<LibraryFragment>::type>(fragments[0]);
		size_t i = 1;
		for (; i < fragments.size(); i++) {
			ret <<= LibraryFragmentSize;
			ret |= static_cast<std::underlying_type<LibraryFragment>::type>(fragments[i]);
		}
		for (size_t n = i; n < 5; n++) {
			ret <<= LibraryFragmentSize;
		}
		ret <<= LibraryHeaderFragmentSize;
		ret |= static_cast<std::underlying_type<LibraryHeaderFragment>::type>(a_extra);
		return ret;
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
