#include "NiPosition.h"

#include "Registry/NiNode/NiMath.h"
// #include "Registry/NiNode/NodeUpdate.h"
#include "Registry/Util/Premutation.h"
#include "Registry/Util/RayCast/ObjectBound.h"

namespace Registry::NiNode
{
	bool RotateNode(RE::NiPointer<RE::NiNode> niNode, const NiMath::Segment& sNode, const RE::NiPoint3& pTarget, float maxAngleAdjust)
	{
		maxAngleAdjust = glm::radians(maxAngleAdjust);
		auto& local = niNode->local.rotate;
		const auto vTarget = pTarget - sNode.first;
		Eigen::Vector3f s = NiMath::ToEigen(sNode.Vector()).normalized();
		Eigen::Vector3f v = NiMath::ToEigen(vTarget).normalized();

		const Eigen::Quaternionf worldQuat(NiMath::ToEigen(niNode->world.rotate));
		const Eigen::Quaternionf localQuat(NiMath::ToEigen(local));
		auto tmpQuat = worldQuat.conjugate() * localQuat;

		float cos_angle = std::clamp(s.dot(v), -1.0f, 1.0f);
		float angle = std::acos(cos_angle);
		if (angle < FLT_EPSILON) {
			return true;
		}
		if (angle > maxAngleAdjust) {
			return false;
		}
		auto rotation_axis = s.cross(v);
		if (rotation_axis.norm() > FLT_EPSILON) {
			rotation_axis.normalize();
			angle = std::min(angle, maxAngleAdjust);
			const auto rotation = Eigen::AngleAxisf{ angle, rotation_axis };
			Eigen::Quaternionf rotation_quat{ rotation.inverse() };
			tmpQuat = rotation_quat * tmpQuat;
		}

		Eigen::Quaternionf resQuat = worldQuat * tmpQuat;
		local = NiMath::ToNiMatrix(resQuat.toRotationMatrix());

		RE::NiUpdateData data{ 0.5f, RE::NiUpdateData::Flag::kNone };
		niNode->Update(data);
		return true;
	}

	NiPosition::Snapshot::Snapshot(NiPosition& a_position) :
		position(a_position),
		bHead([&]() {
			const auto nihead = a_position.nodes.head.get();
			if (!nihead)
				return ObjectBound{};
			auto ret = ObjectBound::MakeBoundingBox(nihead);
			return ret ? *ret : ObjectBound{};
		}())
	{}

	bool NiPosition::Snapshot::GetHeadHeadInteractions(const Snapshot& a_partner)
	{
		const auto mouthstart = GetMouthStartPoint();
		const auto partnermouthstart = a_partner.GetMouthStartPoint();
		if (!mouthstart || !partnermouthstart)
			return false;
		const auto distance = mouthstart->GetDistance(*partnermouthstart);
		if (distance > Settings::fDistanceMouthMouth)
			return false;
		const auto vMyHead = *mouthstart - position.nodes.head->world.translate;
		const auto vPartnerHead = *partnermouthstart - a_partner.position.nodes.head->world.translate;
		auto angle = NiMath::GetAngleDegree(vMyHead, vPartnerHead);
		if (std::abs(angle - 180) > Settings::fAngleMouthMouth) {
			return false;
		}
		interactions.emplace_back(a_partner.position.actor, Interaction::Action::Kissing, distance);
		return true;
	}

	// void Position::Snapshot::GetHeadVaginaInteractions(const Snapshot& a_partner)
	// {
	// 	if (!bHead.IsValid())
	// 		return;
	// 	if (a_partner.position.sex.none(Sex::Female, Sex::Futa))
	// 		return;
	// 	assert(position.nodes.head);
	// 	auto& headworld = position.nodes.head->world;
	// 	const auto mouthstart = GetHeadForwardPoint(bHead.boundMax.y);
	// 	const auto vMyHead = *mouthstart - headworld.translate;
	// 	auto& partnernodes = a_partner.position.nodes;
	// 	auto vVaginal = partnernodes.GetVaginalVector();
	// 	if (!vVaginal || !partnernodes.clitoris || !mouthstart)
	// 		return;
	// 	if (!bHead.IsPointInside(partnernodes.clitoris->world.translate))
	// 		return;
	// 	auto angle = NiMath::GetAngleDegree(*vVaginal, vMyHead);
	// 	if ((angle - 180) > Settings::fAngleMouth * 2)
	// 		return;
	// 	float distance = partnernodes.clitoris->world.translate.GetDistance(*mouthstart);
	// 	interactions.emplace_back(a_partner.position.actor, Interaction::Action::Oral, distance);
	// }

