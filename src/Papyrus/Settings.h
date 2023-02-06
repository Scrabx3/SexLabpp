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
			NoStrip = -1,
			Always = 1,

			None = 0
		};

		Strip CheckStrip(RE::TESForm* a_form);
		Strip CheckKeywords(RE::TESForm* a_form);

	public:
		void Load();
		void Save();

		void AddArmor(RE::TESForm* a_form, Strip a_type) { strips[a_form->formID] = a_type; }
		void RemoveArmor(RE::TESForm* a_form) { strips.erase(a_form->formID); }
		void RemoveArmorAll() { strips.clear(); }

	private:
		std::map<RE::FormID, Strip> strips{};
		YAML::Node _root;
	};

	class MCMConfig :
		public Singleton<MCMConfig>
	{
		struct MCMAbstract
		{
			virtual ~MCMAbstract() = default;

			virtual void Load(const YAML::Node& a_node, Script::ObjectPtr& a_mcm) const = 0;
			virtual void Save(YAML::Node& a_node, const Script::ObjectPtr& a_mcm) const = 0;
		};

		template <class T>
		struct MCMSetting : public MCMAbstract
		{
			MCMSetting(const char* a_setting) :
				_setting(a_setting) {}
			~MCMSetting() = default;

			virtual void Load(const YAML::Node& a_node, Script::ObjectPtr& a_mcm) const
			{
				if (!a_node[_setting].IsDefined()) {
					logger::error("Setting {} is missing from config", _setting);
					return;
				}
				Script::SetProperty(a_mcm, _setting, a_node[_setting].as<T>());
			}

			virtual void Save(YAML::Node& a_node, const Script::ObjectPtr& a_mcm) const
			{
				a_node[_setting] = Script::GetProperty<T>(a_mcm, _setting);
			}

			std::string _setting;
		};

	public:
		void LoadDefaults();
		void Load();
		void Save();

	private:
		const std::vector<std::unique_ptr<MCMAbstract>> settings = []() {
			std::remove_const<decltype(settings)>::type ret;
			ret.reserve(128);
			// Install
			ret.emplace_back(new MCMSetting<bool>("bInstallDefaults"));
			ret.emplace_back(new MCMSetting<bool>("bInstallDefaultsCrt"));
			// Misc
			ret.emplace_back(new MCMSetting<bool>("RestrictAggressive"));
			ret.emplace_back(new MCMSetting<bool>("AllowCreatures"));
			ret.emplace_back(new MCMSetting<bool>("NPCSaveVoice"));
			ret.emplace_back(new MCMSetting<bool>("UseStrapons"));
			ret.emplace_back(new MCMSetting<bool>("RedressVictim"));
			ret.emplace_back(new MCMSetting<bool>("UndressAnimation"));
			ret.emplace_back(new MCMSetting<bool>("UseLipSync"));
			ret.emplace_back(new MCMSetting<bool>("UseExpressions"));
			ret.emplace_back(new MCMSetting<bool>("RefreshExpressions"));
			ret.emplace_back(new MCMSetting<bool>("ScaleActors"));
			ret.emplace_back(new MCMSetting<bool>("UseCum"));
			ret.emplace_back(new MCMSetting<bool>("AllowFFCum"));
			ret.emplace_back(new MCMSetting<bool>("DisablePlayer"));
			ret.emplace_back(new MCMSetting<bool>("AutoTFC"));
			ret.emplace_back(new MCMSetting<bool>("AutoAdvance"));
			ret.emplace_back(new MCMSetting<bool>("ForeplayStage"));
			ret.emplace_back(new MCMSetting<bool>("OrgasmEffects"));
			ret.emplace_back(new MCMSetting<bool>("RaceAdjustments"));
			ret.emplace_back(new MCMSetting<bool>("BedRemoveStanding"));
			ret.emplace_back(new MCMSetting<bool>("UseCreatureGender"));
			ret.emplace_back(new MCMSetting<bool>("LimitedStrip"));
			ret.emplace_back(new MCMSetting<bool>("RestrictSameSex"));
			ret.emplace_back(new MCMSetting<bool>("SeparateOrgasms"));
			ret.emplace_back(new MCMSetting<bool>("RemoveHeelEffect"));
			ret.emplace_back(new MCMSetting<bool>("AdjustTargetStage"));
			ret.emplace_back(new MCMSetting<bool>("ShowInMap"));
			ret.emplace_back(new MCMSetting<bool>("DisableTeleport"));
			ret.emplace_back(new MCMSetting<bool>("SeedNPCStats"));
			ret.emplace_back(new MCMSetting<bool>("DisableScale"));
			ret.emplace_back(new MCMSetting<bool>("LipsFixedValue"));
			ret.emplace_back(new MCMSetting<int>("AnimProfile"));
			ret.emplace_back(new MCMSetting<int>("AskBed"));
			ret.emplace_back(new MCMSetting<int>("NPCBed"));
			ret.emplace_back(new MCMSetting<int>("OpenMouthSize"));
			ret.emplace_back(new MCMSetting<int>("UseFade"));
			ret.emplace_back(new MCMSetting<int>("Backwards"));
			ret.emplace_back(new MCMSetting<int>("AdjustStage"));
			ret.emplace_back(new MCMSetting<int>("AdvanceAnimation"));
			ret.emplace_back(new MCMSetting<int>("ChangeAnimation"));
			ret.emplace_back(new MCMSetting<int>("AdjustChange"));
			ret.emplace_back(new MCMSetting<int>("AdjustForward"));
			ret.emplace_back(new MCMSetting<int>("AdjustSideways"));
			ret.emplace_back(new MCMSetting<int>("AdjustUpward"));
			ret.emplace_back(new MCMSetting<int>("RealignActors"));
			ret.emplace_back(new MCMSetting<int>("MoveScene"));
			ret.emplace_back(new MCMSetting<int>("RestoreOffsets"));
			ret.emplace_back(new MCMSetting<int>("RotateScene"));
			ret.emplace_back(new MCMSetting<int>("EndAnimation"));
			ret.emplace_back(new MCMSetting<int>("ToggleFreeCamera"));
			ret.emplace_back(new MCMSetting<int>("TargetActor"));
			ret.emplace_back(new MCMSetting<int>("AdjustSchlong"));
			ret.emplace_back(new MCMSetting<int>("LipsSoundTime"));
			ret.emplace_back(new MCMSetting<int>("LipsPhoneme"));
			ret.emplace_back(new MCMSetting<int>("LipsMinValue"));
			ret.emplace_back(new MCMSetting<int>("LipsMaxValue"));
			ret.emplace_back(new MCMSetting<float>("CumTimer"));
			ret.emplace_back(new MCMSetting<float>("ShakeStrength"));
			ret.emplace_back(new MCMSetting<float>("AutoSUCSM"));
			ret.emplace_back(new MCMSetting<float>("MaleVoiceDelay"));
			ret.emplace_back(new MCMSetting<float>("FemaleVoiceDelay"));
			ret.emplace_back(new MCMSetting<float>("ExpressionDelay"));
			ret.emplace_back(new MCMSetting<float>("VoiceVolume"));
			ret.emplace_back(new MCMSetting<float>("SFXDelay"));
			ret.emplace_back(new MCMSetting<float>("SFXVolume"));
			ret.emplace_back(new MCMSetting<float>("LeadInCoolDown"));
			ret.emplace_back(new MCMSetting<float>("LipsMoveTime"));
			ret.emplace_back(new MCMSetting<std::vector<int>>("iStripForms"));
			ret.emplace_back(new MCMSetting<std::vector<float>>("fTimers"));
			ret.emplace_back(new MCMSetting<std::vector<float>>("OpenMouthMale"));
			ret.emplace_back(new MCMSetting<std::vector<float>>("OpenMouthFemale"));
			return ret;
		}();
	};

	inline std::vector<RE::FormID> SOS_ExcludeFactions{};

	void LoadData();
}	 // namespace Settings
