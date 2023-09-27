#include "../RayCast.h"
#include "Offsets.h"
#include "bhkLinearCastCollector.h"

/* From SmoothCam */
SkyrimSE::bhkLinearCastCollector* getCastCollector() noexcept
{
	static SkyrimSE::bhkLinearCastCollector collector = SkyrimSE::bhkLinearCastCollector();
	return &collector;
}

Raycast::RayResult Raycast::hkpCastRay(const glm::vec4& start, const glm::vec4& end) noexcept {
	return hkpCastRay(start, end, {});
}

Raycast::RayResult Raycast::hkpCastRay(const glm::vec4& start, const glm::vec4& end, std::initializer_list<const RE::TESObjectREFR*> a_filter) noexcept
{
	constexpr auto hkpScale = 0.0142875f;
	const auto dif = end - start;

	SkyrimSE::bhkRayCastInfo info;
	info.start = start * hkpScale;
	info.end = dif * hkpScale;
	info.collector = getCastCollector();
	info.collector->reset();

	for (auto&& ref : a_filter) {
		auto niobj = ref->Get3D();
		if (niobj) {
			info.collector->addFilter(niobj);
		}
	}

	const auto player = RE::PlayerCharacter::GetSingleton();
	if (!player->parentCell)
		return {};

	auto physicsWorld = player->parentCell->GetbhkWorld();
	if (physicsWorld) {
		typedef void (__thiscall RE::bhkWorld::*CastRay)(SkyrimSE::hkpRayCastInfo*) const;

		(physicsWorld->*reinterpret_cast<CastRay>(&RE::bhkWorld::PickObject))(&info);
		//physicsWorld->CastRay(&info); // <-- use this instead, fix param
	}

	SkyrimSE::bhkRayHitResult best = {};
	best.hitFraction = 1.0f;
	glm::vec4 bestPos = {};

	for (auto& hit : info.collector->results) {
		const auto pos = (dif * hit.hitFraction) + start;
		if (best.hit == nullptr) {
			best = hit;
			bestPos = pos;
			continue;
		}

		if (hit.hitFraction < best.hitFraction) {
			best = hit;
			bestPos = pos;
		}
	}

	RayResult result;
	result.hitPos = bestPos;
	result.rayLength = glm::length(bestPos - start);

	if (!best.hit)
		return result;

	typedef RE::NiAVObject* (*_GetUserData)(SkyrimSE::bhkShapeList*);
	auto getAVObject = REL::Relocation<_GetUserData>(Offsets::GetNiAVObject);
	auto av = getAVObject(best.hit);

	result.hit = av != nullptr;
	result.hitObject = av;

	return result;
}