	bool NiPosition::Snapshot::GetHeadPenisInteractions(const Snapshot& a_partner, std::shared_ptr<Node::NodeData::Schlong> a_schlong)
	{
		if (!bHead.IsValid()) {
			return false;
		}
		assert(position.nodes.head);
		const auto& headworld = position.nodes.head->world;
		const auto sSchlong = a_schlong->GetReferenceSegment();
		const auto dCenter = [&]() {
			auto res = NiMath::ClosestSegmentBetweenSegments({ headworld.translate }, sSchlong);
			return res.Length();
		}();
		if (dCenter > bHead.boundMax.y * Settings::fThroatToleranceRadius) {
			return false;
		}
		const auto& partnernodes = a_partner.position.nodes;
		const auto baseNode = a_schlong->GetBaseReferenceNode();
		const auto vHead = headworld.rotate.GetVectorY();

		const auto [angleToHead, angleToMouth, angleToBase] = [&]() {
			const auto vBaseToHead = headworld.translate - sSchlong.first;
			const auto vPartnerDir = partnernodes.GetCrotchSegment().Vector();
			const auto proj1 = NiMath::ProjectedComponent(vPartnerDir, vHead);
			const auto proj2 = NiMath::ProjectedComponent(vBaseToHead, vHead);
			return std::make_tuple(
				NiMath::GetAngleDegree(proj1, proj2),
				NiMath::GetAngleDegree(proj1, -vHead),
				NiMath::GetAngleDegree(proj2, vHead));
		}();

		const auto aiming_at_head = std::abs(angleToHead - angleToMouth) < Settings::fAngleToHeadTolerance;
		const auto at_side_of_head = std::abs(angleToBase - 90) < Settings::fAngleToHeadSidewaysTolerance;
		const auto in_front_of_head = std::abs(angleToBase - 180) < Settings::fAngleToHeadFrontalTolerance;
		const auto penetrating_skull = dCenter < (at_side_of_head ? bHead.boundMax.x : bHead.boundMax.y);
		const auto vertical_to_shaft = [&]() {
			const auto vSchlong = sSchlong.Vector();
			const auto aSchlongToMouth = NiMath::GetAngleDegree(vSchlong, vHead);
			return std::abs(aSchlongToMouth - 90) < 30.0f;
		}();
		const auto close_to_mouth = [&]() {
			const auto pMouth = GetMouthStartPoint();
			assert(pMouth);
			const auto seg = NiMath::ClosestSegmentBetweenSegments({ *pMouth }, sSchlong);
			const auto d = seg.Length();
			return d < bHead.boundMax.x && d < dCenter;
		}();

		if (in_front_of_head && vertical_to_shaft && close_to_mouth) {
			interactions.emplace_back(a_partner.position.actor, Interaction::Action::LickingShaft, sSchlong.second);
			return true;
		} else if (penetrating_skull && in_front_of_head && aiming_at_head) {
			const auto throat = GetThroatPoint(), mouth = GetMouthStartPoint();
			assert(throat && mouth);
			if (!baseNode || RotateNode(baseNode, sSchlong, *throat, Settings::fAdjustSchlongLimit)) {
				RotateNode(position.nodes.head, { *mouth, *throat }, sSchlong.first, Settings::fAdjustHeadLimit);
				interactions.emplace_back(a_partner.position.actor, Interaction::Action::Oral, sSchlong.second);
				assert(partnernodes.pelvis);
				const auto tip_at_throat = dCenter < bHead.boundMax.y * Settings::fThroatToleranceRadius;
				const auto pelvis_at_head = bHead.IsPointInside(partnernodes.pelvis->world.translate);
				if (tip_at_throat || pelvis_at_head) {
					interactions.emplace_back(a_partner.position.actor, Interaction::Action::Deepthroat, sSchlong.second);
				}
				return true;
			}
		} else if (penetrating_skull && aiming_at_head) {
			if (!baseNode || RotateNode(baseNode, sSchlong, headworld.translate, Settings::fAdjustSchlongLimit)) {
				interactions.emplace_back(a_partner.position.actor, Interaction::Action::Skullfuck, sSchlong.second);
			}
			return true;
		} else if (in_front_of_head && aiming_at_head) {
			interactions.emplace_back(a_partner.position.actor, Interaction::Action::Facial, sSchlong.second);
			return true;
		}
		return false;
	}

