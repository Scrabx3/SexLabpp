#include "Transform.h"

namespace Registry
{
	Transform::Transform(const std::array<float, Offset::Total>& a_rawoffset) :
		_raw(a_rawoffset), _offset(a_rawoffset) {}

	Transform::Transform(std::ifstream& a_binarystream)
	{
		Decode::Read(a_binarystream, _raw[Offset::X]);
		Decode::Read(a_binarystream, _raw[Offset::Y]);
		Decode::Read(a_binarystream, _raw[Offset::Z]);
		Decode::Read(a_binarystream, _raw[Offset::R]);

    _offset = _raw;
	}

	void Transform::Apply(std::array<float, 4>& a_coordinate) const
	{
		const auto cos_theta = std::cosf(a_coordinate[3]);
		const auto sin_theta = std::sinf(a_coordinate[3]);

		a_coordinate[0] += (_offset[0] * cos_theta) - (_offset[1] * sin_theta);
		a_coordinate[1] += (_offset[0] * sin_theta) + (_offset[1] * cos_theta);
		a_coordinate[2] += _offset[2];
		a_coordinate[3] += _offset[3];
	};
}