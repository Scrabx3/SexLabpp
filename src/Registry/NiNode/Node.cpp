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
		get(HANDRIGHT, hand_right, true);
		get(FOOTLEFT, foot_left, true);
		get(FOOTRIGHT, foot_rigt, true);
		get(CLITORIS, clitoris, true);
		get(VAGINADEEP, vaginadeep, true);
		get(VAGINALLEFT, vaginaleft, true);
		get(VAGINALRIGHT, vaginaright, true);
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

		auto vagmid = (vaginaleft->world.translate + vaginaright->world.translate) / 2;
		return vagmid - vaginadeep->world.translate;
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

}
