#include "Papyrus/SexLabRegistry.h"
#include "Papyrus/SexLabUtil.h"
#include "Papyrus/sslActorAlias.h"
#include "Papyrus/sslActorLibrary.h"
#include "Papyrus/sslActorStats.h"
#include "Papyrus/sslAnimationSlots.h"
#include "Papyrus/sslBaseExpression.h"
#include "Papyrus/sslBaseVoice.h"
#include "Papyrus/sslCreatureAnimationSlots.h"
#include "Papyrus/sslExpressionSlots.h"
#include "Papyrus/sslSystemConfig.h"
#include "Papyrus/sslThreadLibrary.h"
#include "Papyrus/sslThreadModel.h"

namespace Papyrus
{
	bool Register()
	{
		const auto papyrus = SKSE::GetPapyrusInterface();
		if (!papyrus) {
			logger::critical("Failed to get papyurs interface");
			return false;
		}
		papyrus->Register(Papyrus::SexLabRegistry::Register);
		papyrus->Register(Papyrus::SexLabUtil::Register);
		papyrus->Register(Papyrus::ActorAlias::Register);
		papyrus->Register(Papyrus::ActorLibrary::Register);
		papyrus->Register(Papyrus::ActorStats::Register);
		papyrus->Register(Papyrus::AnimationSlots::Register);
		papyrus->Register(Papyrus::CreatureAnimationSlots::Register);
		papyrus->Register(Papyrus::BaseExpression::Register);
		papyrus->Register(Papyrus::BaseVoice::Register);
		papyrus->Register(Papyrus::ExpressionSlots::Register);
		papyrus->Register(Papyrus::SystemConfig::Register);
		papyrus->Register(Papyrus::ThreadLibrary::Register);
		papyrus->Register(Papyrus::ThreadModel::Register);

		return true;
	}
}	 // namespace Papyrus