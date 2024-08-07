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
		if (racekey == RaceKey::None) {
			const auto msg = fmt::format("Unable to retrieve race of actor {:X}", a_actor->GetFormID());
			throw std::exception(msg.c_str());
		}
		const auto racestr = RaceHandler::AsString(racekey);
		const auto get = [&](auto str, auto& target, bool log) {
			auto node = obj->GetObjectByName(str);
			auto ninode = node ? node->AsNode() : nullptr;
			if (!ninode) {
				if (log)
					logger::info("Actor {:X} (Race: {}) is missing Node {} (This may be expected)", a_actor->GetFormID(), racestr, str);
				return;
			}
			target = RE::NiPointer{ ninode };
		};
		get(PELVIS, pelvis, true);
		get(SPINELOWER, spine_lower, true);
		if (!pelvis || !spine_lower) {
			throw std::exception("Missing mandatory 3d object (body)");
		}
		get(HEAD, head, true);
		get(HANDLEFT, hand_left, true);
		get(HANDRIGHT, hand_right, true);
		get(FOOTLEFT, foot_left, true);
		get(FOOTRIGHT, foot_rigt, true);
		get(CLITORIS, clitoris, true);
		const auto schlong = [&]<typename T>(const T& a_list, auto& out) {
			decltype(head) tmp;
			for (auto&& it : a_list) {
				get(it, tmp, false);
				if (tmp) {
					out.push_back(tmp);
				}
			}
		};
		schlong(SOSSTART, sos_base);
		schlong(SOSMID, sos_mid);
		schlong(SOSTIP, sos_front);
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
}
