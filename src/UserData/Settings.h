#pragma once

namespace Settings
{
	void Initialize();			// Pre LoadData
	void InitializeData();	// Post LoadData
	void Save();

	// --- MCM
	// Booleans
	static inline bool bCreatureGender{ false };
	static inline bool bAllowCreatures{ false };
	static inline bool bUseStrapons{ true };
	static inline bool bRedressVictim{ true };
	static inline bool bUseLipSync{ false };
	static inline bool bUseExpressions{ false };
	static inline bool bUseCum{ true };
	static inline bool bDisablePlayer{ false };
	static inline bool bAutoTFC{ false };
	static inline bool bAutoAdvance{ true };
	static inline bool bOrgasmEffects{ false };
	static inline bool bLimitedStrip{ true };
	static inline bool bRestrictSameSex{ false };
	static inline bool bShowInMap{ false };
	static inline bool bDisableTeleport{ true };
	static inline bool bDisableScale{ false };
	static inline bool bUndressAnimation{ false };

	// Integers
	static inline int32_t iAskBed{ 1 };
	static inline int32_t iNPCBed{ 0 };
	static inline int32_t iOpenMouthSize{ 80 };
	static inline int32_t iUseFade{ 2 };

	// KeyBinds/Scene Control Related
	static inline bool AdjustTargetStage{ false };
	static inline int32_t iAdjustStage{ 157 };
	static inline int32_t iBackwards{ 54 };
	static inline int32_t iAdvanceAnimation{ 57 };
	static inline int32_t iChangeAnimation{ 24 };
	static inline int32_t iChangePositions{ 13 };
	static inline int32_t iAdjustChange{ 37 };
	static inline int32_t iAdjustForward{ 38 };
	static inline int32_t iAdjustSideways{ 40 };
	static inline int32_t iAdjustUpward{ 39 };
	static inline int32_t iRealignActors{ 26 };
	static inline int32_t iMoveScene{ 27 };
	static inline int32_t iRestoreOffsets{ 12 };
	static inline int32_t iRotateScene{ 22 };
	static inline int32_t iEndAnimation{ 207 };
	static inline int32_t iAdjustSchlong{ 46 };

	// Misc Keys
	static inline int32_t iToggleFreeCamera{ 81 };
	static inline int32_t iTargetActor{ 49 };

	// Floats
	static inline float fCumTimer{ 120.0 };
	static inline float fShakeStrength{ 0.7 };
	static inline float fAutoSUCSM{ 5.0 };
	static inline float fMaleVoiceDelay{ 5.0 };
	static inline float fFemaleVoiceDelay{ 4.0 };
	static inline float fExpressionDelay{ 2.0 };
	static inline float fVoiceVolume{ 1.0 };
	static inline float fSFXDelay{ 3.0 };
	static inline float fSFXVolume{ 1.0 };

	// Int Array
	static inline std::vector<int> iStripForms{ 1032555423, 1, 1032555423, 1, 4719365, 1, 16901, 1, 3952148, 0, 83952148, 0, 352389654, 1, 352389654, 1 };

	// Float Array
	static inline std::vector<float> fTimers{ 30, 20, 15, 15, 9, 10, 10, 10, 8, 8, 20, 15, 10, 10, 4 };
	static inline std::vector<float> fOpenMouthMale{ 0, 0.8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16 };
	static inline std::vector<float> fOpenMouthFemale{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16 };

	// Phoneme Related
	static inline int32_t iLipsPhoneme{ 1 };
	static inline bool bLipsFixedValue{ true };
	static inline int32_t iLipsSoundTime{ 0 };
	static inline int32_t LipsMaxValue{ 20 };
	static inline int32_t LipsSoundTime{ 50 };
	static inline float fLipsMoveTime{ 0.2 };

	// --- INI
	// Animation
	static inline uint32_t iFurniturePrefWeight{ 2 };	 // Weight ised in FindCenter() to use Furniture over default center. Chance = (1 / weight + 1)
	static inline float fScanRadius{ 750.0f };				 // Radius used in FindCenter() in which to look for potential furniture refs
	static inline float fMinScale{ 0.88f };						 // Min Scale for an actor be animated
	static inline bool bAllowDead{ false };						 // if dead actors are allowed in the framework

