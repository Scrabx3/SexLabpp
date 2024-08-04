#pragma once

struct Settings
{
	static constexpr float POPULATION_HETERO_DEFAULT{ 80.0f };
	static constexpr float POPULATION_HOMO_DEFAULT{ 8.0f };

	static void Initialize();	 // Pre LoadData
	static void InitializeYAML();
	static void InitializeINI();
	static void InitializeData();	 // Post LoadData
	static void Save();

	// --- MCM
	// Booleans
	static inline bool bDebugMode{ false };
	static inline bool bCreatureGender{ false };
	static inline bool bAllowCreatures{ false };
	static inline bool bRedressVictim{ true };
	static inline bool bUseLipSync{ false };
	static inline bool bUseExpressions{ false };
	static inline bool bUseCum{ true };
	static inline bool bDisablePlayer{ false };
	static inline bool bAutoTFC{ false };
	static inline bool bAutoAdvance{ true };
	static inline bool bOrgasmEffects{ false };
	static inline bool bShowInMap{ false };
	static inline bool bSetAnimSpeedByEnjoyment{ false };
	static inline bool bDisableTeleport{ true };
	static inline bool bDisableScale{ false };
	static inline bool bUndressAnimation{ false };
	static inline bool bSubmissivePlayer{ false };
	static inline bool bSubmissiveTarget{ false };
	static inline bool bMatchMakerActive{ false };

	// Integers
	static inline int32_t iAskBed{ 1 };
	static inline int32_t iNPCBed{ 0 };
	static inline int32_t iUseFade{ 1 };
	static inline int32_t iClimaxType{ 0 };
	static inline int32_t iFilterStrictness{ 2 };

	// KeyBinds/Scene Control Related
	static inline bool bAdjustTargetStage{ false };
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
	static inline float fCumTimer{ 180.0 };
	static inline float fShakeStrength{ 0.7 };
	static inline float fAutoSUCSM{ 5.0 };
	static inline float fMaleVoiceDelay{ 5.0 };
	static inline float fFemaleVoiceDelay{ 4.0 };
	static inline float fVoiceVolume{ 1.0 };
	static inline float fSFXDelay{ 3.0 };
	static inline float fSFXVolume{ 1.0 };

	// Strings
	static inline std::string sRequiredTags{ ""s };
	static inline std::string sExcludedTags{ ""s };
	static inline std::string sOptionalTags{ ""s };

	// Arrays
	static inline std::vector<int> iStripForms{ 1032555423, 1, 1032555423, 1, 806420, 0, 352928413, 1 };
	static inline std::vector<float> fTimers{ 10.0f, 15.0f, 25.0f, 7.0f };

	// Phoneme Related
	static inline bool bLipsFixedValue{ true };
	static inline int32_t iLipsSoundTime{ 1 };

	// --- INI
	// Animation
	static inline uint32_t iFurniturePrefWeight{ 2 };	 // Weight ised in FindCenter() to use Furniture over default center. Chance = (1 / weight + 1)
	static inline float fScanRadius{ 750.0f };				 // Radius used in FindCenter() in which to look for potential furniture refs
	static inline float fMinScale{ 0.88f };						 // Min Scale for an actor be animated
	static inline bool bAllowDead{ false };						 // if dead actors are allowed in the framework
	static inline float fMinSetupTime{ 0.4f };

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

	// --- Statistics
	static inline float fPercentageHetero{ POPULATION_HETERO_DEFAULT };
	static inline float fPercentageHomo{ POPULATION_HOMO_DEFAULT };

	// --- Distances
	static inline float fDistanceHead{ 14.7f };			 // distance from head node to lips = 9.3
	static inline float fDistanceFoot{ 13.3f };			 // ~7.2 distance to foot middle
	static inline float fDistanceHand{ 8.3f };			 // ~2.2 distance to surface
	static inline float fDistanceCrotch{ 18.0f };		 // Distance from Pelvis/Hip node to crotch
	static inline float fAnglePenetration{ 35.0f };	 // Angle relative to crotch area below which the schlong is no longer considered penetrating
	static inline float fAngleGrinding{ 30.0f };		 // angle for schlong and cortch to be considered "parallel"
	static inline float fAngleMouth{ 20.0f };				 // Angle of the cone from headnode to schlong that interprets the schlong in front of mouth

	// --- Enjoyment
	static inline float fEnjGrinding{ 0.075 };
	static inline float fEnjHandActive{ 0.375 };
	static inline float fEnjHandPassive{ 0.475 };
	static inline float fEnjFootActive{ 0.175 };
	static inline float fEnjFootPassive{ 0.3 };
	static inline float fEnjOralActive{ 0.525 };
	static inline float fEnjOralPassive{ 0.575 };
	static inline float fEnjVaginalActive{ 0.725 };
	static inline float fEnjVaginalPassive{ 0.825 };
	static inline float fEnjAnalActive{ 0.925 };
	static inline float fEnjAnalPassive{ 1.025 };
	static inline float fFactorNonInterEnjRaise{ 0.6 };
	static inline float fFactorInterEnjRaise{ 1.2 };
	static inline float fTimeMax{ 30.0 };
	static inline float fRequiredXP{ 50.0 };
	static inline float fBoostTime{ 30.0 };
	static inline float fPenaltyTime{ 80.0 };
	static inline uint8_t iMaxNoPainOrgasmsM{ 1 };
	static inline uint8_t iMaxNoPainOrgasmsF{ 2 };

	// --- Misc
	static inline std::vector<RE::FormID> SOS_ExcludeFactions{};

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
		ENTRY(bRedressVictim),
		ENTRY(bUseLipSync),
		ENTRY(bUseExpressions),
		ENTRY(bUseCum),
		ENTRY(bDisablePlayer),
		ENTRY(bAutoTFC),
		ENTRY(bAutoAdvance),
		ENTRY(bOrgasmEffects),
		ENTRY(bShowInMap),
		ENTRY(bSetAnimSpeedByEnjoyment),
		ENTRY(bDisableTeleport),
		ENTRY(bDisableScale),
		ENTRY(bUndressAnimation),
		ENTRY(iAskBed),
		ENTRY(iNPCBed),
		ENTRY(iUseFade),
		ENTRY(bAdjustTargetStage),
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
		ENTRY(fVoiceVolume),
		ENTRY(fSFXDelay),
		ENTRY(fSFXVolume),
		ENTRY(iStripForms),
		ENTRY(fTimers),
		ENTRY(bLipsFixedValue),
		ENTRY(iLipsSoundTime),
		ENTRY(iClimaxType),
		ENTRY(bSubmissivePlayer),
		ENTRY(bSubmissiveTarget),
		ENTRY(sRequiredTags),
		ENTRY(sExcludedTags),
		ENTRY(sOptionalTags),
		ENTRY(iFilterStrictness),
		ENTRY(bDebugMode),
		ENTRY(bMatchMakerActive),
	};
};