	bool NiPosition::Snapshot::GetHandPenisInteractions(const Snapshot& a_partner, std::shared_ptr<Node::NodeData::Schlong> a_schlong)
	{
		const auto lHand = position.nodes.hand_left;
		const auto rHand = position.nodes.hand_right;
		if (!lHand || !rHand) {
			return false;
		}
		const auto sSchlong = a_schlong->GetReferenceSegment();
		const auto pLeft = lHand->world.translate;
		const auto pRight = rHand->world.translate;
		const auto rDist = NiMath::ClosestSegmentBetweenSegments(pLeft, sSchlong);
		const auto lDist = NiMath::ClosestSegmentBetweenSegments(pLeft, sSchlong);
		const auto closeToL = lDist.Length() < Settings::fDistanceHand;
		const auto closeToR = rDist.Length() < Settings::fDistanceHand;
		RE::NiPoint3 referencePoint;
		// TODO: Want the palm here, not center of hand
		if (closeToR && closeToL) {	 // Both hands are close, pick the closest to the base
			const auto nSchlong = a_schlong->GetBaseReferenceNode();
			if (nSchlong && nSchlong->world.translate.GetDistance(pLeft) < nSchlong->world.translate.GetDistance(pRight)) {
				referencePoint = pLeft;
			} else {
				referencePoint = pRight;
			}
		} else if (closeToR) {
			referencePoint = pRight;
		} else if (closeToL) {
			referencePoint = pLeft;
		} else {
			return false;
		}
		RotateNode(a_schlong->GetBaseReferenceNode(), sSchlong, referencePoint, Settings::fAdjustSchlongLimit);
		return true;
	}

	// void Position::Snapshot::GetHeadAnimObjInteractions(const Snapshot& a_other)
	// {
	// 	bool out;
	// 	a_other.position.actor->GetGraphVariableBool("bAnimObjectLoaded", out);
	// 	if (!out) {
	// 		return;
	// 	}
	// 	const auto point = GetHeadForwardPoint(bHead.boundMax.y);
	// 	if (!point)
	// 		return;
	// 	const auto getimpl = [&](auto pos) {
	// 		if (!pos)
	// 			return;
	// 		const auto d = pos->world.translate.GetDistance(*point);
	// 		if (d > Settings::fAnimObjDist)
	// 			return;
	// 		interactions.emplace_back(a_other.position.actor, Interaction::Action::AnimObjFace, d);
	// 	};
	// 	const auto& n = a_other.position.nodes;
	// 	getimpl(n.animobj_a);
	// 	getimpl(n.animobj_b);
	// 	getimpl(n.animobj_r);
	// 	getimpl(n.animobj_l);
	// }

	bool NiPosition::Snapshot::GetCrotchPenisInteractions(const Snapshot& a_partner, std::shared_ptr<Node::NodeData::Schlong> a_schlong)
	{
		const auto sSchlong = a_schlong->GetReferenceSegment();
		const auto nSchlong = a_schlong->GetBaseReferenceNode();
		const auto sVaginal = position.nodes.GetVaginalSegment();
		const auto sAnal = position.nodes.GetAnalSegment();
		const auto& nClitoris = position.nodes.clitoris;
		if (sVaginal && sAnal && nClitoris) {	 // 3BA & female
			const auto [type, segment, distance] = [&]() {
				enum
				{
					tNone,
					tVaginal,
					tAnal
				};
				const auto tLast = [&] {
					const auto where = std::ranges::find_if(position.interactions, [&](const Interaction& it) {
						return it.partner == a_partner.position.actor && (it.action == Interaction::Action::Vaginal || it.action == Interaction::Action::Anal);
					});
					if (where == position.interactions.end()) {
						return tNone;
					} else if (where->action == Interaction::Action::Vaginal) {
						return tVaginal;
					} else {
						return tAnal;
					}
				}();
				const auto dVaginal = NiMath::ClosestSegmentBetweenSegments(sSchlong, sVaginal->first).Length();
				const auto dAnal = NiMath::ClosestSegmentBetweenSegments(sSchlong, sAnal->first).Length();
				const auto dif = dVaginal - dAnal;
				bool branchVaginal = true;
				switch (tLast) {
				case tVaginal:
					branchVaginal = dif < Settings::fPenetrationVaginalToleranceRepeat;
					break;
				case tAnal:
					branchVaginal = dif < -Settings::fPenetrationAnalToleranceRepeat;
					break;
				default:
					branchVaginal = dif < Settings::fPenetrationVaginalTolerance;
					break;
				}
				// Giving Vaginal a slight preference as most animations
				// where it is "unclear" are usually intended to be vaginal
				if (branchVaginal) {
					return std::tuple{
						Interaction::Action::Vaginal,
						*sVaginal,
						dVaginal
					};
				} else {
					return std::tuple{
						Interaction::Action::Anal,
						*sAnal,
						dAnal
					};
				}
			}();
			if (distance <= Settings::fDistanceCrotch) {
				const auto aSegment = NiMath::GetAngleDegree(segment.Vector(), sSchlong.Vector());
				if (aSegment <= Settings::fAnglePenetration && (!nSchlong || RotateNode(nSchlong, sSchlong, segment.second, Settings::fAdjustSchlongVaginalLimit))) {
					interactions.emplace_back(a_partner.position.actor, type, sSchlong.second);
					return true;
				}
				const auto sCrotch = NiMath::Segment{ sAnal->first, sVaginal->first };
				const auto aCrotch = NiMath::GetAngleDegree(sCrotch.Vector(), sSchlong.Vector());
				if (std::abs(aCrotch - 180.0f) <= Settings::fAngleGrinding) {
					interactions.emplace_back(a_partner.position.actor, Interaction::Action::Grinding, sSchlong.second);
					return true;
				}
			}
		} else {	// male | no 3BA
			const auto sCrotch = position.nodes.GetCrotchSegment();
			const auto dCrotch = NiMath::ClosestSegmentBetweenSegments(sCrotch, sSchlong).Length();
			if (dCrotch <= Settings::fDistanceCrotch) {
				const auto vBaseToSpine = sCrotch.first - sSchlong.first;
				const auto aCrotch = NiMath::GetAngleDegree(vBaseToSpine, sSchlong.Vector());
				if (aCrotch <= Settings::fAnglePenetration && (!nSchlong || RotateNode(nSchlong, sSchlong, sCrotch.first, Settings::fAdjustSchlongVaginalLimit))) {
					interactions.emplace_back(a_partner.position.actor, Interaction::Action::Anal, sSchlong.second);
					return true;
				} else if (std::abs(aCrotch - 90.0f) <= Settings::fAngleGrinding) {
					interactions.emplace_back(a_partner.position.actor, Interaction::Action::Anal, sSchlong.second);
					return true;
				}
			}
		}
		return false;
	}

