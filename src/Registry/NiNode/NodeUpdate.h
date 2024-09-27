#pragma once

#include <Eigen/Dense>

#include "Registry/NiNode/Node.h"

namespace Registry::Collision
{
	using Schlong = Node::NodeData::SchlongData;

	struct RotationData
	{
		template <class T>
		RotationData(std::shared_ptr<Schlong> a_schlong, const T& a_ideal) :
			schlong(a_schlong) { Update(a_ideal); }

		void ApplyRotation();
		void Update(const RE::NiPointer<RE::NiNode>& a_target);
		void Update(const std::function<RE::NiPoint3()>& a_proj);

	private:
		std::shared_ptr<Schlong> schlong;
		std::function<RE::NiPoint3()> proj;

	public:
		bool operator==(const RotationData& a_rhs) const { return this->schlong == a_rhs.schlong; }
		bool operator==(const std::shared_ptr<Schlong>& a_rhs) const { return this->schlong == a_rhs; }
	};

	struct NodeUpdate
	{
		static void Install();

		static void RegisterSchlong(const std::shared_ptr<Schlong>& schlong, std::vector<Node::NodeData>& partners);

		template <class T>
		static void AddOrUpdateSkew(const std::shared_ptr<Schlong>& a_node, const T& a_ideal)
		{
			std::scoped_lock lk{ _m };
			auto w = std::ranges::find_if(skews, [&](auto& it) { return it == a_node; });
			if (w == skews.end()) {
				skews.emplace_back(a_node, a_ideal);
			} else {
				w->Update(a_ideal);
			}
		}

		static void DeleteSkew(const std::shared_ptr<Schlong>& a_node);
		static void DeleteSkews()
		{
			std::scoped_lock lk{ _m };
			skews.clear();
		}

	private:
		friend void stl::write_thunk_call<NodeUpdate>(std::uintptr_t);
		static void thunk(RE::NiAVObject* a_obj, RE::NiUpdateData* updateData);
		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t size{ 5 };

		static inline std::mutex _m{};
		static inline std::vector<RotationData> skews{};
	};

}	 // namespace Registry::Collision
