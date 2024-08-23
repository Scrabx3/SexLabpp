#include "Node.h"

#include "Registry/Define/RaceKey.h"
#include "Registry/Define/Transform.h"

namespace Registry::Node
{
	NodeData::NodeData(RE::Actor* a_actor)
	{
		const auto obj = a_actor->Get3D();
		if (!obj) {
			const auto msg = fmt::format("Unable to retrieve 3D of actor {:X}", a_actor->GetFormID());
			throw std::exception(msg.c_str());
		}
		const auto racekey = RaceHandler::GetRaceKey(a_actor);
		const auto racestr = racekey == RaceKey::None ? "?" : RaceHandler::AsString(racekey);
		const auto get = [&](auto str, auto& target, bool log) {
			auto node = obj->GetObjectByName(str);
			auto ninode = node ? node->AsNode() : nullptr;
			if (!ninode) {
				if (log)
					logger::info("Actor {:X} (Race: {}) is missing Node {} (This may be expected)", a_actor->GetFormID(), racestr, str);
				return false;
			}
			target = RE::NiPointer{ ninode };
			return true;
		};
		if (!get(PELVIS, pelvis, true) || !get(SPINELOWER, spine_lower, true)) {
			throw std::exception("Missing mandatory 3d object (body)");
		}
		get(HEAD, head, true);
		get(HANDLEFT, hand_left, true);
		get(HANDRIGHT, hand_right, false);
		get(FINGERLEFT, finger_left, false);
		get(FINGERRIGHT, finger_right, false);
		get(FOOTLEFT, foot_left, true);
		get(FOOTRIGHT, foot_right, false);
		get(TOELEFT, toe_left, true);
		get(TOERIGHT, toe_right, true);
		get(CLITORIS, clitoris, true);
		get(VAGINADEEP, vaginadeep, true);
		get(VAGINALLEFT, vaginaleft, false);
		get(VAGINALRIGHT, vaginaright, false);
		get(ANALDEEP, analdeep, true);
		get(ANALLEFT, analleft, false);
		get(ANALRIGHT, analright, false);
		get(ANIMOBJECTA, animobj_a, false);
		get(ANIMOBJECTB, animobj_b, false);
		get(ANIMOBJECTL, animobj_l, false);
		get(ANIMOBJECTR, animobj_r, false);
		for (auto&& it : SCHLONG_NODES) {
			SchlongData tmp(it.rot);
			int s = get(it.base, tmp.base, false) + get(it.mid, tmp.mid, false) + get(it.tip, tmp.tip, false);
			if (s == 0)
				continue;
			schlongs.push_back(tmp);
		}
	}

	RE::NiPoint3 NodeData::ApproximateNode(float a_forward, float a_upward) const
	{
		Coordinate approx(std::vector{ a_forward, 0.0f, a_upward, 0.0f });
		RE::NiPoint3 angle;
		pelvis->world.rotate.ToEulerAnglesXYZ(angle);
		Coordinate ret{ pelvis->world.translate, angle.z };
		approx.Apply(ret);
		return ret.AsNiPoint();
	}

	RE::NiPoint3 NodeData::ApproximateTip() const
	{
		constexpr float forward = 20.0f;
		constexpr float upward = -4.0f;
		return ApproximateNode(forward, upward);
	}

	RE::NiPoint3 NodeData::ApproximateMid() const
	{
		constexpr float forward = 15.0f;
		constexpr float upward = -6.2f;
		return ApproximateNode(forward, upward);
	}

	RE::NiPoint3 NodeData::ApproximateBase() const
	{
		constexpr float forward = 10.0f;
		constexpr float upward = -5.0f;
		return ApproximateNode(forward, upward);
	}

	std::vector<RE::NiPoint3> NodeData::GetSchlongTipReferencePoints(bool a_approximateifempty) const
	{
		std::vector<RE::NiPoint3> ret{};
		for (auto&& s : schlongs) {
			assert(s.base);
			if (s.tip) {
				ret.push_back(s.tip->world.translate);
			} else {
				const auto& world = s.base->world;
				const auto vforward = world.rotate.GetVectorY();
				const auto approximate = (vforward * 20.0f) + world.translate;
				ret.push_back(approximate);
			}
		}
		if (ret.empty() && a_approximateifempty)
			ret.push_back(ApproximateTip());
		return ret;
	}

