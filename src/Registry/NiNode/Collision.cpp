#include "Collision.h"

#include "Registry/Util/Premutation.h"
#include "Registry/Util/RayCast/ObjectBound.h"

namespace Registry::Collision
{

	Position::Snapshot::Snapshot(Position& a_position) :
		position(a_position),
		bHead([&]() {
			const auto nihead = a_position.nodes.head.get();
			if (!nihead)
				return ObjectBound{};
			auto ret = ObjectBound::MakeBoundingBox(nihead);
			return ret ? *ret : ObjectBound{};
		}()),
		vCrotch(a_position.nodes.pelvis->world.translate - a_position.nodes.spine_lower->world.translate)
	{
		vCrotch.Unitize();
	}

	std::optional<Collision::TypeData> Collision::PhysicsData::WorkingData::GetHeadInteraction(const WorkingData& a_partner) const
	{
		if (!a_partner._position._nodes.head)
			return std::nullopt;
		const auto& headworld = a_partner._position._nodes.head->world;
		if (pGenitalReference == RE::NiPoint3::Zero())
			return std::nullopt;
		auto distance = pGenitalReference.GetDistance(headworld.translate);
		if (distance > a_partner.GetHeadForwardDistance())
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

	std::optional<Collision::TypeData> Collision::PhysicsData::WorkingData::GetsOral(const WorkingData& a_partner) const
	{
		if (!a_partner._position._nodes.head)
			return std::nullopt;
		const auto& headworld = a_partner._position._nodes.head->world;
		if (pGenitalReference == RE::NiPoint3::Zero())
			return std::nullopt;
		auto distance = pGenitalReference.GetDistance(headworld.translate);
		if (distance > a_partner.GetHeadForwardDistance())
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

	std::optional<Collision::TypeData> Collision::PhysicsData::WorkingData::GetsHandjob(const WorkingData& a_partner) const
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

	std::optional<Collision::TypeData> Collision::PhysicsData::WorkingData::GetsFootjob(const WorkingData& a_partner) const
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

	std::optional<Collision::TypeData> Collision::PhysicsData::WorkingData::DoesGrinidng(const WorkingData& a_partner) const
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

	std::optional<Collision::TypeData> Collision::PhysicsData::WorkingData::HasIntercourse(const WorkingData& a_partner) const
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

	std::optional<RE::NiPoint3> Collision::PhysicsData::WorkingData::GetHeadForwardPoint(float distance) const
	{
		const auto& nihead = _position._nodes.head;
		if (!nihead)
			return std::nullopt;
		const auto& headworld = nihead->world;
		RE::NiPoint3 vforward{ headworld.rotate.entry[1][0], headworld.rotate.entry[1][1], headworld.rotate.entry[1][2] };
		return (vforward * distance) + nihead->world.translate;
	}

	Collision::PhysicsData::PhysicsData(std::vector<RE::Actor*> a_positions, const Scene* a_scene) :
		_positions([&]() {
			std::vector<Collision::Position> v{};
			v.reserve(a_positions.size());
			for (size_t i = 0; i < a_positions.size(); i++) {
				auto& it = a_positions[i];
				auto sex = a_scene->GetNthPosition(i)->sex.get();
				v.emplace_back(it, sex);
			}
			return v;
		}()),
		_tactive(true), _t(&Collision::PhysicsData::Update, this) {}

	Collision::PhysicsData::~PhysicsData()
	{
		_tactive = false;
		_t.join();
	}

	void Collision::PhysicsData::Update()
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

	void Collision::Register(RE::FormID a_id, std::vector<RE::Actor*> a_positions, const Scene* a_scene) noexcept
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

	void Collision::Unregister(RE::FormID a_id) noexcept
	{
		const auto where = std::ranges::find(_data, a_id, [](auto& it) { return it.first; });
		if (where == _data.end()) {
			logger::error("No object registered using ID {:X}", a_id);
			return;
		}
		_data.erase(where);
	}

	bool Collision::IsRegistered(RE::FormID a_id) const noexcept
	{
		return std::ranges::contains(_data, a_id, [](auto& it) { return it.first; });
	}

	const Collision::PhysicsData* Collision::GetData(RE::FormID a_id) const
	{
		const auto where = std::ranges::find(_data, a_id, [](auto& it) { return it.first; });
		return where == _data.end() ? nullptr : where->second.get();
	}

}	 // namespace Registry
