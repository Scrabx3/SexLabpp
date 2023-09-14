#pragma once

namespace Hooks
{
	constexpr auto no_collision_flag = static_cast<std::uint32_t>(RE::CFilter::Flag::kNoCollision);

	struct CollisionHandler
	{
		static bool DoDisableCollision(RE::Actor* a_actor, float a_delta)
    {
      if (!a_actor->IsInFaction(GameForms::AnimatingFaction))
        return false;

			const auto charController = a_actor->GetCharController();
			if (!charController)
				return false;

			const auto& bumpedColObj = charController->bumpedCharCollisionObject;
			if (!bumpedColObj)
				return false;

			auto& filter = bumpedColObj->collidable.broadPhaseHandle.collisionFilterInfo;
			if (filter & (no_collision_flag))
				return false;

			const auto bumpedColRef = RE::TESHavokUtilities::FindCollidableRef(bumpedColObj->collidable);
			if (!bumpedColRef)
				return false;

			const auto actorref = bumpedColRef->As<RE::Actor>();
			if (actorref && !actorref->IsInFaction(GameForms::AnimatingFaction))
				return false;

			filter |= no_collision_flag;

			func(a_actor, a_delta);

			filter &= ~no_collision_flag;

			return true;
		}

		static void thunk(RE::Actor* a_actor, float a_delta)
		{
			if (!DoDisableCollision(a_actor, a_delta)) {
				return func(a_actor, a_delta);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		// TODO: find VR offset
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(36359, 37350), OFFSET(0xF0, 0xFB) };
		stl::write_thunk_call<CollisionHandler>(target.address());
	}

}	 // namespace Hooks
