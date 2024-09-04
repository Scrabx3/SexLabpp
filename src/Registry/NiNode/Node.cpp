#include "Node.h"

#include "NiMath.h"
#include "Registry/Define/RaceKey.h"
#include "Registry/Define/Transform.h"

namespace Registry::Collision::Node
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
			auto data = SchlongData::CreateSchlongData(obj, it.base, it.rot);
			if (!data)
				continue;
			auto ptr = std::make_shared<SchlongData>(*data);
			schlongs.push_back(ptr);
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
			auto it = s->GetTipReferencePoint();
			ret.push_back(it);
		}
		if (ret.empty() && a_approximateifempty)
			ret.push_back(ApproximateTip());
		return ret;
	}

	std::vector<RE::NiPoint3> NodeData::GetSchlongReferenceVectors(bool a_approximateifempty) const
	{
		std::vector<RE::NiPoint3> ret{};
		for (auto&& s : schlongs) {
			auto it = s->GetSchlongVector();
			ret.push_back(it);
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

	std::optional<NodeData::SchlongData> NodeData::SchlongData::CreateSchlongData(RE::NiAVObject* a_root, std::string_view a_basenode, const glm::mat3& a_rot)
	{
		auto obj = a_root->GetObjectByName(a_basenode);
		auto niobj = obj ? obj->AsNode() : nullptr;
		if (!niobj)
			return std::nullopt;
		return SchlongData(RE::NiPointer{ niobj }, a_rot);
	}

	NodeData::SchlongData::SchlongData(RE::NiPointer<RE::NiNode> a_basenode, const glm::mat3& a_rot) :
		rot({ a_rot[0].x, a_rot[0].y, a_rot[0].z }, { a_rot[1].x, a_rot[1].y, a_rot[1].z }, { a_rot[2].x, a_rot[2].y, a_rot[2].z }),
		nodes({ a_basenode })
	{
		assert(a_basenode);
		do {
			auto& parent = nodes.back();
			auto& childs = parent->children;
			switch (childs.size()) {
			case 0:
				break;
			case 1:
				{
					auto& child = childs.front();
					auto niobj = child ? child->AsNode() : nullptr;
					if (niobj) {
						nodes.emplace_back(niobj);
					}
				}
				break;
			default:
				{
					auto v1 = nodes.size() < 2 ? parent->world.rotate.GetVectorY() : parent->world.translate - a_basenode->world.translate;
					if (v1.SqrLength() > FLT_EPSILON) {
						for (auto&& child : childs) {
							if (!child)
								continue;
							auto nichild = child->AsNode();
							if (!nichild)
								continue;
							auto v2 = nichild->world.translate - a_basenode->world.translate;
							auto angle = NiMath::GetAngleDegree(v1, v2);
							if (angle <= 90.0f) {
								nodes.emplace_back(nichild);
								break;
							}
						}
					} else {
						auto user = a_basenode->GetUserData();
						auto id = user ? user->GetFormID() : 0;
						logger::error("Ambiguous Skeleton Structure for user {:X} at node depth {}", id, nodes.size());
					}
				}
				break;
			}
			if (nodes.back() == parent)
				break;
		} while (true);
	}

	RE::NiPointer<RE::NiNode> NodeData::SchlongData::GetBaseReferenceNode() const
	{
		switch (nodes.size()) {
		case 0:
			assert(false);
			return nullptr;
		default:
			return nodes.front();
		}
	}

	RE::NiPoint3 NodeData::SchlongData::GetTipReferencePoint() const
	{
		switch (nodes.size()) {
		case 0:
			assert(false);
			return RE::NiPoint3::Zero();
		case 1:
			{
				auto vforward = GetSchlongVector();
				vforward.Unitize();
				return (vforward * MIN_SCHLONG_LEN) + nodes.front()->world.translate;
			}
		default:
			return nodes.back()->world.translate;
		}
	}

	RE::NiPoint3 NodeData::SchlongData::GetSchlongVector() const
	{
		switch (nodes.size()) {
		case 0:
			assert(false);
			return RE::NiPoint3::Zero();
		case 1:
			{
				auto translate = rot * nodes.front()->world.rotate;
				return translate.GetVectorY() * MIN_SCHLONG_LEN;
			}
		default:
			{
				std::vector<Eigen::Vector3f> argV{};
				argV.reserve(nodes.size());
				for (auto&& node : nodes) {
					if (!node)
						continue;
					auto argT = NiMath::ToEigen(node->world.translate);
					argV.push_back(argT);
				}
				auto seg = NiMath::LeastSquares(argV, MIN_SCHLONG_LEN);
				return seg.second - seg.first;
			}
			// return nodes.back()->world.translate - nodes.front()->world.translate;
		}
	}
}
