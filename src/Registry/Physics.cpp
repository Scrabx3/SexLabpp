#include "Physics.h"

namespace Registry
{
	Physics::Position::Nodes::Nodes(const RE::Actor* a_actor)
	{
		const auto obj = a_actor->Get3D();
		if (!obj) {
			const auto msg = fmt::format("Unable to retrieve 3D of actor {:X}", a_actor->GetFormID());
			throw std::exception(msg.c_str());
		}

		head = RE::NiPointer{ obj->GetObjectByName(HEAD) };
		if (!head) {
			logger::info("Actor {:X} is missing head node (This may be expected for creature actors)", a_actor->formID);
		}
		pelvis = RE::NiPointer{ obj->GetObjectByName(PELVIS) };
		spine_lower = RE::NiPointer{ obj->GetObjectByName(SPINELOWER) };
		if (!pelvis || !spine_lower) {
			throw std::exception("Missing mandatory 3d object (body)");
		}

		hand_left = RE::NiPointer{ obj->GetObjectByName(HANDLEFT) };
		hand_right = RE::NiPointer{ obj->GetObjectByName(HANDRIGHT) };
		foot_left = RE::NiPointer{ obj->GetObjectByName(FOOTLEFT) };
		foot_rigt = RE::NiPointer{ obj->GetObjectByName(FOOTRIGHT) };
		if (!hand_left || !hand_right || !foot_left || !foot_rigt) {
			logger::info("Actor {:X} is missing limb nodes (This may be expected for creature actors)", a_actor->formID);
			// throw std::exception("Missing mandatory 3d object (limbs)");
		}

		clitoris = RE::NiPointer{ obj->GetObjectByName(CLITORIS) };
		vagina_deep = RE::NiPointer{ obj->GetObjectByName(VAGINA) };
		vagina_left = RE::NiPointer{ obj->GetObjectByName(VAGINALLEFT) };
		vagina_right = RE::NiPointer{ obj->GetObjectByName(VAGINALRIGHT) };
		anal_deep = RE::NiPointer{ obj->GetObjectByName(ANAL) };
		anal_left = RE::NiPointer{ obj->GetObjectByName(ANALLEFT) };
		anal_right = RE::NiPointer{ obj->GetObjectByName(ANALRIGHT) };

		const auto findschlong = [&]<typename T>(const T& a_list) -> RE::NiAVObject* {
			for (auto&& it : a_list) {
				if (const auto ret = obj->GetObjectByName(it))
					return ret;
			}
			return nullptr;
		};
		// if (a_alternatenodes) {
		sos_base = RE::NiPointer{ findschlong(SOSSTART_ALT) };
		sos_mid = RE::NiPointer{ findschlong(SOSMID_ALT) };
		sos_front = RE::NiPointer{ findschlong(SOSTIP_ALT) };
		// } else {
		// 	sos_base = RE::NiPointer{ findschlong(SOSSTART) };
		// 	sos_mid = RE::NiPointer{ findschlong(SOSMID) };
		// 	sos_front = RE::NiPointer{ findschlong(SOSTIP) };
		// }
	}

	Physics::Position::Position(RE::Actor* a_owner) :
		_owner(a_owner->GetFormID()), _sex(Registry::GetSex(a_owner)), _nodes(a_owner) {}

	Physics::PhysicsData::PhysicsData(std::vector<RE::Actor*> a_positions) :
		_positions([&]() {
			std::vector<Physics::Position> v{};
			v.reserve(a_positions.size());
			for (auto&& it : a_positions) {
				v.emplace_back(it);
			}
			return v;
		}()),
		_tactive(true), _t(&Physics::PhysicsData::Update, this) {}

	Physics::PhysicsData::~PhysicsData()
  {
    _tactive = false;
    _t.join();
  }

	void Physics::PhysicsData::Update()
	{
		while (_tactive) {
			// TODO: Implement
		}
	}

	void Physics::Register(RE::FormID a_id, std::vector<RE::Actor*> a_positions) noexcept
	{
		try {
			auto process = std::make_unique<PhysicsData>(a_positions);
			_data.emplace_back(a_id, std::move(process));
		} catch (const std::exception& e) {
			logger::error("Cannot register sound processing unit, Error: {}", e.what());
		}
	}

	void Physics::Unregister(RE::FormID a_id) noexcept
	{
		const auto where = std::ranges::find(_data, a_id, [](auto& it) { return it.first; });
		if (where == _data.end()) {
			logger::error("No object registered using ID {:X}", a_id);
			return;
		}
		_data.erase(where);
	}

}	 // namespace Registry