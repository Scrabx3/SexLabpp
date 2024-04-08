#pragma once

namespace Papyrus::ExpressionSlots
{
	enum ExpressionStatus
	{
		None = 0,
		Submissive = 1,
		Dominant = 2
	};

	std::vector<RE::BSFixedString> GetAllProfileIDs(RE::StaticFunctionTag*);
	std::vector<RE::BSFixedString> GetExpressionsByStatus(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, int a_status);
	std::vector<RE::BSFixedString> GetExpressionsByTags(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::string_view a_tags);

		inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetAllProfileIDs, "sslExpressionSlots", true);
		REGISTERFUNC(GetExpressionsByStatus, "sslExpressionSlots", true);
		REGISTERFUNC(GetExpressionsByTags, "sslExpressionSlots", true);

		return true;
	}
}	 // namespace Papyrus::ExpressionSlots
