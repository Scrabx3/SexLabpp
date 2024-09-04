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
		Eigen::Vector3f s = NiMath::ToEigen(vS).normalized();
		Eigen::Vector3f s_XY = Eigen::Vector3f(s.x(), s.y(), 0);
		Eigen::Vector3f i_XY = Eigen::Vector3f(vIdeal.x(), vIdeal.y(), 0);
		float i_yaw = std::atan2(s_XY.cross(i_XY).z(), s_XY.dot(i_XY));

		Eigen::Vector3f s_YZ = Eigen::Vector3f(0, s.y(), s.z());
		Eigen::Vector3f i_YZ = Eigen::Vector3f(0, vIdeal.y(), vIdeal.z());
		float i_pitch = std::atan2(s_YZ.cross(i_YZ).x(), s_YZ.dot(i_YZ));
		constexpr auto angle_tolerance = glm::radians(5.0f);
		if (i_yaw < angle_tolerance && i_pitch < angle_tolerance) {
			return;
		}
		Eigen::Matrix3f l = NiMath::ToEigen(local);

		const float yaw = NiMath::GetAngleXY(node->world.rotate) + RE::NI_PI;
		const auto neutralizeYaw = Eigen::AngleAxisf(-yaw, Eigen::Vector3f::UnitZ());
		auto tmpL = neutralizeYaw * l;

		auto pitchYaw = Eigen::AngleAxisf(i_yaw, Eigen::Vector3f::UnitZ());
		auto pitchRot = Eigen::AngleAxisf(i_pitch, Eigen::Vector3f::UnitX());
		tmpL = pitchYaw * pitchRot * tmpL;

		auto reapplyYaw = Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitZ());
		l = reapplyYaw * tmpL;
		local = NiMath::ToNiMatrix(l);

		RE::NiUpdateData data{
			0.5f,
			RE::NiUpdateData::Flag::kNone
		};
		node->Update(data);
	}

	void RotationData::Update(const RE::NiPoint3& a_newIdeal)
	{
		vIdeal = NiMath::ToEigen(a_newIdeal).normalized();
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
