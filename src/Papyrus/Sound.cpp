#include "Sound.h"

#include "Registry/Define/Sex.h"
#include "Registry/Define/Transform.h"

namespace Papyrus
{
	bool Sound::RegisterProcessing(RE::FormID a_id, std::vector<RE::Actor*> a_positions)
	{
		try {
			auto process = std::make_unique<SoundProcess>(a_positions);
			_registered.emplace_back(a_id, std::move(process));
			return true;
		} catch (const std::exception& e) {
			logger::error("Cannot register sound processing unit, Error: {}", e.what());
		}
		return false;
	}

	void Sound::UnregisterProcessing(RE::FormID a_id)
	{
		const auto where = std::ranges::find_if(_registered, [&](auto& it) { return it.first == a_id; });
		if (where == _registered.end()) {
			logger::error("Invalid Sound ID, Object not registered {:X}", a_id);
			return;
		}
		_registered.erase(where);
	}

	std::pair<Sound::Type, float> Sound::GetSoundType(RE::FormID a_id) const
	{
		const auto where = std::ranges::find_if(_registered, [&](auto& it) { return it.first == a_id; });
		if (where == _registered.end()) {
			logger::error("Invalid Sound ID, Object not registered {:X}", a_id);
			return { Type::None, 0.0f };
		}
		return where->second->GetSoundType();
	}

	Sound::SoundProcess::SoundProcess(const std::vector<RE::Actor*>& a_positions) :
		_running(true)
	{
		for (auto&& position : a_positions) {
			actors.emplace_back(position);
		}
		for (size_t i = 0; i < actors.size(); i++) {
			for (size_t n = i; n < actors.size(); n++) {
				Data d(actors[i], actors[n]);
				data.push_back(d);
			}
		}
		_t = std::thread(&Sound::SoundProcess::Process, this);
	}

	Sound::SoundProcess::~SoundProcess()
	{
		_running = false;
		_t.join();
	}

	void Sound::SoundProcess::Process()
	{
		constexpr auto interval = 64ms;
		for (auto&& d : data) {
			d.Update(0.0f);
		}
		while (_running) {
			std::this_thread::sleep_for(interval);
			for (auto&& d : data) {
				const auto delta = static_cast<float>(interval.count());
				d.Update(delta);
			}
		}
	}

	std::pair<Sound::Type, float> Sound::SoundProcess::GetSoundType() const
	{
		Type besttype = Type::None;
		float velocity = 0.0f;
		for (auto&& d : data) {
			if (d._type > besttype) {
				besttype = d._type;
				velocity = d._velocity;
			}
		}
		return { besttype, velocity };
	}

	void Sound::Data::Update(float a_delta)
	{
		const auto& [a1, a2] = this->participants;
		auto [c_type, c_distance] = GetCurrentTypeAndDistance(a1, a2);
		if (a1 != a2) {
			auto [alt_type, alt_distance] = GetCurrentTypeAndDistance(a2, a1);
			if (alt_type > c_type) {
				c_type = alt_type;
				c_distance = alt_distance;
			}
		}
		if (c_distance != 0.0f && _type == c_type && a_delta != 0.0f) {
			float delta_dist = c_distance - _distance;
			_velocity = (_velocity + (delta_dist / a_delta)) / 2;
		} else {
			_velocity = 0.0f;
		}
		_type = c_type;
	}

