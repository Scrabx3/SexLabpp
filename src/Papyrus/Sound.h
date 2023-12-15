#pragma once

#include "Registry/Define/Sex.h"

namespace Papyrus
{
	static constexpr std::string_view HEAD{ "NPC Head [Head]"sv };					// Back of throat
	static constexpr std::string_view PELVIS{ "NPC Pelvis [Pelv]"sv };			// bottom mid (front)
	static constexpr std::string_view SPINELOWER{ "NPC Spine [Spn0]"sv };		// bottom mid (back)

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

			RE::NiPointer<RE::NiAVObject> hand_left;
			RE::NiPointer<RE::NiAVObject> hand_right;
			RE::NiPointer<RE::NiAVObject> foot_left;
			RE::NiPointer<RE::NiAVObject> foot_rigt;

			RE::NiPointer<RE::NiAVObject> clitoris;
			RE::NiPointer<RE::NiAVObject> vagina_deep;
			RE::NiPointer<RE::NiAVObject> vagina_left;
			RE::NiPointer<RE::NiAVObject> vagina_right;
			RE::NiPointer<RE::NiAVObject> sos_base;
			RE::NiPointer<RE::NiAVObject> sos_mid;
			RE::NiPointer<RE::NiAVObject> sos_front;
			RE::NiPointer<RE::NiAVObject> anal_deep;
			RE::NiPointer<RE::NiAVObject> anal_left;
			RE::NiPointer<RE::NiAVObject> anal_right;

		public:
			RE::NiPoint3 ApproximateTip() const;
			RE::NiPoint3 ApproximateMid() const;
			RE::NiPoint3 ApproximateBase() const;

		private:
			RE::NiPoint3 ApproximateNode(float a_forward, float a_upward) const;
		};

	public:
		SoundActor(RE::Actor* a_actor) :
			represent(a_actor), sex(Registry::GetSex(a_actor)), nodes(a_actor) {}
		~SoundActor() = default;

	public:
		[[nodiscard]] constexpr bool operator==(const SoundActor& a_rhs) const noexcept { return represent == a_rhs.represent; }

	public:
		RE::Actor* represent;
		Registry::Sex sex;
		Nodes nodes;
	};

	class Sound :
		public Singleton<Sound>
	{
		enum class Type
		{
			None = 0,
			Foot = 1 << 0,
			Hand = 1 << 1,
			Tribadism = 1 << 2,
			Grinding = 1 << 3,
			Oral = 1 << 4,
			Anal = 1 << 5,
			Vaginal = 1 << 6,
		};
		static constexpr size_t TYPEBITS = 7;

		struct Data
		{
			Data(SoundActor& a, SoundActor& b) :
				_types(Type::None), participants(a, b), _distance({}), _velocity({}) {}
			~Data() = default;

			void Update(float a_delta);

		public:
			std::pair<SoundActor&, SoundActor&> participants;
			stl::enumeration<Type> _types;
			std::array<float, TYPEBITS> _distance;
			std::array<float, TYPEBITS> _velocity;

		private:
			static std::pair<stl::enumeration<Type>, std::array<float, TYPEBITS>> GetCurrentTypeAndDistance(const SoundActor& a_active, const SoundActor& a_passive);
			// static std::pair<Type, float> GetCurrentTypeAndDistance(const SoundActor& a_passive, const SoundActor& a_active);
		};

		struct SoundProcess
		{
			SoundProcess(const std::vector<RE::Actor*>& a_positions);
			~SoundProcess();

			std::vector<SoundActor> actors;
			std::vector<Data> data;

		public:
			stl::enumeration<Type> GetSoundType() const;

		private:
			void Process();

			std::thread _t;
			bool _running;
		};

	public:
		stl::enumeration<Sound::Type> GetSoundType(RE::FormID a_id) const;

		bool RegisterProcessing(RE::FormID a_id, std::vector<RE::Actor*> a_positions);
		void UnregisterProcessing(RE::FormID a_id);

	private:
		std::vector<std::pair<RE::FormID, std::unique_ptr<SoundProcess>>> _registered;
	};

}	 // namespace Papyrus
