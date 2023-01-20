#pragma once

#include "Singleton.h"

namespace Settings
{
	class StripConfig :
		public Singleton<StripConfig>
	{
	public:
		enum class Strip
		{
			Never = -1,
			Always = 1,

			None = 0
		};

	public:
		void Load();
		void Save();

		void AddArmor(RE::TESForm* a_form, Strip a_type);
		void RemoveArmor(RE::TESForm* a_form, Strip a_type);
		Strip CheckStrip(RE::TESForm* a_form);

	private:
		std::vector<RE::FormID> NoStrip{};
		std::vector<RE::FormID> AlwaysStrip{};

		YAML::Node _root;
	};

	inline std::vector<RE::FormID> SOS_ExcludeFactions{};

	void LoadData();
}	 // namespace Settings