	std::pair<Sound::Type, float> Sound::Data::GetCurrentTypeAndDistance(const SoundActor& a_active, const SoundActor& a_passive)
	{
		const auto getCrotchFront = [&](const SoundActor::Nodes& a_nodes) -> std::pair<RE::NiPoint3, float> {
			if (a_nodes.vagina_right && a_passive.nodes.vagina_left)
				return { (a_nodes.vagina_right->world.translate + a_nodes.vagina_left->world.translate) / 2, 0.0f };
			if (a_nodes.sos_base)
				return { a_nodes.sos_base->world.translate, 0.0f };
			return { a_nodes.pelvis->world.translate, Settings::fDistanceCrotchBonus };
		};
		const auto [p_crotchfront, pcf_distance] = getCrotchFront(a_passive.nodes);
		const auto [p_crotchback, pcb_distance] = [&]() -> std::pair<RE::NiPoint3, float> {
			if (a_passive.nodes.anal_left && a_passive.nodes.anal_right)
				return { (a_passive.nodes.anal_left->world.translate + a_passive.nodes.anal_right->world.translate) / 2, 0.0f };
			return { a_passive.nodes.spine_lower->world.translate, Settings::fDistanceCrotchBonus };
		}();

		if (a_active.sex != Registry::Sex::Female) {	// schlong interactions
			const auto& schlongglance = a_active.nodes.sos_glance ? a_active.nodes.sos_glance->world.translate : a_active.nodes.ApproximateGlance();
			const auto& schlongmid = a_active.nodes.sos_mid ? a_active.nodes.sos_mid->world.translate : a_active.nodes.ApproximateMid();
			if (a_active != a_passive) {
				// crotch?
				const float dcrotchfront = p_crotchfront.GetDistance(schlongmid);
				const float dcrotchback = p_crotchback.GetDistance(schlongmid);
				if (dcrotchfront < pcf_distance + Settings::fDistanceCrotchFront || dcrotchback < pcb_distance + Settings::fDistanceCrotchBack) {
					RE::NiPoint3 vschlong = schlongmid - schlongglance;
					RE::NiPoint3 vcrotch = p_crotchfront - p_crotchback;
					vschlong.Unitize();
					vcrotch.Unitize();
					// how steep is the angle into the body? If approx parallel, assume no penetration
					const auto radians = std::acos(vschlong.Dot(vcrotch) / (vcrotch.Length() * vschlong.Length()));
					if (radians < glm::radians(Settings::fAnglePenetration) || radians > glm::radians(180.0f - Settings::fAnglePenetration)) {
						return { Type::Grinding, dcrotchfront };
					}
					// otherwise penetration..
					if (a_passive.nodes.anal_deep && a_passive.nodes.vagina_deep) {
						const auto da = a_passive.nodes.anal_deep->world.translate.GetDistance(schlongglance);
						const auto dv = a_passive.nodes.vagina_deep->world.translate.GetDistance(schlongglance);
						if (dv < da) {
							return { Type::Vaginal, dv };
						} else {
							return { Type::Anal, da };
						}
					}
					return { Type::Anal, dcrotchback };
				} else if (const auto d = schlongglance.GetDistance(a_passive.nodes.head->world.translate); d < Settings::fDistanceHead) {	// oral
					const auto& headworld = a_passive.nodes.head->world;
					auto vheading = schlongglance - headworld.translate;
					auto vforward = RE::NiPoint3{ headworld.rotate.entry[0][0], headworld.rotate.entry[0][1], headworld.rotate.entry[0][2] };
					vheading.Unitize();
					vforward.Unitize();
					const auto radians = std::acos(vheading.Dot(vforward) / (vheading.Length() * vforward.Length()));
					if (radians < glm::radians(Settings::fAngleMouth)) {
						return { Type::Oral, d };
					}
				}
			}
			// hand?
			float dhandleft = a_passive.nodes.hand_left->world.translate.GetDistance(schlongmid);
			float dhandright = a_passive.nodes.hand_right->world.translate.GetDistance(schlongmid);
			if (dhandleft < Settings::fDistanceHand || dhandright < Settings::fDistanceHand) {
				return { Type::Hand, std::min(dhandleft, dhandright) };
			}
			// foot?
			float dfootleft = a_passive.nodes.foot_left->world.translate.GetDistance(schlongmid);
			float dfootright = a_passive.nodes.foot_rigt->world.translate.GetDistance(schlongmid);
			if (dfootleft < Settings::fDistanceFoot || dfootright < Settings::fDistanceFoot) {
				return { Type::Foot, std::min(dfootleft, dfootright) };
			}
		} else if (a_active != a_passive && a_active.sex != Registry::Sex::Male && a_passive.sex != Registry::Sex::Male) {
			const auto [active_crotchfront, acf_distance] = getCrotchFront(a_active.nodes);
			const auto c_distance = p_crotchfront.GetDistance(active_crotchfront);
			if (c_distance < Settings::fDistanceCrotchFront + acf_distance + pcf_distance) {
				return { Type::Tribadism, c_distance };
			}
		}	 
		// finger
		const auto& alhand = a_active.nodes.hand_left->world.translate;
		const auto& arhand = a_active.nodes.hand_right->world.translate;
		const auto& da = std::min(alhand.GetDistance(p_crotchback), arhand.GetDistance(p_crotchback));
		if (a_passive.sex == Registry::Sex::Female) {
			const auto& dv = std::min(alhand.GetDistance(p_crotchfront), arhand.GetDistance(p_crotchfront));
			if (dv < Settings::fDistanceCrotchFront || da < Settings::fDistanceCrotchBack) {
				return dv < da ? std::pair{ Type::FingeringV, dv } : std::pair{ Type::FingeringA, da };
			}
		} else if (da < Settings::fDistanceCrotchBack) {
			return { Type::FingeringA, da };
		}
		return { Type::None, 0.0f };
	}

