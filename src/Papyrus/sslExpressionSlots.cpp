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
  
} // namespace Papyrus::ExpressionSlots
