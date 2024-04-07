#pragma once

namespace Papyrus::ExpressionSlots
{
	std::vector<RE::BSFixedString> GetAllProfileIDs(RE::StaticFunctionTag*);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(GetAllProfileIDs, "sslExpressionSlots", true);

		return true;
	}
}	 // namespace Papyrus::ExpressionSlots