	SoundActor::Nodes::Nodes(const RE::Actor* a_actor)
	{
		const auto obj = a_actor->Get3D();
		if (!obj) {
			throw std::exception("Unable to retrieve 3D of actor {:X}", a_actor->GetFormID());
		}
		head = RE::NiPointer{ obj->GetObjectByName(HEAD) };
		pelvis = RE::NiPointer{ obj->GetObjectByName(PELVIS) };
		spine_lower = RE::NiPointer{ obj->GetObjectByName(SPINELOWER) };
		tigh_right = RE::NiPointer{ obj->GetObjectByName(TIGHRIGHT) };
		tigh_left = RE::NiPointer{ obj->GetObjectByName(THIGHLEFT) };

		if (!head || !pelvis || !spine_lower || !tigh_right || !tigh_left)
			throw std::exception("Missing mandatory 3d object (body)");

		hand_left = RE::NiPointer{ obj->GetObjectByName(HANDLEFT) };
		hand_right = RE::NiPointer{ obj->GetObjectByName(HANDRIGHT) };
		foot_left = RE::NiPointer{ obj->GetObjectByName(FOOTLEFT) };
		foot_rigt = RE::NiPointer{ obj->GetObjectByName(FOOTRIGHT) };

		if (!hand_left || !hand_right || !foot_left || !foot_rigt)
			throw std::exception("Missing mandatory 3d object (limbs)");

		clitoris = RE::NiPointer{ obj->GetObjectByName(CLITORIS) };
		vagina_deep = RE::NiPointer{ obj->GetObjectByName(VAGINA) };
		vagina_left = RE::NiPointer{ obj->GetObjectByName(VAGINALLEFT) };
		vagina_right = RE::NiPointer{ obj->GetObjectByName(VAGINALRIGHT) };
		sos_base = RE::NiPointer{ obj->GetObjectByName(SOSSTART) };
		sos_start = RE::NiPointer{ obj->GetObjectByName(SOSBASE) };
		sos_mid = RE::NiPointer{ obj->GetObjectByName(SOSMID) };
		sos_glance = RE::NiPointer{ obj->GetObjectByName(SOSGLANCE) };
		sos_scrotum = RE::NiPointer{ obj->GetObjectByName(SOSSCROTUM) };
		anal_deep = RE::NiPointer{ obj->GetObjectByName(ANAL) };
		anal_left = RE::NiPointer{ obj->GetObjectByName(ANALLEFT) };
		anal_right = RE::NiPointer{ obj->GetObjectByName(ANALRIGHT) };
	}

	RE::NiPoint3 SoundActor::Nodes::ApproximateNode(float a_forward, float a_upward) const
	{
		Registry::Coordinate approx(std::vector{ a_forward, 0.0f, a_upward, 0.0f });
		RE::NiPoint3 angle;
		pelvis->world.rotate.ToEulerAnglesXYZ(angle);
		Registry::Coordinate ret{ pelvis->world.translate, angle.z };
		approx.Apply(ret);
		return ret.AsNiPoint();
	}

	RE::NiPoint3 SoundActor::Nodes::ApproximateGlance() const
	{
		constexpr float forward = 20.0f;
		constexpr float upward = -4.0f;
		return ApproximateNode(forward, upward);
	}

	RE::NiPoint3 SoundActor::Nodes::ApproximateMid() const
	{
		constexpr float forward = 15.0f;
		constexpr float upward = -6.2f;
		return ApproximateNode(forward, upward);
	}

	RE::NiPoint3 SoundActor::Nodes::ApproximateBase() const
	{
		constexpr float forward = 10.0f;
		constexpr float upward = -5.0f;
		return ApproximateNode(forward, upward);
	}


}	 // namespace Papyrus
