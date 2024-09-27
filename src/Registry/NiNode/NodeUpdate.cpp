#include "NodeUpdate.h"

#include "NiMath.h"

namespace Registry::Collision
{
	void RotationData::ApplyRotation()
	{
		constexpr auto angle_tolerance = glm::radians(2.5f);
		// X -> Pitch (Schlong Angle Axis on Humans)
		// Y -> Roll
		// Z -> Yaw
		auto node = schlong->GetBaseReferenceNode();
		auto& local = node->local.rotate;

		const auto vS = schlong->GetSchlongVector();
		const auto vT = proj() - node->world.translate;
		Eigen::Vector3f s = NiMath::ToEigen(vS).normalized();
		Eigen::Vector3f v = NiMath::ToEigen(vT).normalized();

		const Eigen::Quaternionf worldQuat(NiMath::ToEigen(node->world.rotate));
		const Eigen::Quaternionf localQuat(NiMath::ToEigen(local));
		auto tmpQuat = worldQuat.conjugate() * localQuat;

		Eigen::Vector3f sXZ = Eigen::Vector3f(s.x(), 0, s.z());
		Eigen::Vector3f vXZ = Eigen::Vector3f(v.x(), 0, v.z());
		float aRoll = std::atan2(sXZ.cross(vXZ).x(), sXZ.dot(vXZ));
		if (std::abs(aRoll) > angle_tolerance) {
			const auto rotRoll = Eigen::AngleAxisf(-aRoll, Eigen::Vector3f::UnitX());
			tmpQuat = rotRoll * tmpQuat;
		}

		Eigen::Vector3f sXY = Eigen::Vector3f(s.x(), s.y(), 0);
		Eigen::Vector3f vXY = Eigen::Vector3f(v.x(), v.y(), 0);
		float aYaw = std::atan2(sXY.cross(vXY).z(), sXY.dot(vXY));
		if (std::abs(aYaw) > angle_tolerance) {
			const auto rotYaw = Eigen::AngleAxisf(-aYaw, Eigen::Vector3f::UnitZ());
			tmpQuat = rotYaw * tmpQuat;
		}

		Eigen::Quaternionf finalQuat = worldQuat * tmpQuat;
		local = NiMath::ToNiMatrix(finalQuat.toRotationMatrix());

		RE::NiUpdateData data{ 0.5f, RE::NiUpdateData::Flag::kNone };
		node->Update(data);
	}

	void RotationData::Update(const RE::NiPointer<RE::NiNode>& a_target)
	{
		proj = [a_target]() {
			return a_target->world.translate;
		};
	}

	void RotationData::Update(const std::function<RE::NiPoint3()>& a_proj)
	{
		proj = a_proj;
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

	void NodeUpdate::DeleteSkew(const std::shared_ptr<Schlong>& a_node)
	{
		std::scoped_lock lk{ _m };
		auto w = std::ranges::find_if(skews, [&](auto& it) { return it == a_node; });
		if (w != skews.end())
			skews.erase(w);
	}

}	 // namespace Registry::Collision
