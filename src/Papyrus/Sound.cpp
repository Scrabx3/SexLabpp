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

	stl::enumeration<Sound::Type> Sound::GetSoundType(RE::FormID a_id) const
	{
		const auto where = std::ranges::find_if(_registered, [&](auto& it) { return it.first == a_id; });
		if (where == _registered.end()) {
			logger::error("Invalid Sound ID, Object not registered {:X}", a_id);
			return { Type::None };
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

	stl::enumeration<Sound::Type> Sound::SoundProcess::GetSoundType() const
	{
		stl::enumeration<Type> ret{};
		for (auto&& d : data) {
			ret.set(d._types.get());
		}
		return ret;
	}

	void Sound::Data::Update(float a_delta)
	{
		assert(a_delta != 0.0f);
		const auto& [a1, a2] = this->participants;
		_types = Type::None;
		auto [c_type, c_distance] = GetCurrentTypeAndDistance(a1, a2);
		auto [alt_type, alt_distance] = a1 == a2 ? std::pair{ stl::enumeration{ Type::None }, std::array<float, TYPEBITS>{} } : GetCurrentTypeAndDistance(a2, a1);
		_types.set(c_type.get(), alt_type.get());
		for (size_t i = 0; i < TYPEBITS; i++) {
			const auto type = Type(1 << i);
			if (_types.none(type)) {
				_distance[i] = _velocity[i] = 0.0f;
				continue;
			}
			const float new_dist = c_type.all(type) ? c_distance[i] : alt_distance[i];
			const float delta_dist = new_dist - _distance[i];
			_velocity[i] = (_velocity[i] + (delta_dist / a_delta)) / 2;
			_distance[i] = new_dist;
		}
	}

	std::pair<stl::enumeration<Sound::Type>, std::array<float, Sound::TYPEBITS>> Sound::Data::GetCurrentTypeAndDistance(const SoundActor& a_active, const SoundActor& a_passive)
	{
		const auto getHand = [&](const RE::NiPoint3& a_reference, float a_distancederiv = 0.0f) -> std::optional<std::pair<Sound::Type, float>> {
			if (!a_passive.nodes.hand_left || !a_passive.nodes.hand_right)
				return std::nullopt;
			const auto dist_max = Settings::fDistanceHand + a_distancederiv;
			float dhandleft = a_passive.nodes.hand_left->world.translate.GetDistance(a_reference);
			float dhandright = a_passive.nodes.hand_right->world.translate.GetDistance(a_reference);
			if (dhandleft <= dist_max || dhandright <= dist_max) {
				return std::pair{ Type::Hand, std::min(dhandleft, dhandright) };
			}
			return std::nullopt;
		};
		const auto getFoot = [&](const RE::NiPoint3& a_reference, float a_distancederiv = 0.0f) -> std::optional<std::pair<Sound::Type, float>> {
			if (!a_passive.nodes.foot_left || !a_passive.nodes.foot_rigt)
				return std::nullopt;
			const auto dist_max = Settings::fDistanceFoot + a_distancederiv;
			float dfootleft = a_passive.nodes.foot_left->world.translate.GetDistance(a_reference);
			float dfootright = a_passive.nodes.foot_rigt->world.translate.GetDistance(a_reference);
			if (dfootleft <= dist_max || dfootright <= dist_max) {
				return std::pair{ Type::Foot, std::min(dfootleft, dfootright) };
			}
			return std::nullopt;
		};
		const auto getHead = [&](const RE::NiPoint3& a_reference, float a_distancederiv = 0.0f) -> std::optional<std::pair<Sound::Type, float>> {
			if (!a_passive.nodes.head)
				return std::nullopt;
			const auto d = a_reference.GetDistance(a_passive.nodes.head->world.translate);
			if (d > Settings::fDistanceHead + a_distancederiv)
				return std::nullopt;
			const auto& headworld = a_passive.nodes.head->world;
			auto vheading = a_reference - headworld.translate;
			auto vforward = RE::NiPoint3{ headworld.rotate.entry[0][0], headworld.rotate.entry[0][1], headworld.rotate.entry[0][2] };
			vheading.Unitize();
			vforward.Unitize();
			const auto radians = std::acos(vheading.Dot(vforward) / (vheading.Length() * vforward.Length()));
			if (radians < glm::radians(Settings::fAngleMouth)) {
				return std::pair{ Type::Oral, d };
			}
			return std::nullopt;
		};

		stl::enumeration<Type> typeret{ Type::None };
		std::array<float, TYPEBITS> distret{};
		if (a_active.sex != Registry::Sex::Female)
		{
			const auto& schlongmid = a_active.nodes.sos_mid ? a_active.nodes.sos_mid->world.translate : a_active.nodes.ApproximateMid();
			if (a_active != a_passive) {
				// crotch
				const auto [p_crotchfront, pcf_distance] = [&]() -> std::pair<RE::NiPoint3, float> {
					if (a_passive.nodes.vagina_right && a_passive.nodes.vagina_left)
						return { (a_passive.nodes.vagina_right->world.translate + a_passive.nodes.vagina_left->world.translate) / 2, 0.0f };
					return { a_passive.nodes.pelvis->world.translate, Settings::fDistanceCrotchBonus };
				}();
				const auto [p_crotchback, pcb_distance] = [&]() -> std::pair<RE::NiPoint3, float> {
					if (a_passive.nodes.anal_left && a_passive.nodes.anal_right)
						return { (a_passive.nodes.anal_left->world.translate + a_passive.nodes.anal_right->world.translate) / 2, 0.0f };
					return { a_passive.nodes.spine_lower->world.translate, Settings::fDistanceCrotchBonus };
				}();
				const float dcrotchfront = p_crotchfront.GetDistance(schlongmid);
				const float dcrotchback = p_crotchback.GetDistance(schlongmid);
				if (dcrotchfront < pcf_distance + Settings::fDistanceCrotchFront || dcrotchback < pcb_distance + Settings::fDistanceCrotchBack) {
					// find some point to create a vector from to represent direction of schlong
					RE::NiPoint3 vschlong = schlongmid - [&]() -> RE::NiPoint3 {
						if (a_active.nodes.sos_front)
							return a_active.nodes.sos_front->world.translate;
						if (a_active.nodes.sos_base)
							return a_active.nodes.sos_base->world.translate;
						return a_active.nodes.ApproximateTip();
					}();
					RE::NiPoint3 vcrotch = p_crotchfront - p_crotchback;
					vschlong.Unitize();
					vcrotch.Unitize();
					// angle of penetration
					const auto radians = std::acos(vschlong.Dot(vcrotch) / (vcrotch.Length() * vschlong.Length()));
					if (radians < glm::radians(Settings::fAnglePenetration) || radians > glm::radians(180.0f - Settings::fAnglePenetration)) {
						typeret.set(Type::Grinding);
						distret[Registry::FlagIndex(Type::Grinding)]  = dcrotchback;
					} else if (a_passive.nodes.anal_deep && a_passive.nodes.vagina_deep) {
						const auto da = a_passive.nodes.anal_deep->world.translate.GetDistance(schlongmid);
						const auto dv = a_passive.nodes.vagina_deep->world.translate.GetDistance(schlongmid);
						if (dv < da) {
							typeret.set(Type::Vaginal);
							distret[Registry::FlagIndex(Type::Vaginal)]  = dv;
						} else {
							typeret.set(Type::Anal);
							distret[Registry::FlagIndex(Type::Anal)]  = da;
						}
					} else {
						typeret.set(Type::Anal);
						distret[Registry::FlagIndex(Type::Anal)] = dcrotchback;
					}
				}
				if (const auto tmp = getHead(schlongmid)) {
					typeret.set(tmp->first);
					distret[Registry::FlagIndex(tmp->first)] = tmp->second;
				}
				if (const auto tmp = getFoot(schlongmid)) {
					typeret.set(tmp->first);
					distret[Registry::FlagIndex(tmp->first)] = tmp->second;
				}
			}
			if (const auto tmp = getHand(schlongmid)) {
				typeret.set(tmp->first);
				distret[Registry::FlagIndex(tmp->first)] = tmp->second;
			}
		}
		if (a_active.sex != Registry::Sex::Male) {
			const auto [cl, cl_d] = [&]() -> std::pair<RE::NiPoint3, float> {
				if (a_active.nodes.clitoris)
					return { a_active.nodes.clitoris->world.translate, 0.0f };
				return { a_active.nodes.pelvis->world.translate, Settings::fDistanceCrotchBonus };
			}();
			if (a_active != a_passive) {	// partner-only actions
				if (a_passive.sex != Registry::Sex::Male && a_passive.nodes.clitoris) {
					// tribadism
					const auto& c2 = a_active.nodes.clitoris->world.translate;
					const auto d = c2.GetDistance(cl);
					if (d < Settings::fDistanceCrotchFront) {
						typeret.set(Type::Tribadism);
						distret[Registry::FlagIndex(Type::Tribadism)] = d;
					}
				}
				if (const auto tmp = getHead(cl, cl_d)) {
					typeret.set(tmp->first);
					distret[Registry::FlagIndex(tmp->first)] = tmp->second;
				}
				if (const auto tmp = getFoot(cl, cl_d)) {
					typeret.set(tmp->first);
					distret[Registry::FlagIndex(tmp->first)] = tmp->second;
				}
			}
			if (const auto tmp = getHand(cl, cl_d)) {
				typeret.set(tmp->first);
				distret[Registry::FlagIndex(tmp->first)] = tmp->second;
			}
		}
		return { typeret, distret };
	}

	SoundActor::Nodes::Nodes(const RE::Actor* a_actor)
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
		sos_base = RE::NiPointer{ obj->GetObjectByName(SOSSTART) };
		sos_mid = RE::NiPointer{ obj->GetObjectByName(SOSMID) };
		sos_front = RE::NiPointer{ obj->GetObjectByName(SOSGLANCE) };
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

	RE::NiPoint3 SoundActor::Nodes::ApproximateTip() const
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
