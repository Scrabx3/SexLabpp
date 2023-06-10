#pragma once

namespace Registry
{
  /// @brief Validate the given actor
  /// @return some code [1; -inf) if the actor is valid. See implementation for details
	bool IsValidActor(RE::Actor* a_actor);
	int32_t IsValidActorImpl(RE::Actor* a_actor);


} // namespace Registry
