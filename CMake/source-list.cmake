set(CXX_FILES
	src/PCH.h
	src/main.cpp
	src/Serialization.h
	src/GameForms.h

	src/API/IPluginInterface.h

	src/UserData/Settings.h
	src/UserData/Settings.cpp
	src/UserData/StripData.h
	src/UserData/StripData.cpp

	src/Util/Combinatorics.h
	src/Util/FormLookup.h
	src/Util/Misc.h
	src/Util/Premutation.h
	src/Util/Random.h
	src/Util/Script.h
	src/Util/Singleton.h
	src/Util/StringUtil.h
	src/Util/FormLookup.h
	src/Util/World.h

	# src/Hooks/Hooks.h
	# src/Hooks/Hooks.cpp
	src/Registry/Util/RayCast/bhkLinearCastCollector.h
	src/Registry/Util/RayCast/bhkRigidBodyT.h
	src/Registry/Util/RayCast/Math.h
	src/Registry/Util/RayCast/ObjectBound.h
	src/Registry/Util/RayCast/ObjectBound.cpp
	src/Registry/Util/RayCast/RayCast.cpp
	src/Registry/Util/RayCast/Offsets.h

	src/Registry/Util/Decode.h
	src/Registry/Util/RayCast.h
	src/Registry/Util/SAT.h
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
	src/Registry/Expression.h
	src/Registry/Expression.cpp
	src/Registry/Library.h
	src/Registry/Library.cpp
	src/Registry/Stats.h
	src/Registry/Stats.cpp
	src/Registry/Validation.h
	src/Registry/Validation.cpp
	src/Registry/Voice.h
	src/Registry/Voice.cpp

	src/Papyrus/sslLibrary/LegacyData.h
	src/Papyrus/sslLibrary/Serialize.h
	src/Papyrus/sslLibrary/sslActorLibrary.cpp
	src/Papyrus/sslLibrary/sslActorLibrary.h
	src/Papyrus/sslLibrary/sslThreadLibrary.cpp
	src/Papyrus/sslLibrary/sslThreadLibrary.h
	src/Papyrus/sslObject/sslAnimationSlots.cpp
	src/Papyrus/sslObject/sslAnimationSlots.h
	src/Papyrus/sslObject/sslCreatureAnimationSlots.cpp
	src/Papyrus/sslObject/sslCreatureAnimationSlots.h
	src/Papyrus/sslObject/sslExpressionSlots.cpp
	src/Papyrus/sslObject/sslExpressionSlots.h
	src/Papyrus/sslObject/sslVoiceSlots.cpp
	src/Papyrus/sslObject/sslVoiceSlots.h
	src/Papyrus/Papyrus.cpp
	src/Papyrus/Papyrus.h
	src/Papyrus/SexLabRegistry.cpp
	src/Papyrus/SexLabRegistry.h
	src/Papyrus/SexLabUtil.h
	src/Papyrus/sslActorStats.cpp
	src/Papyrus/sslActorStats.h
	src/Papyrus/sslSceneMenu.cpp
	src/Papyrus/sslSceneMenu.h
	src/Papyrus/sslSystemConfig.cpp
	src/Papyrus/sslSystemConfig.h
	src/Papyrus/sslThreadModel.cpp
	src/Papyrus/sslThreadModel.h

	src/Thread/Interface/Interface.h
	src/Thread/Interface/SceneMenu.h
	src/Thread/Interface/SceneMenu.cpp

	src/Thread/NiNode/NiMath.h
	src/Thread/NiNode/NiMath.cpp
	src/Thread/NiNode/NiPosition.h
	src/Thread/NiNode/NiPosition.cpp
	src/Thread/NiNode/NiUpdate.h
	src/Thread/NiNode/NiUpdate.cpp
	src/Thread/NiNode/Node.h
	src/Thread/NiNode/Node.cpp
)
