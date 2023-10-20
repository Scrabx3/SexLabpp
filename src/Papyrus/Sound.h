#pragma once

#include "Registry/Define/Sex.h"

namespace Papyrus
{
	static constexpr std::string_view HEAD{ "NPC Head [Head]"sv };					// Back of throat
	static constexpr std::string_view PELVIS{ "NPC Pelvis [Pelv]"sv };			// closest to groin area
	static constexpr std::string_view SPINELOWER{ "NPC Spine [Spn0]"sv };		// closest to butt (center)
	static constexpr std::string_view THIGHLEFT{ "NPC L Thigh [LThg]"sv };	// roughly same height as pelvis
	static constexpr std::string_view TIGHRIGHT{ "NPC R Thigh [RThg]"sv };

	static constexpr std::string_view HANDLEFT{ "NPC L Finger20 [LF20]"sv };	// Base of middle finger
	static constexpr std::string_view HANDRIGHT{ "NPC R Finger20 [RF20]"sv };
	static constexpr std::string_view FOOTLEFT{ "NPC L Foot [Lft ]"sv };	// Ankle
	static constexpr std::string_view FOOTRIGHT{ "NPC R Foot [Rft ]"sv };

	static constexpr std::string_view CLITORIS{ "Clitoral1"sv };
	static constexpr std::string_view VAGINA{ "VaginaDeep1"sv };
	static constexpr std::string_view VAGINALLEFT{ "NPC L Pussy02"sv };
	static constexpr std::string_view VAGINALRIGHT{ "NPC R Pussy02"sv };
	static constexpr std::string_view ANAL{ "NPC Anus Deep2"sv };
	static constexpr std::string_view ANALLEFT{ "NPC LB Anus2"sv };
	static constexpr std::string_view ANALRIGHT{ "NPC RB Anus2"sv };
	static constexpr std::string_view SOSBASE{ "NPC GenitalsBase [GenBase]"sv };
	static constexpr std::string_view SOSSTART{ "NPC Genitals01 [Gen01]"sv };
	static constexpr std::string_view SOSMID{ "NPC Genitals04 [Gen04]"sv };
	static constexpr std::string_view SOSGLANCE{ "NPC Genitals06 [Gen06]"sv };
	static constexpr std::string_view SOSSCROTUM{ "NPC GenitalsScrotum [GenScrot]"sv };

	struct SoundActor
	{
		struct Nodes
		{
			Nodes(const RE::Actor* a_actor);
			~Nodes() = default;

			RE::NiPointer<RE::NiAVObject> head;
			RE::NiPointer<RE::NiAVObject> pelvis;
			RE::NiPointer<RE::NiAVObject> spine_lower;
			RE::NiPointer<RE::NiAVObject> tigh_right;
			RE::NiPointer<RE::NiAVObject> tigh_left;

			RE::NiPointer<RE::NiAVObject> hand_left;
			RE::NiPointer<RE::NiAVObject> hand_right;
			RE::NiPointer<RE::NiAVObject> foot_left;
			RE::NiPointer<RE::NiAVObject> foot_rigt;

			RE::NiPointer<RE::NiAVObject> clitoris;
			RE::NiPointer<RE::NiAVObject> vagina_deep;
			RE::NiPointer<RE::NiAVObject> vagina_left;
			RE::NiPointer<RE::NiAVObject> vagina_right;
			RE::NiPointer<RE::NiAVObject> sos_base;
			RE::NiPointer<RE::NiAVObject> sos_start;
			RE::NiPointer<RE::NiAVObject> sos_mid;
			RE::NiPointer<RE::NiAVObject> sos_glance;
			RE::NiPointer<RE::NiAVObject> sos_scrotum;
			RE::NiPointer<RE::NiAVObject> anal_deep;
			RE::NiPointer<RE::NiAVObject> anal_left;
			RE::NiPointer<RE::NiAVObject> anal_right;

		public:
			RE::NiPoint3 ApproximateGlance() const;
			RE::NiPoint3 ApproximateMid() const;
			RE::NiPoint3 ApproximateBase() const;

		private:
			RE::NiPoint3 ApproximateNode(float a_forward, float a_upward) const;
		};

	public:
		SoundActor(RE::Actor* a_actor) :
			represent(a_actor), sex(Registry::GetSex(a_actor)), nodes(a_actor) {}
		~SoundActor() = default;

		// Wrappers if a sos schlong is missing but expected
		// Points are calculated by applying an offset to pelvis node

	public :
		RE::Actor* represent;
		Registry::Sex sex;
		Nodes nodes;
	};

	class Sound :
		public Singleton<Sound>
	{
		enum class Type
		{
			None,

			Hand,
			Foot,
			FingeringA,
			FingeringV,
			Tribadism,
			Grinding,

			Oral,
			Anal,
			Vaginal,
		};

		struct Data
		{
			Data(SoundActor& a, SoundActor& b) :
				type(Type::None), participants(a, b), _distance(0), _velocity(0) {}
			~Data() = default;

			Type type;
			std::pair<SoundActor&, SoundActor&> participants;
			float _distance;
			float _velocity;

			void Update(float a_delta);

		private:
			static std::pair<Type, float> GetCurrentTypeAndDistance(const SoundActor& a_passive, const SoundActor& a_active);
		};

		struct SoundProcess
		{
			SoundProcess(const std::vector<RE::Actor*>& a_positions);
			~SoundProcess();

			std::vector<SoundActor> actors;
			std::vector<Data> data;

		public:
			Type GetSoundType() const;

		private:
			void Process();

			std::thread _t;
			bool _running;
		};

	public:
		Type GetSoundType(RE::FormID a_id) const;

		bool RegisterProcessing(RE::FormID a_id, std::vector<RE::Actor*> a_positions);
		void UnregisterProcessing(RE::FormID a_id);

	private:
		std::vector<std::pair<RE::FormID, std::unique_ptr<SoundProcess>>> _registered;
	};

}	 // namespace Papyrus
