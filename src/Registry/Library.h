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
		void Initialize();

		_NODISCARD std::vector<Scene*> LookupAnimations(
			std::vector<RE::Actor*>& a_actors,
			const std::vector<std::string_view>& tags,
			std::vector<RE::Actor*>& a_submissives) const;

		_NODISCARD const Scene* GetSceneByID(const std::string& a_id) const;
		_NODISCARD const Scene* GetSceneByID(const RE::BSFixedString& a_id) const;
		_NODISCARD size_t GetSceneCount() const;

		_NODISCARD std::vector<RE::Actor*> SortByScene(const std::vector<std::pair<RE::Actor*, PositionFragment>>& a_positions, const Scene* a_scene) const;

	public:
		_NODISCARD std::vector<Scene*> GetByTags(int32_t a_positions, const std::vector<std::string_view>& a_tags) const;

		_NODISCARD std::vector<RE::BGSRefAlias*> MapToProxy(const RE::TESQuest* a_owner, const std::vector<Scene*>& a_scenes) const;

	private:
		// Construct a library key from a **sorted** list of fragments
		LibraryKey ConstructHashKey(const std::vector<PositionFragment>& a_fragments, PositionHeader a_extra) const;
		LibraryKey ConstructHashKeyUnsorted(std::vector<PositionFragment>& a_fragments, PositionHeader a_extra) const;

	private:
		mutable std::shared_mutex read_write_lock{};

		std::map<std::string_view, Scene*> scene_map;
		std::vector<std::unique_ptr<AnimPackage>> packages;
		std::unordered_map<LibraryKey, std::vector<Scene*>> scenes;

		std::vector<std::pair<RE::TESQuest*, std::map<Scene*, RE::BGSRefAlias*>>> legacy_mapping;
	};
}