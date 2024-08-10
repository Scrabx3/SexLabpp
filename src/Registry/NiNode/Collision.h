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
				Vaginal,
				Anal,

				FootJob,
				HandJob,
				Grinding,

				Cunnilingius,
				Oral,
				Deepthroat,
				Skullfuck,
				LickingShaft,

				Kissing,
				Facial,

				Total,
			};

			Interaction(RE::ActorPtr a_partner, Action a_action, float a_distance) :
				partner(a_partner), action(a_action), distance(a_distance) {}
			~Interaction() = default;

			RE::ActorPtr partner{ 0 };
			Action action{ Action::None };
			float distance{ 0.0f };
			float velocity{ 0.0f };
		};

		struct Snapshot
		{
			Snapshot(Position& a_position);
			~Snapshot() = default;

			std::vector<Interaction> GetHeadInteractions(const Snapshot& a_other) const;	// get interactions this actor is performing with its head
			std::vector<Interaction> GetVaginalInteractions(const Snapshot& a_other) const;	// get interactions this actor is performing with its head

		public:
			std::optional<RE::NiPoint3> GetHeadForwardPoint(float distance) const;

		public:
			Position& position;
			ObjectBound bHead;

			std::vector<Interaction> interactions{};
			RE::NiPoint3 pGenitalReference{};
			RE::NiPoint3 vCrotch{};
			RE::NiPoint3 vSchlong{};
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
