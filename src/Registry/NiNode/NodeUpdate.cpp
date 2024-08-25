#include "NodeUpdate.h"

namespace Registry::Collision
{
	void NodeUpdate::Install()
	{
		// UpdateThirdPerson
		REL::Relocation<std::uintptr_t> addr{ RELOCATION_ID(39446, 40522), 0x94 };
		stl::write_thunk_call<NodeUpdate>(addr.address());
	}

	void NodeUpdate::thunk(RE::NiAVObject* a_obj, RE::NiUpdateData* updateData)
	{
		RE::NiUpdateData data{
			0.f,
			RE::NiUpdateData::Flag::kNone
		};

		for (auto&& [node, skew] : skews) {
			node->local.rotate = skew;
			node->Update(data);
		}

		return func(a_obj, updateData);
	}

	void NodeUpdate::AddOrUpdateSkew(RE::NiPointer<RE::NiNode> a_node, RE::NiMatrix3 a_skew)
	{
		auto arg = std::make_pair(a_node, a_skew);
		return AddOrUpdateSkew(arg);
	}

	void NodeUpdate::AddOrUpdateSkew(std::pair<RE::NiPointer<RE::NiNode>, RE::NiMatrix3> a_skew)
  {
		auto w = std::ranges::find_if(skews, [&](auto it) { return it.first == a_skew.first; });
    if (w == skews.end()) {
			skews.push_back(a_skew);
		} else {
			w->second = a_skew.second;
		}
	}

	void NodeUpdate::DeleteSkew(const RE::NiPointer<RE::NiNode>& a_skew)
	{
		auto w = std::ranges::find_if(skews, [&](auto it) { return it.first == a_skew; });
		if (w != skews.end()) {
			skews.erase(w);
		}
	}

}	 // namespace Registry::Collision
