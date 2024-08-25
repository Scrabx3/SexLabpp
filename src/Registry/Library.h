#pragma once

#include <shared_mutex>

#include "Animation.h"
#include "Define/Fragment.h"
#include "Define/Furniture.h"

namespace Registry
{
	class Library : public Singleton<Library>
	{
	public:
		void Initialize() noexcept;

		_NODISCARD std::vector<Scene*> LookupScenes(std::vector<RE::Actor*>& a_actors, const std::vector<std::string_view>& tags, const std::vector<RE::Actor*>& a_submissives) const;
		_NODISCARD std::vector<Scene*> GetByTags(int32_t a_positions, const std::vector<std::string_view>& a_tags) const;

		_NODISCARD const Scene* GetSceneByID(const RE::BSFixedString& a_id) const;
		_NODISCARD Scene* GetSceneByID(const RE::BSFixedString& a_id);
		_NODISCARD const Scene* GetSceneByName(const RE::BSFixedString& a_id) const;
		_NODISCARD Scene* GetSceneByName(const RE::BSFixedString& a_id);
		_NODISCARD size_t GetSceneCount() const;

		void ForEachPackage(std::function<bool(const AnimPackage*)> a_visitor) const;
		void ForEachScene(std::function<bool(const Scene*)> a_visitor) const;

	public:
		_NODISCARD const FurnitureDetails* GetFurnitureDetails(const RE::TESObjectREFR* a_ref) const;

	public:
		void Save();
		void Load();

	private:
		mutable std::shared_mutex read_write_lock{};

		FurnitureDetails offset_bedroll{ FurnitureType::BedRoll, Coordinate(std::vector{ 0.0f, 0.0f, 7.5f, 180.0f }) };
		FurnitureDetails offset_bedsingle{ FurnitureType::BedSingle, Coordinate(std::vector{ 0.0f, -31.0f, 42.5f, 0.0f }) };
		FurnitureDetails offset_beddouble{ FurnitureType::BedDouble, Coordinate(std::vector{ 0.0f, -31.0f, 42.5f, 0.0f }) };
		std::map<RE::BSFixedString, std::unique_ptr<FurnitureDetails>, FixedStringCompare> furnitures;	// custom furniture details

		std::map<RE::BSFixedString, Scene*, FixedStringCompare> scene_map;	// Mapping every scene to their respective id for quick lookup
		std::vector<std::unique_ptr<AnimPackage>> packages;									// All registered packages, containing all available scenes
		std::unordered_map<FragmentHash, std::vector<Scene*>> scenes;				// The main lookup table using LibraryKeys
	};
}
