#pragma once

#include "Animation.h"
#include <shared_mutex>

namespace Registry
{
	static inline constexpr size_t MAX_ACTOR_COUNT = 5;

	enum class LibraryFragment
	{
		None = 0,

		Male = 1 << 0,
		Female = 1 << 1,
		Futa = (Male | Female),

		Human = 1 << 2,
		Vampire = 1 << 3,
		Yoke = 1 << 4,
		Arminder = 1 << 5,
		HandShackle = (Yoke | Arminder),
		LegsBound = 1 << 6,
		Petsuit = 1 << 7,
		// Unused = 1 << 8,

		CrtBit0 = 1 << 3,
		CrtBit1 = 1 << 4,
		CrtBit2 = 1 << 5,
		CrtBit3 = 1 << 6,
		CrtBit4 = 1 << 7,
		CrtBit5 = 1 << 8,

		Submissive = 1 << 9,
		Unconscious = 1 << 10,
	};
	using FragmentUnderlying = std::underlying_type<LibraryFragment>::type;
	static inline constexpr size_t LibraryFragmentSize = 11;

	enum class LibraryHeaderFragment
	{
		None = 0,

		AllowBed = 1ULL << 0,
		// Unused = 1ULL << 61,
		// Unused = 1ULL << 62,
		// Unused = 1ULL << 63,
	};
	static inline constexpr size_t LibraryHeaderFragmentSize = 1;
	using FragmentHeaderUnderlying = std::underlying_type<LibraryHeaderFragment>::type;

	using LibraryKey = std::bitset<MAX_ACTOR_COUNT * LibraryFragmentSize + LibraryHeaderFragmentSize>;

	class Library : public Singleton<Library>
	{
	public:
		struct SceneEntry
		{
			Scene* scene;
			std::vector<size_t> order;
		};

	public:
		void Initialize();

		_NODISCARD std::vector<Scene*> LookupAnimations(
			std::vector<RE::Actor*>& a_actors,
			const std::vector<std::string_view>& tags,
			std::vector<RE::Actor*>& a_submissives) const;
		_NODISCARD Scene* GetSceneByID(const std::string& a_id);

		_NODISCARD size_t GetSceneCount() const;

	private:
		// The library operates by creating a hashmap through combination of HashFragments
		// these hash keys are explicetely unique for a specific type of actor, PositionInfos are moreso
		// "masks" that can represent multiple fragments at once
		stl::enumeration<LibraryFragment, FragmentUnderlying> BuildFragment(RE::Actor* a_actor, bool a_submissive) const;
		std::vector<LibraryFragment> BuildFragmentsFromInfo(const PositionInfo& a_position) const;
		LibraryKey ConstructHashKey(const std::vector<LibraryFragment>& a_fragments, LibraryHeaderFragment a_extra) const;

	private:
		mutable std::shared_mutex read_write_lock{};

		std::map<std::string_view, Scene*> scene_map;
		std::vector<std::unique_ptr<AnimPackage>> packages;
		std::unordered_map<LibraryKey, std::vector<SceneEntry>> scenes;
	};
}