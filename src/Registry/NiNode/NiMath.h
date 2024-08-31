#pragma once

#include <Eigen/Dense>

namespace Registry::Collision::NiMath
{
	using Segment = std::pair<RE::NiPoint3, RE::NiPoint3>;
	Segment ClosestSegmentBetweenSegments(const Segment& u, const Segment& v);

	Eigen::Vector3f ToEigen(const RE::NiPoint3& a_point);
	Eigen::Matrix3f ToEigen(const RE::NiMatrix3& a_point);
	RE::NiMatrix3 AsNiMatrix(const Eigen::Matrix3f& a_mat);

	/// @brief Obtain an angle s.t. ret * vector aligns with ideal (0°)
	/// @param vector The vector to align
	/// @param ideal The ideal rotation
	/// @return A rotation to transform vector with
	Eigen::AngleAxisf AlignAxis(const Eigen::Vector3f& vector, const Eigen::Vector3f& ideal);
	
	/// @brief Create a rotation matrix using Rodrigues Formula
	/// @param v The vector to align, normalized
	/// @param i The ideal rotation of the vector, normalized
	/// @return Rotation matrix s.t. angle(v * M, i) == 0
	RE::NiMatrix3 Rodrigue(const RE::NiPoint3& v, const RE::NiPoint3& i);
	Eigen::Matrix3f Rodrigue(const Eigen::Vector3f& v, const Eigen::Vector3f& i);

	/// @brief Perform least squares on the given set of points
	/// @param a_points The points to perform the analysis on
	/// @return A best-fit segment reaching from the first point and ending at the last
	Eigen::Matrix3f LeastSquares(const std::vector<RE::NiPoint3>& a_points);
	Eigen::Matrix3f LeastSquares(const std::vector<Eigen::Vector3f>& a_points);

	/// @brief Compute the Angle between v1 and v2, in radians
	/// @param v1 The first vector
	/// @param v2 The second vector
	/// @return The angle, in radians
	float GetAngle(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2);

	/// @brief Get the angle between v1 and v2
	/// @param v1 The first vector
	/// @param v2 The second vector
	/// @return Angle, in degree
	float GetAngleDegree(const RE::NiPoint3& v1, const RE::NiPoint3& v2);

}	 // namespace Registry::Collision::NiMath
