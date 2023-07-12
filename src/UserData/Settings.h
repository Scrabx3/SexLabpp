#pragma once

namespace Settings
{
  // --- MCM
  static inline bool bAllowCreatures{ false };
	static inline bool bCreatureGender{ false };

	// --- INI
	// Animation
	static inline float fScanRadius{ 750.0f };
	static inline float fMinScale{ 0.88f };
  static inline bool bAllowDead{ false };

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

}	 // namespace Settings
