#include "NiMath.h"

namespace Registry::Collision::NiMath
{
	Segment ClosestSegmentBetweenSegments(const Segment& u, const Segment& v)
	{
		const auto vU = u.second - u.first, vV = v.second - v.first, vR = v.first - u.first;
		const auto lU = vU.SqrLength(), lV = vV.SqrLength();
		const bool isPointU = lU <= FLT_EPSILON, isPointV = lV <= FLT_EPSILON;

		if (isPointU &&  isPointV) {
			return std::make_pair(u.first, v.first);
		}
		const auto vUvR = vU.Dot(vR), vVvR = vV.Dot(vR);

		float s, t;
		if (isPointU) {
			s = 0.0f;
			t = std::clamp(-vVvR / lV, 0.0f, 1.0f);
		} else if (isPointV) {
			s = std::clamp(vUvR / lU, 0.0f, 1.0f);
			t = 0.0f;
		} else {
			const auto vUvV = vV.Dot(vU);
			const auto det = lU * lV - vUvV * vUvV;

			if (det < FLT_EPSILON * lU * lV) {
				s = std::clamp(vUvR / lU, 0.0f, 1.0f);
				t = 0.0f;
			} else {
				s = std::clamp((vUvR * lV - vVvR * vUvR) / det, 0.0f, 1.0f);
				t = std::clamp((vUvR * vUvR - vVvR * lU) / det, 0.0f, 1.0f);
			}
			s = std::clamp((t * vUvR + vUvR) / lU, 0.0f, 1.0f);
			t = std::clamp((s * vUvR - vVvR) / lV, 0.0f, 1.0f);
		}

		const auto c1 = u.first + (vU * s);
		const auto c2 = v.first + (vV * t);
		return std::make_pair(c1, c2);
	}

	RE::NiMatrix3 Rodrigue(RE::NiPoint3 start, RE::NiPoint3 end, RE::NiPoint3 vIdeal)
	{
		auto vRef = end - start;
    return Rodrigue(vRef, vIdeal);
	}

	RE::NiMatrix3 Rodrigue(RE::NiPoint3 vector, RE::NiPoint3 vIdeal)
	{
		vector.Unitize();
		vIdeal.Unitize();
		const auto cross = vector.Cross(vIdeal);
		const auto cos_theta = vIdeal.Dot(vector);
		const auto theta = std::acos(cos_theta);
		if (cross.SqrLength() < FLT_EPSILON)
			return RE::NiMatrix3{};

		RE::NiMatrix3 I{}, u{
			{ 0.0f, cross.z, -cross.y },
			{ -cross.z, 0.0f, cross.x },
			{ cross.y, -cross.x, 0.0f }
		};
		auto result = I + (u * sin(theta)) + ((u * u) * (1 - cos_theta));
		return result;
	}

	float GetVectorAngle(const RE::NiPoint3& v1, const RE::NiPoint3& v2)
	{
		const auto dot = v1.Dot(v2);
		const auto l = v1.Length() * v2.Length();
		const auto x = dot / l;
		return RE::rad_to_deg(std::acosf(x));
	}

}	 // namespace Registry::Collision::NiMath
