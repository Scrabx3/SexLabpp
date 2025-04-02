#pragma once

namespace Registry
{
	class CumFx :
		public Singleton<CumFx>
	{
		using FxPair = std::pair<RE::BSFixedString, uint8_t>;

	public:
		enum FxType
		{
			Vaginal = 0,
			Anal,
			Oral,

			MainThree = Oral,

			Total
		};

	public:
		void Initialize();
		RE::BSFixedString PickRandomFxSet(FxType a_type) const;
		uint8_t GetFxCount(FxType a_type, RE::BSFixedString a_set) const;

	private:
		std::optional<uint8_t> ParseType(const fs::directory_entry& a_typePath);

	private:
		std::vector<FxPair> fxList[FxType::Total];
	};
}	 // namespace Registry