	// void Position::Snapshot::GetVaginaVaginaInteractions(const Snapshot& a_other)
	// {
	// 	if (position.sex.none(Sex::Female, Sex::Futa) || a_other.position.sex.none(Sex::Female, Sex::Futa))
	// 		return;
	// 	const auto &c1 = position.nodes.clitoris, &c2 = a_other.position.nodes.clitoris;
	// 	if (!c1 || !c2)
	// 		return;
	// 	const auto distance = c1->world.translate.GetDistance(c2->world.translate);
	// 	if (distance > Settings::fDistanceCrotch)
	// 		return;
	// 	auto vVaginal = position.nodes.GetVaginalVector();
	// 	auto vVaginalPartner = a_other.position.nodes.GetVaginalVector();
	// 	if (!vVaginal || !vVaginalPartner)
	// 		return;
	// 	const auto angle = NiMath::GetAngleDegree(*vVaginal, *vVaginalPartner);
	// 	if (std::abs(angle - 180) > Settings::fAngleGrinding)
	// 		return;
	// 	interactions.emplace_back(a_other.position.actor, Interaction::Action::Grinding, distance);
	// }

	// void Position::Snapshot::GetGenitalLimbInteractions(const Snapshot& a_other)
	// {
	// 	const auto& othernodes = a_other.position.nodes;
	// 	const auto impl = [&](const decltype(othernodes.hand_left)& activePoint, auto action) {
	// 		const auto make = [&](RE::NiPoint3 refPoint) {
	// 			const auto d = activePoint->world.translate.GetDistance(refPoint);
	// 			if (d > Settings::fDistanceHand)
	// 				return false;
	// 			interactions.emplace_back(a_other.position.actor, action, d);
	// 			return true;
	// 		};
	// 		if (!activePoint)
	// 			return false;
	// 		for (auto&& p : position.nodes.schlongs) {
	// 			const auto& refpoint = p->GetSchlongVector();
	// 			if (make(refpoint))
	// 				return true;
	// 		}
	// 		if (const auto& c = position.nodes.clitoris) {
	// 			if (make(c->world.translate))
	// 				return true;
	// 		}
	// 		return false;
	// 	};
	// 	for (auto&& node : { othernodes.hand_left, othernodes.hand_right }) {
	// 		if (impl(node, Interaction::Action::HandJob))
	// 			break;
	// 	}
	// 	for (auto&& node : { othernodes.foot_left, othernodes.foot_right }) {
	// 		if (impl(node, Interaction::Action::FootJob))
	// 			break;
	// 	}
	// }

	std::optional<RE::NiPoint3> NiPosition::Snapshot::GetMouthStartPoint() const
	{
		auto ret = GetThroatPoint();
		if (!ret) {
			return std::nullopt;
		}
		const auto& nihead = position.nodes.head;
		assert(nihead);
		const auto distforward = bHead.boundMax.y * 0.88f;
		const auto vforward = nihead->world.rotate.GetVectorY();
		return (vforward * distforward) + *ret;
	}

	std::optional<RE::NiPoint3> NiPosition::Snapshot::GetThroatPoint() const
	{
		if (!bHead.IsValid()) {
			return std::nullopt;
		}
		const auto& nihead = position.nodes.head;
		assert(nihead);
		const auto distdown = bHead.boundMin.z * 0.17f;
		const auto vup = nihead->world.rotate.GetVectorZ();
		return (vup * distdown) + nihead->world.translate;
	}

}	 // namespace Registry::NiNode