	// Race
	static inline bool bAshHopper{ true };
	static inline bool bBear{ true };
	static inline bool bBoar{ true };
	static inline bool bBoarMounted{ true };
	static inline bool bBoarSingle{ true };
	static inline bool bCanine{ true };
	static inline bool bChaurus{ true };
	static inline bool bChaurusHunter{ true };
	static inline bool bChaurusReaper{ true };
	static inline bool bChicken{ true };
	static inline bool bCow{ true };
	static inline bool bDeer{ true };
	static inline bool bDog{ true };
	static inline bool bDragon{ true };
	static inline bool bDragonPriest{ true };
	static inline bool bDraugr{ true };
	static inline bool bDwarvenBallista{ true };
	static inline bool bDwarvenCenturion{ true };
	static inline bool bDwarvenSphere{ true };
	static inline bool bDwarvenSpider{ true };
	static inline bool bFalmer{ true };
	static inline bool bFlameAtronach{ true };
	static inline bool bFox{ true };
	static inline bool bFrostAtronach{ true };
	static inline bool bGargoyle{ true };
	static inline bool bGiant{ true };
	static inline bool bGiantSpider{ true };
	static inline bool bGoat{ true };
	static inline bool bHagraven{ true };
	static inline bool bHare{ true };
	static inline bool bHorker{ true };
	static inline bool bHorse{ true };
	static inline bool bIceWraith{ true };
	static inline bool bLargeSpider{ true };
	static inline bool bLurker{ true };
	static inline bool bMammoth{ true };
	static inline bool bMudcrab{ true };
	static inline bool bNetch{ true };
	static inline bool bRiekling{ true };
	static inline bool bSabrecat{ true };
	static inline bool bSeeker{ true };
	static inline bool bSkeever{ true };
	static inline bool bSlaughterfish{ true };
	static inline bool bSpider{ true };
	static inline bool bSpriggan{ true };
	static inline bool bStormAtronach{ true };
	static inline bool bTroll{ true };
	static inline bool bVampireLord{ true };
	static inline bool bWerewolf{ true };
	static inline bool bWisp{ true };
	static inline bool bWispmother{ true };
	static inline bool bWolf{ true };

	// --- Misc
	inline std::vector<RE::FormID> SOS_ExcludeFactions{};

	// --- MCM Access
	struct StringCmp
	{
		bool operator()(const std::string& a_lhs, const std::string& a_rhs) const
		{
			return _strcmpi(a_lhs.c_str(), a_rhs.c_str()) < 0;
		}
	};

	enum class VariantType
	{
		FLOAT = 0,			 // f
		STRING = 1,			 // s
		BOOL = 2,				 // b
		INT = 3,				 // i
		FLOATARRAY = 4,	 // v
		INTARRAY = 5		 // j
	};

#define ENTRY(var) \
	{                \
		#var##s, &var  \
	}
	static inline std::map<std::string, std::variant<float*, std::string*, bool*, int*, std::vector<float>*, std::vector<int>*>, StringCmp> table{
		ENTRY(bCreatureGender),
		ENTRY(bAllowCreatures),
		ENTRY(bUseStrapons),
		ENTRY(bRedressVictim),
		ENTRY(bUseLipSync),
		ENTRY(bUseExpressions),
		ENTRY(bUseCum),
		ENTRY(bDisablePlayer),
		ENTRY(bAutoTFC),
		ENTRY(bAutoAdvance),
		ENTRY(bOrgasmEffects),
		ENTRY(bLimitedStrip),
		ENTRY(bRestrictSameSex),
		ENTRY(bShowInMap),
		ENTRY(bDisableTeleport),
		ENTRY(bDisableScale),
		ENTRY(bUndressAnimation),
		ENTRY(iAskBed),
		ENTRY(iNPCBed),
		ENTRY(iOpenMouthSize),
		ENTRY(iUseFade),
		ENTRY(AdjustTargetStage),
		ENTRY(iAdjustStage),
		ENTRY(iBackwards),
		ENTRY(iAdvanceAnimation),
		ENTRY(iChangeAnimation),
		ENTRY(iChangePositions),
		ENTRY(iAdjustChange),
		ENTRY(iAdjustForward),
		ENTRY(iAdjustSideways),
		ENTRY(iAdjustUpward),
		ENTRY(iRealignActors),
		ENTRY(iMoveScene),
		ENTRY(iRestoreOffsets),
		ENTRY(iRotateScene),
		ENTRY(iEndAnimation),
		ENTRY(iAdjustSchlong),
		ENTRY(iToggleFreeCamera),
		ENTRY(iTargetActor),
		ENTRY(fCumTimer),
		ENTRY(fShakeStrength),
		ENTRY(fAutoSUCSM),
		ENTRY(fMaleVoiceDelay),
		ENTRY(fFemaleVoiceDelay),
		ENTRY(fExpressionDelay),
		ENTRY(fVoiceVolume),
		ENTRY(fSFXDelay),
		ENTRY(fSFXVolume),
		ENTRY(iStripForms),
		ENTRY(fTimers),
		ENTRY(fOpenMouthMale),
		ENTRY(fOpenMouthFemale),
		ENTRY(iLipsPhoneme),
		ENTRY(bLipsFixedValue),
		ENTRY(iLipsSoundTime),
		ENTRY(LipsMaxValue),
		ENTRY(LipsSoundTime),
		ENTRY(fLipsMoveTime)
	};

}	 // namespace Settings
