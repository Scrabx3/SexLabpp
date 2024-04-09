#include "sslExpressionSlots.h"

#include "Registry/Expression.h"

namespace Papyrus::ExpressionSlots
{
	std::vector<RE::BSFixedString> GetAllProfileIDs(RE::StaticFunctionTag*)
  {
		std::vector<RE::BSFixedString> ret{};
		Registry::Expression::GetSingleton()->ForEachProfile([&](Registry::Expression::Profile& profile) {
			ret.push_back(profile.id);
      return false;
		});
    return ret;
	}
  
	std::vector<RE::BSFixedString> GetExpressionsByStatus(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int a_status)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::BSFixedString> ret{};
		RE::BSFixedString tag;
		switch (a_status) {
		case ExpressionStatus::Submissive:
			tag = "Victim";
			break;
		case ExpressionStatus::Dominant:
			tag = "Aggressor";
			break;
		default:
			tag = "Normal";
			break;
		}
		Registry::Expression::GetSingleton()->ForEachProfile([&](Registry::Expression::Profile& profile) {
			if (!profile.tags.HasTag(tag)) {
				return false;
			}
			ret.push_back(profile.id);
			return false;
		});
		return ret;
	}

	std::vector<RE::BSFixedString> GetExpressionsByTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::string_view a_tags)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is none", a_stackID);
			return {};
		}
		std::vector<RE::BSFixedString> ret{};
		auto list = Registry::StringSplit(a_tags);
		Registry::TagDetails search{ list };
		Registry::Expression::GetSingleton()->ForEachProfile([&](Registry::Expression::Profile& profile) {
			if (!search.MatchTags(profile.tags)) {
				return false;
			}
			ret.push_back(profile.id);
			return false;
		});
		return ret;
	}

} // namespace Papyrus::ExpressionSlots
