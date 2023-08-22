#include "ConfigData.h"

namespace UserData
{
	std::optional<Registry::FurnitureType> ConfigData::CheckCustomType(const RE::TESModel* const a_model) const
	{
		std::string model = a_model->GetModel();
		Registry::ToLower(model);
		const auto entry = model_data.find(model);
		if (entry == model_data.end())
			return Registry::FurnitureType::None;

		return entry->second.type;
	}

	void ConfigData::AdjustOffsetByModel(const RE::TESModel* const a_model, std::vector<float>& a_offset) const
	{
    assert(a_model && a_offset.size() == Offset::Total);

		std::string model = a_model->GetModel();
		Registry::ToLower(model);
		const auto entry = model_data.find(model);
		if (entry == model_data.end())
      return;

		const auto& data = entry->second;
		a_offset[Offset::X] += data.offsets[Offset::X];
		a_offset[Offset::Y] += data.offsets[Offset::Y];
		a_offset[Offset::Z] += data.offsets[Offset::Z];
		a_offset[Offset::R] += data.offsets[Offset::R];
	}

	void ConfigData::AdjustOffsetByStage(const std::string& a_scene, const std::string& a_stage, size_t a_position, std::vector<float>& a_offset) const
	{
		assert(a_offset.size() == Offset::Total);

		const auto entry = stage_offsets.find(std::make_tuple(a_scene, a_stage, a_position));
		if (entry == stage_offsets.end())
			return;

		a_offset[Offset::X] += entry->second[Offset::X];
		a_offset[Offset::Y] += entry->second[Offset::Y];
		a_offset[Offset::Z] += entry->second[Offset::Z];
		a_offset[Offset::R] += entry->second[Offset::R];
	}

} // namespace UserData
