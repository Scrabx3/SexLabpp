#pragma once

#include "Registry/Animation.h"
#include "Registry/Define/Sex.h"
#include "Registry/NiNode/Node.h"
#include "Registry/Util/RayCast/ObjectBound.h"

namespace Registry::Collision
{
	constexpr inline auto INTERVAL = 128ms;

	struct Position
	{
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
		};

		struct Snapshot
		{
			Snapshot(Position& a_position);
			~Snapshot() = default;

			std::vector<Interaction> GetHeadHeadInteractions(const Snapshot& a_other) const;			// This head on other head
			std::vector<Interaction> GetHeadVaginaInteractions(const Snapshot& a_other) const;		// This head on other vagina
			std::vector<Interaction> GetHeadPenisInteractions(const Snapshot& a_other) const;			// This head on other penis
			std::vector<Interaction> GetCrotchPenisInteractions(const Snapshot& a_other) const;		// This crotch/vagina on other penis
			std::vector<Interaction> GetVaginaVaginaInteractions(const Snapshot& a_other) const;	// This vagina on other vagina
			std::vector<Interaction> GetGenitalLimbInteractions(const Snapshot& a_other) const;		// This vagina on other limbs

		public:
			std::optional<RE::NiPoint3> GetHeadForwardPoint(float distance) const;

		public:
			Position& position;
			ObjectBound bHead;

			std::map<std::pair<RE::FormID, Interaction::Action>, Interaction> interactions{};
		};

	public:
		Position(RE::Actor* a_owner, Sex a_sex) :
			actor(a_owner), nodes(a_owner), sex(a_sex) {}
		~Position() = default;

	public:
		RE::ActorPtr actor;
		Node::NodeData nodes;
		stl::enumeration<Sex> sex;
		std::vector<Interaction> interactions{};
	};

	class Handler :
		public Singleton<Handler>
	{
		class Process
		{
		public:
			Process(const std::vector<RE::Actor*>& a_positions, const Scene* a_scene);
			~Process();

		private:
			void Update();
			std::vector<Position> positions;
			std::atomic<bool> active;
			std::thread _t;
		};

	public:
		void Register(RE::FormID a_id, std::vector<RE::Actor*> a_positions, const Scene* a_scene) noexcept;
		void Unregister(RE::FormID a_id) noexcept;
		bool IsRegistered(RE::FormID a_id) const noexcept;
		const Process* GetProcess(RE::FormID a_id) const;

	private:
		std::vector<std::pair<RE::FormID, std::unique_ptr<Process>>> processes;
	};

}	 // namespace Registry
