#pragma once

namespace Registry::Collision::NiMath
{
	using Segment = std::pair<RE::NiPoint3, RE::NiPoint3>;
	Segment ClosestSegmentBetweenSegments(const Segment& u, const Segment& v);

	RE::NiMatrix3 Rodrigue(RE::NiPoint3 start, RE::NiPoint3 end, RE::NiPoint3 vIdeal);
	RE::NiMatrix3 Rodrigue(RE::NiPoint3 vector, RE::NiPoint3 vIdeal);

	float GetVectorAngle(const RE::NiPoint3& v1, const RE::NiPoint3& v2);

}	 // namespace Registry::Collision::NiMath
