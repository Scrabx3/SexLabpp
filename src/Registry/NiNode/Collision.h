#pragma once

#include "Registry/Animation.h"
#include "Registry/Define/Sex.h"
#include "Registry/Util/RayCast/ObjectBound.h"

namespace Registry
{
	class Collision :
		public Singleton<Collision>
	{
		struct TypeData
		{
			enum class Type
			{
				None = -1,
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

			RE::FormID _partner{ 0 };	 // actor performing type
			Type _type{ Type::None };	 // action type performed by partner
			float _velocity{ 0.0f };
			float _distance{ 0.0f };
		};

		struct Position
		{
			struct Nodes
			{
				Nodes(const RE::Actor* a_actor, bool a_alternatenodes);
				~Nodes() = default;

				RE::NiPointer<RE::NiNode> head;
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

			Collision::TypeData* GetType(TypeData& a_data);

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

				std::optional<TypeData> GetHeadInteraction(const WorkingData& a_partner) const; // get interactions this actor is performing with its head

				std::optional<TypeData> GetsOral(const WorkingData& a_partner) const;				 // a_partner giving oral to this
				std::optional<TypeData> GetsHandjob(const WorkingData& a_partner) const;		 // a_partner pleasuring this with hands
				std::optional<TypeData> GetsFootjob(const WorkingData& a_partner) const;		 //  --- "" --- feet
				std::optional<TypeData> DoesGrinidng(const WorkingData& a_partner) const;		 // a_partner penetrating this
				std::optional<TypeData> HasIntercourse(const WorkingData& a_partner) const;	 // a_partner penetrating this

			public:
				float GetHeadForwardDistance() const { return bHead.boundMax.y; }
				std::optional<RE::NiPoint3> GetHeadForwardPoint(float distance) const;

			public:
				Position& _position;
			  ObjectBound bHead;
				RE::NiNode* niGenitals;

				std::vector<TypeData> types{};
				RE::NiPoint3 pGenitalReference{};
				RE::NiPoint3 vCrotch{};
				RE::NiPoint3 vSchlong{};
			};

		public:
			PhysicsData(std::vector<RE::Actor*> a_positions, const Scene* a_scene);
			~PhysicsData();

		public:
			std::vector<Position> _positions;

		private:
			void Update();

			std::atomic<bool> _tactive;
			std::thread _t;
		};

	public:
		void Register(RE::FormID a_id, std::vector<RE::Actor*> a_positions, const Scene* a_scene) noexcept;
		void Unregister(RE::FormID a_id) noexcept;
		bool IsRegistered(RE::FormID a_id) const noexcept;
		const PhysicsData* GetData(RE::FormID a_id) const;

	private:
		std::vector<std::pair<RE::FormID, std::unique_ptr<PhysicsData>>> _data;
	};

}	 // namespace Registry
