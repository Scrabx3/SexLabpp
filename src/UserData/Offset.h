#pragma once

namespace UserData
{
  class Offset
	{
	public:
		static void AdjustModelOffset(std::vector<float>& a_offset, const RE::TESModel* const a_model);
		static void AdjustStageOffset(std::vector<float>& a_offset, size_t n, const std::string& a_scene, const std::string& a_stage);

		// static void EditModelOffset(const RE::TESModel* const a_model, std::array<float, 4> a_edit);	TODO:

		static void Load();
    static void Save();

	private:
		static inline std::map<std::string, std::array<float, 4>> model_offsets;
		static inline std::map<std::tuple<std::string, std::string, size_t>, std::array<float, 4>> stage_offsets;
	};
} // namespace UserData
