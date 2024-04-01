#pragma once

#include "Registry/Define/Sex.h"
#include "Registry/Animation.h"

namespace Registry
{
	static constexpr std::string_view HEAD{ "NPC Head [Head]"sv };				 // Back of throat
	static constexpr std::string_view PELVIS{ "NPC Pelvis [Pelv]"sv };		 // bottom mid (front)
	static constexpr std::string_view SPINELOWER{ "NPC Spine [Spn0]"sv };	 // bottom mid (back)

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
	static constexpr std::array SOSSTART{
		"NPC Genitals01 [Gen01]"sv, "AH Base"sv, "DD 2"sv, "DD2"sv, "NPC IceGenital02"sv, "BearD 3"sv, "GS 3"sv, "BoarDick01"sv, "RD 2"sv, "CDPenis 2"sv, "CO 2"sv,
		"ElkD03"sv, "DwarvenSpiderDildo01"sv, "FD 3"sv, "GD 3"sv, "Goat_Penis02"sv, "Horker_Penis04"sv, "HS 3"sv, "SCD 3"sv, "SkeeverD 03"sv, "TD 3"sv, "VLDick03"sv,
		"WWD 4"sv, "Torso Rock 2a"sv
	};
	static constexpr std::array SOSMID{
		"NPC Genitals04 [Gen04]"sv, "AH 3"sv, "DD 3"sv, "DD3"sv, "NPC IceGenital03"sv, "BearD 6"sv, "GS 4"sv, "BoarDick03"sv, "RD 3"sv, "CDPenis 5"sv, "CO 5"sv,
		"ElkD04"sv, "DwarvenSpiderDildo02"sv, "FD 4"sv, "GD 4"sv, "Goat_Penis04"sv, "Horker_Penis06"sv, "HS 6"sv, "SCD 4"sv, "SkeeverD 05"sv, "TD 5"sv, "VLDick05"sv,
		"WWD 7"sv, "Torso Rock 2"sv
	};
	static constexpr std::array SOSTIP{
		"NPC Genitals06 [Gen06]"sv, "AH 6"sv, "DD 6"sv, "DD6"sv, "BearD 8"sv, "GS 7"sv, "BoarDick06"sv, "RD 5"sv, "CDPenis 7"sv, "CO 9"sv, "ElkD06"sv, "DwarvenSpiderDildo03"sv,
		"FD 7"sv, "GD 7"sv, "Goat_Penis06"sv, "Horker_Penis10"sv, "HS 7"sv, "SCD 7"sv, "SkeeverD 07"sv, "TD 7"sv, "VLDick06"sv, "WWD 9"sv
	};
	// Actors with more than 1 schlong
	static constexpr std::array SOSSTART_ALT{ "RD 2"sv };
	static constexpr std::array SOSMID_ALT{ "RD 3"sv };
	static constexpr std::array SOSTIP_ALT{ "RD 5"sv };

	class Physics :
		public Singleton<Physics>
	{
		struct TypeData
		{
			enum class Type
			{
				VaginalP = 0,	 // being penetrated (passive)
				AnalP = 1,
				VaginalA = 2,	 // penetrating (active)
				AnalA = 3,
				Oral = 4,
				Foot = 5,
				Hand = 6,
				Grinding = 7,

				Total,
			};

			RE::FormID _partner;	// actor performing type
			Type _type;						// action type performed by partner

			float _velocity;
			float _distance;
		};

		struct Position
		{
			struct Nodes
			{
				Nodes(const RE::Actor* a_actor, bool a_alternatenodes);
				~Nodes() = default;

				RE::NiPointer<RE::NiAVObject> head;
				RE::NiPointer<RE::NiAVObject> pelvis;
				RE::NiPointer<RE::NiAVObject> spine_lower;

				RE::NiPointer<RE::NiAVObject> hand_left;
				RE::NiPointer<RE::NiAVObject> hand_right;
				RE::NiPointer<RE::NiAVObject> foot_left;
				RE::NiPointer<RE::NiAVObject> foot_rigt;

				RE::NiPointer<RE::NiAVObject> clitoris;
				RE::NiPointer<RE::NiAVObject> sos_base;
				RE::NiPointer<RE::NiAVObject> sos_mid;
				RE::NiPointer<RE::NiAVObject> sos_front;

			public:
				RE::NiPoint3 ApproximateTip() const;
				RE::NiPoint3 ApproximateMid() const;
				RE::NiPoint3 ApproximateBase() const;

			private:
				RE::NiPoint3 ApproximateNode(float a_forward, float a_upward) const;
			};

		public:
			Position(RE::Actor* a_owner, Sex a_sex);
			~Position() = default;

			std::optional<Physics::TypeData*> GetType(TypeData& a_data);

		public:
			RE::FormID _owner;
			stl::enumeration<Sex> _sex;
			Nodes _nodes;
			std::vector<TypeData> _types;
		};

		class PhysicsData
		{
			struct WorkingData
			{
				WorkingData(Position& a_position);
				~WorkingData() = default;

				std::optional<TypeData> GetsOral(const WorkingData& a_partner) const;				 // a_partner giving oral to this
				std::optional<TypeData> GetsHandjob(const WorkingData& a_partner) const;		 // a_partner pleasuring this with hands
				std::optional<TypeData> GetsFootjob(const WorkingData& a_partner) const;		 //  --- "" --- feet
				std::optional<TypeData> DoesGrinidng(const WorkingData& a_partner) const;		 // a_partner penetrating this
				std::optional<TypeData> HasIntercourse(const WorkingData& a_partner) const;	 // a_partner penetrating this

			public:
				Position& _position;
				RE::NiPoint3 pGenitalReference;
				RE::NiPoint3 vCrotch;
				RE::NiPoint3 vSchlong;
			};

		public:
			PhysicsData(std::vector<RE::Actor*> a_positions, const Scene* a_scene);
			~PhysicsData();

		private:
			void Update();

		private:
			std::vector<Position> _positions;
			std::atomic<bool> _tactive;
			std::thread _t;
		};

	public:
		void Register(RE::FormID a_id, std::vector<RE::Actor*> a_positions, const Scene* a_scene) noexcept;
		void Unregister(RE::FormID a_id) noexcept;
		bool IsRegistered(RE::FormID a_id) noexcept;

	private:
		std::vector<std::pair<RE::FormID, std::unique_ptr<PhysicsData>>> _data;
	};

}	 // namespace Registry
