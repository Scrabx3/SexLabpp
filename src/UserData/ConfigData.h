#pragma once

#include "Registry/Animation.h"

namespace UserData
{
	using Offset = Registry::Offset;

	/// @brief Information on custom furniture definitions
	struct ModelOffset
	{
		std::string_view filepath;
		std::string modelpath;

		Registry::FurnitureType type;
		std::array<float, Offset::Total> offsets;
	};

	/// @brief Data about user defined/external offset related information
	class ConfigData :
		public Singleton<ConfigData>
	{
	public:
		std::optional<Registry::FurnitureType> CheckCustomType(const RE::TESModel* const a_model) const;
		void AdjustOffsetByModel(const RE::TESModel* const a_model, std::vector<float>& a_offset) const;
		void EditModelOffset(const RE::TESModel* const a_model, Offset a_direction, float a_value);

		void AdjustOffsetByStage(const std::string& a_scene, const std::string& a_stage, size_t a_position, std::vector<float>& a_offset) const;
		void EditStageOffset(const std::string& a_scene, const std::string& a_stage, size_t a_position, Offset a_direction, float a_value);

	public:
		void Load();
		void Save();

	private:
		std::map<std::string_view, ModelOffset> model_data;
		std::map<std::tuple<std::string_view, std::string_view, size_t>, std::array<float, Offset::Total>> stage_offsets;
	};
}	 // namespace UserData
