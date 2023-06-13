#pragma once

#include "Animation.h"
#include <shared_mutex>

namespace Registry
{
	static inline constexpr size_t MAX_ACTOR_COUNT = 5;
	using LibraryKey = std::bitset<MAX_ACTOR_COUNT * PositionFragmentSize + PositionHeaderSize>;

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
		_NODISCARD Scene* GetSceneByID(const RE::BSFixedString& a_id);
		_NODISCARD size_t GetSceneCount() const;

	private:
		// The library operates by creating a hashmap through combination of HashFragments
		// these hash keys are explicetely unique for a specific type of actor, PositionInfos are moreso
		// "masks" that can represent multiple fragments at once
		LibraryKey ConstructHashKey(const std::vector<PositionFragment>& a_fragments, PositionHeader a_extra) const;

	private:
		mutable std::shared_mutex read_write_lock{};

		std::map<std::string_view, Scene*> scene_map;
		std::vector<std::unique_ptr<AnimPackage>> packages;
		std::unordered_map<LibraryKey, std::vector<SceneEntry>> scenes;
	};
}