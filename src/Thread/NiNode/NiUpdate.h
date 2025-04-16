#pragma once

#include "NiPosition.h"
#include "Node.h"
#include "Registry/Define/Animation.h"

namespace Thread::NiNode
{
	class NiUpdate
	{
		struct Process
		{
			friend class NiUpdate;

		public:
			Process(const std::vector<RE::Actor*>& a_positions, const Registry::Scene* a_scene);
			~Process() = default;

			bool VisitPositions(std::function<bool(const NiPosition&)> a_visitor) const;

		private:
			void UpdateInteractions(float a_delta);
			void GetInteractionsMale(std::vector<NiPosition::Snapshot>& list, const NiPosition::Snapshot& it);
			void GetInteractionsFemale(std::vector<NiPosition::Snapshot>& list, const NiPosition::Snapshot& it);
			void GetInteractionsNeutral(std::vector<NiPosition::Snapshot>& list, const NiPosition::Snapshot& it);

			std::vector<NiPosition> positions;
			mutable std::mutex _m{};
		};

	public:
		static void Install();

		static void Register(RE::FormID a_id, std::vector<RE::Actor*> a_positions, const Registry::Scene* a_scene) noexcept;
		static void Unregister(RE::FormID a_id) noexcept;
		static bool IsRegistered(RE::FormID a_id) noexcept;
		static const Process* GetProcess(RE::FormID a_id) noexcept;

	private:
		friend void stl::write_thunk_call<NiUpdate>(std::uintptr_t);
		static void thunk(RE::NiAVObject* a_obj, RE::NiUpdateData* updateData);
		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t size{ 5 };

		static inline std::mutex _m{};
		static inline std::vector<std::pair<RE::FormID, std::unique_ptr<Process>>> processes;
	};

}	 // namespace Thread::Collision
