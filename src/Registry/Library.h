#pragma once

#include "Animation.h"
#include "Define/Fragment.h"
#include <shared_mutex>

namespace Registry
{
	class Library : public Singleton<Library>
	{
	public:
		void Initialize() noexcept;

		_NODISCARD std::vector<Scene*> LookupScenes(std::vector<RE::Actor*>& a_actors, const std::vector<std::string_view>& tags, const std::vector<RE::Actor*>& a_submissives) const;
		_NODISCARD std::vector<Scene*> GetByTags(int32_t a_positions, const std::vector<std::string_view>& a_tags) const;

		_NODISCARD const Scene* GetSceneByID(const std::string& a_id) const;
		_NODISCARD const Scene* GetSceneByID(const RE::BSFixedString& a_id) const;
		_NODISCARD size_t GetSceneCount() const;

	public:
		void ForEachScene(std::function<bool(const Scene*)> a_visitor) const;

	private:
		mutable std::shared_mutex read_write_lock{};

		std::map<std::string_view, Scene*> scene_map;									 // Mapping every scene to their respective id for quick lookup
		std::vector<std::unique_ptr<AnimPackage>> packages;						 // All registered packages, containing all available scenes
		std::unordered_map<FragmentHash, std::vector<Scene*>> scenes;	 // The main lookup table using LibraryKeys
	};
}