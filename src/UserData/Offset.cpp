#include "Offset.h"

#include "Registry/Animation.h"

namespace UserData
{
	void Offset::AdjustModelOffset(std::vector<float>& a_offset, const RE::TESModel* const a_model)
  {
    assert(a_model && a_offset.size() == Registry::Offset::Total);

    const auto entry = model_offsets.find(a_model->GetModel());
    if (entry == model_offsets.end())
      return;

		a_offset[Registry::Offset::X] += entry->second[Registry::Offset::X];
		a_offset[Registry::Offset::Y] += entry->second[Registry::Offset::Y];
		a_offset[Registry::Offset::Z] += entry->second[Registry::Offset::Z];
		a_offset[Registry::Offset::R] += entry->second[Registry::Offset::R];
	}

	void Offset::AdjustStageOffset(std::vector<float>& a_offset, size_t n, const std::string& a_scene, const std::string& a_stage)
	{
		const auto entry = stage_offsets.find({ a_scene, a_stage, n });
		if (entry == stage_offsets.end())
			return;

		a_offset[Registry::Offset::X] += entry->second[Registry::Offset::X];
		a_offset[Registry::Offset::Y] += entry->second[Registry::Offset::Y];
		a_offset[Registry::Offset::Z] += entry->second[Registry::Offset::Z];
		a_offset[Registry::Offset::R] += entry->second[Registry::Offset::R];
	}

} // namespace UserData
