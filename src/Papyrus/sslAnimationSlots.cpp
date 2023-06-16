#include "sslAnimationSlots.h"

#include "Registry/Animation.h"
#include "Registry/Library.h"

namespace Papyrus::AnimationSlots
{
	std::vector<RE::BGSRefAlias*> GetByTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int32_t a_actorcount, std::vector<std::string_view> a_tags)
  {
		if (a_actorcount <= 0 || a_actorcount > 5) {
			a_vm->TraceStack("Actor count should be between 0 and 5", a_stackID);
      return {};
		}
    const auto lib = Registry::Library::GetSingleton();
    const auto scenes = lib->GetByTags(a_actorcount, a_tags);
    return lib->MapToProxy(a_qst, scenes);
	}

} // namespace Papyrus::AnimationSlots