	std::vector<RE::NiPoint3> NodeData::GetSchlongReferenceVectors(bool a_approximateifempty) const
	{
		std::vector<RE::NiPoint3> ret{};
		for (auto&& s : schlongs) {
			assert(s.base);
			RE::NiPoint3 refpoint = s.base->world.translate;
			if (s.mid) {
				ret.push_back(s.mid->world.translate - refpoint);
			} else if (s.tip) {
				ret.push_back(s.mid->world.translate - refpoint);
			} else {
				auto translate = s.rot * s.mid->world.rotate;
				ret.push_back(translate.GetVectorY());
			}
		}
		if (ret.empty() && a_approximateifempty) {
			const auto approxMid = ApproximateMid(), approxbase = ApproximateBase();
			ret.push_back(approxMid - approxbase);
		}
		return ret;
	}

	std::optional<RE::NiPoint3> NodeData::GetVaginalVector() const
	{
		if (!vaginadeep || !vaginaleft || !vaginaright)
			return std::nullopt;

		auto vagmid = GetVaginalStart();
		assert(vagmid);
		return *vagmid - vaginadeep->world.translate;
	}

	std::optional<RE::NiPoint3> NodeData::GetVaginalStart() const
	{
		if (!vaginaleft || !vaginaright)
			return std::nullopt;
		return (vaginaleft->world.translate + vaginaright->world.translate) / 2;
	}

	std::optional<RE::NiPoint3> NodeData::GetAnalVector() const
	{
		if (!analdeep || !analleft || !analright)
			return std::nullopt;
		
		auto analmid = GetAnalStart();
		assert(analmid);
		return *analmid - analdeep->world.translate;
	}

	std::optional<RE::NiPoint3> NodeData::GetAnalStart() const
	{
		if (!analleft || !analright)
			return std::nullopt;
		return (analleft->world.translate + analright->world.translate) / 2;
	}

	std::optional<RE::NiPoint3> NodeData::GetToeVectorLeft() const
	{
		if (!foot_left || !toe_left)
			return std::nullopt;
		return toe_left->world.translate - foot_left->world.translate;
	}

	std::optional<RE::NiPoint3> NodeData::GetToeVectorRight() const
	{
		if (!foot_right || !toe_right)
			return std::nullopt;
		return toe_right->world.translate - foot_right->world.translate;
	}
	
	std::optional<RE::NiPoint3> NodeData::GetHandVectorLeft() const
	{
		if (!hand_left || !finger_left)
			return std::nullopt;
		return finger_left->world.translate - hand_left->world.translate;
	}

	std::optional<RE::NiPoint3> NodeData::GetHandVectorRight() const
	{
		if (!hand_right || !finger_right)
			return std::nullopt;
		return finger_right->world.translate - hand_right->world.translate;
	}

	RE::NiPoint3 NodeData::GetCrotchVector() const
	{
		assert(pelvis && spine_lower);
		return spine_lower->world.translate - pelvis->world.translate;
	}

	RE::NiPoint3 NodeData::SchlongData::GetTipReferencePoint() const
	{
		assert(base);
		if (tip) {
			return tip->world.translate;
		}
		auto vforward = GetTipReferenceVector();
		vforward.Unitize();
		return (vforward * 20.0f) + base->world.translate;
	}

	RE::NiPoint3 NodeData::SchlongData::GetTipReferenceVector() const
	{
		assert(base);
		const auto& refpoint = base->world.translate;
		if (mid) {
			return mid->world.translate - refpoint;
		} else if (tip) {
			return mid->world.translate - refpoint;
		}
		auto translate = rot * base->world.rotate;
		return translate.GetVectorY();
	}

	float GetVectorAngle(const RE::NiPoint3& v1, const RE::NiPoint3& v2)
	{
		const auto dot = v1.Dot(v2);
		const auto l = v1.Length() * v2.Length();
		const auto x = dot / l;
		return RE::rad_to_deg(std::acosf(x));
	}

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

}
