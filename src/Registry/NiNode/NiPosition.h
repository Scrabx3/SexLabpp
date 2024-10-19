#pragma once

#include "Registry/Animation.h"
#include "Registry/Define/Sex.h"
#include "Registry/NiNode/Node.h"
#include "Registry/Util/RayCast/ObjectBound.h"

namespace Registry::NiNode
{
	/// @brief Rotate a_schlong s.t. a_target is on the line drawn by its orientation
	void RotateNode(RE::NiPointer<RE::NiNode> niNode, const NiMath::Segment& sNode, const RE::NiPoint3& pTarget, float maxAngleAdjust);

	struct Interaction
	{
		enum class Action
		{
			None = 0,
			Vaginal,			 // This being penetrated
			Anal,					 // This being penetrated
			Oral,					 // This licking/sucking other
			Grinding,			 // This being grinded against by other
			Deepthroat,		 // Oral, with other close to/at maximum depth
			Skullfuck,		 // This head penetration that isnt Oral
			LickingShaft,	 // This licking others shaft

			FootJob,	// This being pleasured by other
			HandJob,	// This being pleasured by other
			Kissing,	// This kissing partner
			Facial,		// Other penis in front of this face

			AnimObjFace,

			Total,
		};

		Interaction(RE::ActorPtr a_partner, Action a_action, float a_distance) :
			partner(a_partner), action(a_action), distance(a_distance) {}
		~Interaction() = default;

	public:
		RE::ActorPtr partner{ 0 };
		Action action{ Action::None };
		float distance{ 0.0f };
		float velocity{ 0.0f };

	public:
		bool operator==(const Interaction& a_rhs) const { return a_rhs.partner == partner && a_rhs.action == action; }
		bool operator<(const Interaction& a_rhs) const
		{
			const auto cmp = partner->GetFormID() <=> a_rhs.partner->GetFormID();
			return cmp == 0 ? action < a_rhs.action : cmp < 0;
		}
	};

	struct NiPosition
	{
		struct Snapshot
		{
			Snapshot(NiPosition& a_position);
			~Snapshot() = default;

			bool GetHeadPenisInteractions(const Snapshot& a_partner, std::shared_ptr<Node::NodeData::SchlongData> a_schlong);

			// void GetHeadHeadInteractions(const Snapshot& a_other);			// This head on other head
			// void GetHeadVaginaInteractions(const Snapshot& a_other);		// This head on other vagina
			// void GetHeadPenisInteractions(const Snapshot& a_other);			// This head on other penis
			// void GetHeadAnimObjInteractions(const Snapshot& a_other);		// This head with either animobj
			// void GetCrotchPenisInteractions(const Snapshot& a_other);		// This crotch/vagina on other penis
			// void GetVaginaVaginaInteractions(const Snapshot& a_other);	// This vagina on other vagina
			// void GetGenitalLimbInteractions(const Snapshot& a_other);		// This vagina on other limbs

		public:
			std::optional<RE::NiPoint3> GetMouthStartPoint() const;
			std::optional<RE::NiPoint3> GetThroatPoint() const;

		public:
			NiPosition& position;
			ObjectBound bHead;
			std::vector<Interaction> interactions{};

		public:
			bool operator==(const Snapshot& a_rhs) const { return position == a_rhs.position; }
		};

	public:
		NiPosition(RE::Actor* a_owner, Sex a_sex) :
			actor(a_owner), nodes(a_owner), sex(a_sex) {}
		~NiPosition() = default;

	public:
		RE::ActorPtr actor;
		Node::NodeData nodes;
		stl::enumeration<Sex> sex;
		std::set<Interaction> interactions{};

	public:
		bool operator==(const NiPosition& a_rhs) const { return actor == a_rhs.actor; }
	};

}	 // namespace Registry::NiNode
