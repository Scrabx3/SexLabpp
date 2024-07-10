#include "Physics.h"

#include "Util/Premutation.h"
#include "Util/RayCast/ObjectBound.h"
#include "Registry/Node.h"

using namespace Registry::Node;	// TODO: remove this <-

namespace Registry
{
	Physics::Position::Nodes::Nodes(const RE::Actor* a_actor, bool a_alternatenodes)
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
		}
		clitoris = RE::NiPointer{ obj->GetObjectByName(CLITORIS) };
		const auto findschlong = [&]<typename T>(const T& a_list) -> RE::NiAVObject* {
			for (auto&& it : a_list) {
				if (const auto ret = obj->GetObjectByName(it))
					return ret;
			}
			return nullptr;
		};
		if (a_alternatenodes) {
			sos_base = RE::NiPointer{ findschlong(SOSSTART_ALT) };
			sos_mid = RE::NiPointer{ findschlong(SOSMID_ALT) };
			sos_front = RE::NiPointer{ findschlong(SOSTIP_ALT) };
		} else {
			sos_base = RE::NiPointer{ findschlong(SOSSTART) };
			sos_mid = RE::NiPointer{ findschlong(SOSMID) };
			sos_front = RE::NiPointer{ findschlong(SOSTIP) };
		}
	}

	RE::NiPoint3 Physics::Position::Nodes::ApproximateNode(float a_forward, float a_upward) const
	{
		Registry::Coordinate approx(std::vector{ a_forward, 0.0f, a_upward, 0.0f });
		RE::NiPoint3 angle;
		pelvis->world.rotate.ToEulerAnglesXYZ(angle);
		Registry::Coordinate ret{ pelvis->world.translate, angle.z };
		approx.Apply(ret);
		return ret.AsNiPoint();
	}

	RE::NiPoint3 Physics::Position::Nodes::ApproximateTip() const
	{
		constexpr float forward = 20.0f;
		constexpr float upward = -4.0f;
		return ApproximateNode(forward, upward);
	}

	RE::NiPoint3 Physics::Position::Nodes::ApproximateMid() const
	{
		constexpr float forward = 15.0f;
		constexpr float upward = -6.2f;
		return ApproximateNode(forward, upward);
	}

	RE::NiPoint3 Physics::Position::Nodes::ApproximateBase() const
	{
		constexpr float forward = 10.0f;
		constexpr float upward = -5.0f;
		return ApproximateNode(forward, upward);
	}

	Physics::Position::Position(RE::Actor* a_owner, Sex a_sex) :
		_owner(a_owner->GetFormID()), _sex(a_sex), _nodes(a_owner, false), _types({}) {}

	Physics::TypeData* Physics::Position::GetType(TypeData& a_data){
		auto where = std::ranges::find_if(_types, [&](auto& type) {
			return a_data._type == type._type && a_data._partner == type._partner;
		});
		if (where == _types.end()) {
			return nullptr;
		}
		return &(*where);
	}

	Physics::PhysicsData::WorkingData::WorkingData(Position& a_position) :
		_position(a_position),
		pGenitalReference([&]() {
			if (this->vSchlong == RE::NiPoint3::Zero()) {
				if (this->_position._nodes.clitoris)
					return this->_position._nodes.clitoris->world.translate;
			} else {
				return this->_position._nodes.sos_mid ?
								 this->_position._nodes.sos_mid->world.translate :
								 this->_position._nodes.ApproximateMid();
			}
			return RE::NiPoint3::Zero();
		}()),
		vCrotch(a_position._nodes.pelvis->world.translate - a_position._nodes.spine_lower->world.translate),
		vSchlong([&]() {
			const auto& nodes = a_position._nodes;
			if (!nodes.sos_mid) {
				if (a_position._sex.any(Sex::Male, Sex::Futa)) {
					const auto mid = a_position._nodes.ApproximateMid();
					const auto base = a_position._nodes.ApproximateBase();
					return mid - base;
				}
				return RE::NiPoint3::Zero();
			}
			if (nodes.sos_base) {
				return nodes.sos_mid->world.translate - nodes.sos_base->world.translate;
			} else if (nodes.sos_front) {
				return nodes.sos_front->world.translate - nodes.sos_mid->world.translate;
			}
			return RE::NiPoint3::Zero();
		}())
	{
		vCrotch.Unitize();
		vSchlong.Unitize();
	}

	std::optional<Physics::TypeData> Physics::PhysicsData::WorkingData::GetsOral(const WorkingData& a_partner) const
	{
		if (!a_partner._position._nodes.head)
			return std::nullopt;
		const auto& headworld = a_partner._position._nodes.head->world;
		if (pGenitalReference == RE::NiPoint3::Zero())
			return std::nullopt;
		auto distance = pGenitalReference.GetDistance(headworld.translate);
		if (distance > Settings::fDistanceHead)
			return std::nullopt;
		if (vSchlong != RE::NiPoint3::Zero()) {
			const auto vRot = headworld.rotate * vSchlong;
			const auto dot = vRot.Dot(vSchlong);
			const auto angle = RE::rad_to_deg(std::acosf(dot));
			if (angle < (180 - Settings::fAngleMouth)) {
				return std::nullopt;
			}
		}
		TypeData ret{};
		ret._distance = distance;
		ret._partner = a_partner._position._owner;
		ret._type = TypeData::Type::Oral;
		return ret;
	}

	std::optional<Physics::TypeData> Physics::PhysicsData::WorkingData::GetsHandjob(const WorkingData& a_partner) const
	{
		const auto& handL = a_partner._position._nodes.hand_left;
		const auto& handR = a_partner._position._nodes.hand_right;
		if (pGenitalReference == RE::NiPoint3::Zero())
			return std::nullopt;
		const auto make = [&](decltype(handL)& hand) -> std::optional<TypeData> {
			if (!hand)
				return std::nullopt;
			const auto d = hand->world.translate.GetDistance(pGenitalReference);
			if (d > Settings::fDistanceHand)
				return std::nullopt;
			TypeData ret{};
			ret._distance = d;
			ret._partner = a_partner._position._owner;
			ret._type = TypeData::Type::Hand;
			return ret;
		};
		if (auto ret = make(handL))
			return ret;
		if (auto ret = make(handR))
			return ret;
		return std::nullopt;
	}

	std::optional<Physics::TypeData> Physics::PhysicsData::WorkingData::GetsFootjob(const WorkingData& a_partner) const
	{
		const auto& footL = a_partner._position._nodes.foot_left;
		const auto& footR = a_partner._position._nodes.foot_rigt;
		if (pGenitalReference == RE::NiPoint3::Zero())
			return std::nullopt;
		const auto make = [&](decltype(footL)& foot) -> std::optional<TypeData> {
			if (!foot)
				return std::nullopt;
			const auto d = foot->world.translate.GetDistance(pGenitalReference);
			if (d > Settings::fDistanceFoot)
				return std::nullopt;
			TypeData ret{};
			ret._distance = d;
			ret._partner = a_partner._position._owner;
			ret._type = TypeData::Type::Foot;
			return ret;
		};
		if (auto ret = make(footL))
			return ret;
		if (auto ret = make(footR))
			return ret;
		return std::nullopt;
	}

	std::optional<Physics::TypeData> Physics::PhysicsData::WorkingData::DoesGrinidng(const WorkingData& a_partner) const
	{
		const auto& cT = _position._nodes.clitoris;
		if (!cT)
			return std::nullopt;
		float d;
		if (a_partner.vSchlong != RE::NiPoint3::Zero()) {
			const auto& dot = vCrotch.Dot(a_partner.vSchlong);
			const auto deg = RE::rad_to_deg(std::acosf(dot));
			if (deg < (180 - Settings::fAngleGrinding) || deg > Settings::fAngleGrinding) {
				return std::nullopt;
			}
			const auto& sPtr = a_partner._position._nodes.sos_mid;
			const auto refP = sPtr ? sPtr->world.translate : a_partner._position._nodes.ApproximateMid();
			d = cT->world.translate.GetDistance(refP);
		} else {
			const auto& cPtr = a_partner._position._nodes.clitoris;
			if (!cPtr)
				return std::nullopt;
			d = cT->world.translate.GetDistance(cPtr->world.translate);
		}
		if (d > Settings::fDistanceCrotch / 2)
			return std::nullopt;
		TypeData ret{};
		ret._distance = d;
		ret._partner = a_partner._position._owner;
		ret._type = TypeData::Type::Grinding;
		return ret;
	}

	std::optional<Physics::TypeData> Physics::PhysicsData::WorkingData::HasIntercourse(const WorkingData& a_partner) const
	{
		if (a_partner.vSchlong == RE::NiPoint3::Zero()) {
			return std::nullopt;
		}
		const auto dot = vCrotch.Dot(a_partner.vSchlong);
		const auto deg = RE::rad_to_deg(std::acosf(dot));
		if (deg < (90 - Settings::fAnglePenetration) || deg > (90 + Settings::fAnglePenetration)) {
			return std::nullopt;
		}
		assert(_position._nodes.pelvis && _position._nodes.spine_lower);
		const auto& refTA = _position._nodes.spine_lower->world.translate;
		const auto& refP = a_partner._position._nodes.sos_mid ?
												 a_partner._position._nodes.sos_mid->world.translate :
												 a_partner._position._nodes.ApproximateMid();
		const auto dA = refTA.GetDistance(refP);
		if (dA > Settings::fDistanceCrotch) {
			return std::nullopt;
		}
		TypeData ret{};
		ret._partner = a_partner._position._owner;
		if (_position._sex != Sex::Male) {
			const auto& refTV = _position._nodes.pelvis->world.translate;
			const auto dV = refTV.GetDistance(refP);
			if (dV < Settings::fDistanceCrotch) {
				ret._distance = dV < dA ? dV : dA;
				ret._type = dV < dA ? TypeData::Type::VaginalP : TypeData::Type::AnalP;
				return ret;
			}
		}
		ret._distance = dA;
		ret._type = TypeData::Type::AnalP;
		return ret;
	}

	Physics::PhysicsData::PhysicsData(std::vector<RE::Actor*> a_positions, const Scene* a_scene) :
		_positions([&]() {
			std::vector<Physics::Position> v{};
			v.reserve(a_positions.size());
			for (size_t i = 0; i < a_positions.size(); i++) {
				auto& it = a_positions[i];
				auto sex = a_scene->GetNthPosition(i)->sex.get();
				v.emplace_back(it, sex);
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
		constexpr auto interval = 128ms;
		const auto main = RE::Main::GetSingleton();
		while (_tactive) {
			if (!main->gameActive) {
				std::this_thread::sleep_for(interval * 2);
				continue;
			}
			const auto update = [&interval](WorkingData& data, std::optional<TypeData> a_type) {
				if (!a_type)
					return a_type;
				auto stored = data._position.GetType(*a_type);
				if (stored) {
					const float delta_dist = a_type->_distance - stored->_distance;
					a_type->_velocity = (stored->_velocity + (delta_dist / interval.count())) / 2;
				} else {
					a_type->_velocity = 0.0f;
				}
				data.types.push_back(*a_type);
				return a_type;
			};
			if (_positions.size() == 1) {
				WorkingData data{ _positions[0] };
				update(data, data.GetsHandjob(data));
				_positions[0]._types = data.types;
			} else {
				std::vector<std::shared_ptr<WorkingData>> snapshots{};
				snapshots.reserve(_positions.size());
				for (auto&& it : _positions) {
					auto& obj = snapshots.emplace_back(std::make_shared<WorkingData>(it));
					update(*obj, obj->GetsHandjob(*obj));
				}
				assert(_positions.size() == snapshots.size());
				Combinatorics::for_each_permutation(snapshots.begin(), snapshots.begin() + 2, snapshots.end(),
					[&](auto start, [[maybe_unused]] auto end) {
						assert(std::distance(start, end) == 2);
						auto& fst = **start;
						auto& snd = **(start + 1);
						update(fst, fst.GetsOral(snd));
						update(fst, fst.GetsHandjob(snd));
						update(fst, fst.GetsFootjob(snd));
						update(fst, fst.DoesGrinidng(snd));
						if (auto type = update(fst, fst.HasIntercourse(snd))) {
							TypeData mirror = *type;
							mirror._type = type->_type == TypeData::Type::VaginalP ? TypeData::Type::VaginalA : TypeData::Type::AnalA;
							snd.types.push_back(mirror);
						}
						return false;
					});
				for (size_t i = 0; i < _positions.size(); i++) {
					_positions[i]._types = snapshots[i]->types;
				}
			}
			std::this_thread::sleep_for(interval);
		}
	}

	void Physics::Register(RE::FormID a_id, std::vector<RE::Actor*> a_positions, const Scene* a_scene) noexcept
	{
		try {
			const auto where = std::ranges::find(_data, a_id, [](auto& it) { return it.first; });
			if (where != _data.end()) {
				_data.erase(where);
			}
			auto process = std::make_unique<PhysicsData>(a_positions, a_scene);
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

	bool Physics::IsRegistered(RE::FormID a_id) const noexcept
	{
		return std::ranges::contains(_data, a_id, [](auto& it) { return it.first; });
	}

	const Physics::PhysicsData* Physics::GetData(RE::FormID a_id) const
	{
		const auto where = std::ranges::find(_data, a_id, [](auto& it) { return it.first; });
		return where == _data.end() ? nullptr : where->second.get();
	}

}	 // namespace Registry
