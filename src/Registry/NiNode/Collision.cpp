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

	std::vector<Position::Interaction> Position::Snapshot::GetHeadInteractions(const Snapshot& a_partner) const
	{
		if (!bHead.IsValid())
			return {};
		assert(position.nodes.head);
		auto& headworld = position.nodes.head->world;
		auto& partnernodes = a_partner.position.nodes;
		const auto mouthstart = GetHeadForwardPoint(bHead.boundMax.y);
		const auto vMyHead = *mouthstart - headworld.translate;
		assert(mouthstart);
		std::vector<Position::Interaction> ret{};
		if (a_partner.position.sex.any(Sex::Female, Sex::Futa)) {
			auto vVaginal = partnernodes.GetVaginalVector();
			assert(!vVaginal || partnernodes.clitoris);
			if (vVaginal && bHead.IsPointInside(partnernodes.clitoris->world.translate)) {
				auto dot = (*vVaginal).Dot(vMyHead);
				auto angle = RE::rad_to_deg(std::acosf(dot));
				if ((angle - 180) < Settings::fAngleMouth * 2) {
					float distance = partnernodes.clitoris->world.translate.GetDistance(*mouthstart);
					Interaction act{ a_partner.position.actor, Interaction::Action::Oral, distance };
					ret.push_back(act);
				}
			}
			if (!ret.empty()) {
				return ret;
			}
		}
		if (a_partner.position.sex.any(Sex::Male, Sex::Futa)) {
			for (auto&& p : partnernodes.schlongs) {
				assert(p.base);
				const auto& base = p.base->world;
				const auto vBaseToHead = headworld.translate - base.translate;
				const auto pTip = p.GetTipReferencePoint();
				const auto vSchlong = p.GetTipReferenceVector();
				const auto angle_front = vBaseToHead.Dot(vSchlong);
				const auto dCenter = headworld.translate.GetDistance(pTip);
				const auto penetrating_skull = dCenter < ((std::abs(angle_front - 90) < 60 ? bHead.boundMax.x : bHead.boundMax.y) * 0.8);
				if (angle_front < 0) {
					if (!penetrating_skull) {
						Interaction act{ a_partner.position.actor, Interaction::Action::Facial, dCenter };
						ret.push_back(act);
					}
					const auto distance_mouth = vBaseToHead.Dot(vSchlong) / base.translate.SqrLength();
					if (distance_mouth < (bHead.boundMax.x / 2)) {
						auto vRot = headworld.rotate * vSchlong;
						auto dot = vRot.Dot(vSchlong);
						auto angle = RE::rad_to_deg(std::acosf(dot));
						if (std::abs(angle - 180) < Settings::fAngleMouth) {
							Interaction act{ a_partner.position.actor, Interaction::Action::Oral, dCenter };
							ret.push_back(act);
							assert(partnernodes.pelvis);
							const auto tip_throat = pTip.GetDistance(headworld.translate) < bHead.boundMax.y * 0.1;
							const auto pelvis_head = partnernodes.pelvis->world.translate.GetDistance(headworld.translate) <= bHead.boundMax.y;
							if (tip_throat || pelvis_head) {
								Interaction act{ a_partner.position.actor, Interaction::Action::Deepthroat, dCenter };
								ret.push_back(act);
							} else if (std::abs(angle - 90) < Settings::fAngleMouth) {
								Interaction act{ a_partner.position.actor, Interaction::Action::LickingShaft, dCenter };
								ret.push_back(act);
							}
						} else if (penetrating_skull) {
							goto __MAKE_SKULLFUCK;
						}
					} else if (penetrating_skull) {
__MAKE_SKULLFUCK:
						Interaction act{ a_partner.position.actor, Interaction::Action::Skullfuck, dCenter };
						ret.push_back(act);
					}
				}
				if (!ret.empty()) {
					return ret;
				}
			}
		}
		const auto pmouthstart = a_partner.GetHeadForwardPoint(a_partner.bHead.boundMax.y);
		if (pmouthstart) {
			assert(partnernodes.head);
			auto vPartnerHead = *pmouthstart - partnernodes.head->world.translate;
			auto dot = vMyHead.Dot(vPartnerHead);
			auto angle = RE::rad_to_deg(std::acosf(dot));
			if (std::abs(angle - 180) < Settings::fAngleMouth) {
				if (bHead.IsPointInside(*pmouthstart) || a_partner.bHead.IsPointInside(*mouthstart)) {
					float distance = pmouthstart->GetDistance(*mouthstart);
					Interaction act{ a_partner.position.actor, Interaction::Action::Kissing, distance };
					ret.push_back(act);
				}
			}
		}
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


	void Handler::Register(RE::FormID a_id, std::vector<RE::Actor*> a_positions, const Scene* a_scene) noexcept
	{
		try {
			const auto where = std::ranges::find(processes, a_id, [](auto& it) { return it.first; });
			if (where != processes.end()) {
				processes.erase(where);
			}
			auto process = std::make_unique<Process>(a_positions, a_scene);
			processes.emplace_back(a_id, std::move(process));
		} catch (const std::exception& e) {
			logger::error("Cannot register sound processing unit, Error: {}", e.what());
		}
	}

	void Handler::Unregister(RE::FormID a_id) noexcept
	{
		const auto where = std::ranges::find(processes, a_id, [](auto& it) { return it.first; });
		if (where == processes.end()) {
			logger::error("No object registered using ID {:X}", a_id);
			return;
		}
		processes.erase(where);
	}

	bool Handler::IsRegistered(RE::FormID a_id) const noexcept
	{
		return std::ranges::contains(processes, a_id, [](auto& it) { return it.first; });
	}

	const Handler::Process* Handler::GetProcess(RE::FormID a_id) const
	{
		const auto where = std::ranges::find(_data, a_id, [](auto& it) { return it.first; });
		return where == _data.end() ? nullptr : where->second.get();
	}

}	 // namespace Registry::Collision
