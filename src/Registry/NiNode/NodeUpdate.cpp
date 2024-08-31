#include "NodeUpdate.h"

#include "NiMath.h"

namespace Registry::Collision
{
	void RotationData::ApplyRotation()
	{
		// X -> Pitch (Schlong Angle Axis on Humans)
		// Y -> Roll
		// Z -> Yaw
		auto node = schlong->GetBaseReferenceNode();
		auto& local = node->local.rotate;

		auto vS = schlong->GetSchlongVector();
		vS.Unitize();

		auto s = NiMath::ToEigen(vS), i = NiMath::ToEigen(vIdeal);
		auto transform = NiMath::Rodrigue(s, i);
		auto euler = transform.eulerAngles(0, 1, 2);
		auto rot = Eigen::AngleAxisf(-euler[0], Eigen::Vector3f::UnitX()) *
							 Eigen::AngleAxisf(euler[2], Eigen::Vector3f::UnitZ());
		auto niRot = NiMath::AsNiMatrix(rot.toRotationMatrix());
		local = niRot * local;

		RE::NiUpdateData data{
			0.f,
			RE::NiUpdateData::Flag::kNone
		};
		node->Update(data);
	}

	void RotationData::Update(const RE::NiPoint3& a_newIdeal)
	{
		vIdeal = a_newIdeal;
		vIdeal.Unitize();
	}

	void NodeUpdate::Install()
	{
		// UpdateThirdPerson
		REL::Relocation<std::uintptr_t> addr{ RELOCATION_ID(39446, 40522), 0x94 };
		stl::write_thunk_call<NodeUpdate>(addr.address());
	}

	void NodeUpdate::thunk(RE::NiAVObject* a_obj, RE::NiUpdateData* updateData)
	{
		{
			std::scoped_lock lk{ _m };
			for (auto& rot : skews) {
				rot.ApplyRotation();
			}
		}
		return func(a_obj, updateData);
	}

	void NodeUpdate::AddOrUpdateSkew(const std::shared_ptr<Schlong>& a_node, const RE::NiPoint3& vIdeal)
	{
		std::scoped_lock lk{ _m };
		auto w = std::ranges::find_if(skews, [&](auto& it) { return it == a_node; });
		if (w == skews.end()) {
			skews.emplace_back(a_node, vIdeal);
		} else {
			w->Update(vIdeal);
		}
	}

	void NodeUpdate::DeleteSkew(const std::shared_ptr<Schlong>& a_node)
	{
		std::scoped_lock lk{ _m };
		auto w = std::ranges::find_if(skews, [&](auto& it) { return it == a_node; });
		if (w != skews.end())
			skews.erase(w);
	}

}	 // namespace Registry::Collision
