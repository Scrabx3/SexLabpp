#include "sslAnimationSlots.h"

#include "Registry/Animation.h"
#include "Registry/Library.h"

namespace Papyrus::AnimationSlots
{
  int32_t GetAllocatedSize(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst)
	{
		if (!a_qst) {
			a_vm->TraceStack("Cannot call GetAllocatedSize on a none object", a_stackID);
			return 0;
		}
    return Registry::Library::GetSingleton()->GetProxySize(a_qst);
	}

	std::vector<RE::BGSRefAlias*> GetByTagsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, int32_t a_actorcount, std::vector<std::string_view> a_tags)
  {
		if (!a_qst) {
			a_vm->TraceStack("Cannot call GetByTagsImpl on a none object", a_stackID);
			return {};
		}
		if (a_actorcount <= 0 || a_actorcount > Registry::MAX_ACTOR_COUNT) {
			a_vm->TraceStack("Actor count should be between 0 and 5", a_stackID);
      return {};
		}
		const auto lib = Registry::Library::GetSingleton();
    const auto scenes = lib->GetByTags(a_actorcount, a_tags);
		return lib->MapToProxy(a_qst, scenes);
	}

	std::vector<RE::BGSRefAlias*> GetByTypeImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst,
		int32_t a_actorcount, int32_t a_males, int32_t a_females, std::vector<std::string_view> a_tags)
	{
		if (!a_qst) {
			a_vm->TraceStack("Cannot call GetByTypeImpl on a none object", a_stackID);
			return {};
		}
		if (a_actorcount <= 0 || a_actorcount > Registry::MAX_ACTOR_COUNT) {
			a_vm->TraceStack("Actor count should be between 0 and 5", a_stackID);
			return {};
		}
		const auto lib = Registry::Library::GetSingleton();
		auto scenes = lib->GetByTags(a_actorcount, a_tags);
		const auto end = std::remove_if(scenes.begin(), scenes.end(), [&](Registry::Scene* a_scene) {
			for (auto&& tag : a_scene->tags.extra) {
				std::string_view view{ tag.data() };
				if (view.find_first_not_of("MFC") != std::string_view::npos) {
					continue;
				}
				if (a_males != -1 && std::count(view.begin(), view.end(), 'M') != a_males)
					return true;
				if (a_females != -1 && std::count(view.begin(), view.end(), 'F') != a_females)
					return true;
				return false;
			}
      logger::error("Scene {}-{} has no sex tag", a_scene->hash, a_scene->id);
      return false;
		});
		scenes.erase(end, scenes.end());
		return lib->MapToProxy(a_qst, scenes);
	}

	std::vector<RE::BGSRefAlias*> PickByActorsImpl(VM* a_vm, StackID a_stackID, RE::TESQuest* a_qst, std::vector<RE::Actor*> a_positions, std::vector<std::string_view> a_tags)
	{
		if (!a_qst) {
			a_vm->TraceStack("Cannot call PickByActorsImpl on a none object", a_stackID);
			return {};
		}
		if (a_positions.empty() || std::find(a_positions.begin(), a_positions.end(), nullptr) != a_positions.end()) {
			a_vm->TraceStack("Cannot find scenes for a none position", a_stackID);
			return {};
		}
    const auto lib = Registry::Library::GetSingleton();
		std::vector<RE::Actor*> vic{};
		if (std::find(a_tags.begin(), a_tags.end(), "Forced") != a_tags.end()) {
			vic.push_back(a_positions[0]);
		}
		const auto scenes = lib->LookupScenes(a_positions, a_tags, vic);
		return lib->MapToProxy(a_qst, scenes);
	}

} // namespace Papyrus::AnimationSlots
