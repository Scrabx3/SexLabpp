set(CXX_FILES
	src/PCH.h
	src/main.cpp

	src/Random.h
	src/Singleton.h
	src/Script.h
	src/GameForms.h

	src/IPluginInterface.h

	src/UserData/Settings.h
	src/UserData/Settings.cpp
	src/UserData/StripData.h
	src/UserData/StripData.cpp

	# src/Hooks/Hooks.h
	# src/Hooks/Hooks.cpp
	src/Registry/Util/RayCast/bhkLinearCastCollector.h
	src/Registry/Util/RayCast/bhkRigidBodyT.h
	src/Registry/Util/RayCast/Math.h
	src/Registry/Util/RayCast/ObjectBound.h
	src/Registry/Util/RayCast/ObjectBound.cpp
	src/Registry/Util/RayCast/RayCast.cpp
	src/Registry/Util/RayCast/Offsets.h

	src/Registry/Util/CellCrawler.h
	src/Registry/Util/Combinatorics.h
	src/Registry/Util/RayCast.h
	src/Registry/Util/Scale.h
	src/Registry/Util/Scale.cpp

	src/Registry/Define/Fragment.h
	src/Registry/Define/Fragment.cpp
	src/Registry/Define/Furniture.h
	src/Registry/Define/Furniture.cpp
	src/Registry/Define/RaceKey.h
	src/Registry/Define/RaceKey.cpp
	src/Registry/Define/Sex.h
	src/Registry/Define/Sex.cpp
	src/Registry/Define/Tags.h
	src/Registry/Define/Tags.cpp
	src/Registry/Define/Transform.h
	src/Registry/Define/Transform.cpp

	src/Registry/Animation.h
	src/Registry/Animation.cpp
	src/Registry/Library.h
	src/Registry/Library.cpp
	src/Registry/Misc.h
	src/Registry/Misc.cpp
	src/Registry/Stats.h
	src/Registry/Stats.cpp
	src/Registry/Validation.h
	src/Registry/Validation.cpp

	src/Papyrus/LegacyData.h
	src/Papyrus/Serialize.h
	src/Papyrus/Sound.h
	src/Papyrus/Sound.cpp

	src/Papyrus/SexLabRegistry.h
	src/Papyrus/SexLabRegistry.cpp
	src/Papyrus/sslActorAlias.h
	src/Papyrus/sslActorAlias.cpp
	src/Papyrus/sslActorLibrary.h
	src/Papyrus/sslActorLibrary.cpp
	src/Papyrus/sslAnimationSlots.h
	src/Papyrus/sslAnimationSlots.cpp
	src/Papyrus/sslCreatureAnimationSlots.h
	src/Papyrus/sslCreatureAnimationSlots.cpp
	src/Papyrus/sslSystemConfig.h
	src/Papyrus/sslSystemConfig.cpp
	src/Papyrus/sslThreadLibrary.h
	src/Papyrus/sslThreadLibrary.cpp
	src/Papyrus/sslThreadModel.h
	src/Papyrus/sslThreadModel.cpp
)
