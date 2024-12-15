#include "Transform.h"

#include <glm/gtx/transform.hpp>
#include <numbers>

#include "Registry/Util/Decode.h"

namespace Registry
{
	Coordinate::Coordinate(const RE::TESObjectREFR* a_ref) :
		location(a_ref->data.location.x, a_ref->data.location.y, a_ref->data.location.z), rotation(a_ref->data.angle.z) {}
	Coordinate::Coordinate(const RE::NiPoint3& a_point, float a_rotation) :
		location(a_point.x, a_point.y, a_point.z), rotation(a_rotation) {}
	Coordinate::Coordinate(const std::vector<float>& a_coordinates) :
		location(glm::vec3{ a_coordinates[0], a_coordinates[1], a_coordinates[2] }), rotation(a_coordinates[3])
	{}
	Coordinate::Coordinate(std::ifstream& a_stream) :
		location([&]() {
			glm::vec3 ret{};
			Decode::Read(a_stream, ret.x);
			Decode::Read(a_stream, ret.y);
			Decode::Read(a_stream, ret.z);
			return ret;
		}()),
		rotation(Decode::Read<float>(a_stream)) {}

	void Coordinate::Apply(Coordinate& a_coordinate) const
	{
		const auto rotate = glm::rotate(glm::mat4(1.0f), a_coordinate.rotation, glm::vec3(0, 0, 1));
		const auto transform = glm::mat3{ rotate } * location;
		a_coordinate.location += transform;

		if (this->rotation) {
			a_coordinate.rotation += this->rotation;
		}
	}

	Transform::Transform(const Coordinate& a_rawoffset) :
		_raw(a_rawoffset), _offset(a_rawoffset) {}

	Transform::Transform(std::ifstream& a_binarystream) :
		_raw(a_binarystream), _offset(_raw) {}

	const Coordinate& Transform::GetRawOffset() const
	{
		return _raw;
	}

	const Coordinate& Transform::GetOffset() const
	{
		return _offset;
	}

	void Transform::UpdateOffset(const Coordinate& a_newoffset)
	{
		_offset = a_newoffset;
	}

	void Transform::UpdateOffset(float x, float y, float z, float rot)
	{
		_offset.location = {
			x, y, z
		};
		_offset.rotation = glm::radians(rot);
	}

	void Transform::UpdateOffset(float a_value, CoordinateType a_type)
	{
		switch (a_type) {
		case CoordinateType::X:
			_offset.location.x += a_value;
			break;
		case CoordinateType::Y:
			_offset.location.y += a_value;
			break;
		case CoordinateType::Z:
			_offset.location.z += a_value;
			break;
		case CoordinateType::R:
			_offset.rotation += glm::radians(a_value);
			break;
		}
	}

	void Transform::ResetOffset()
	{
		_offset = _raw;
	}

	Coordinate Transform::ApplyCopy(const Coordinate& a_coordinate) const
	{
		Coordinate ret{ a_coordinate };
		Apply(ret);
		return ret;
	}

	void Transform::Save(YAML::Node& a_node) const
	{
		auto loc = a_node["Location"];
		loc[0] = _offset.location.x;
		loc[1] = _offset.location.y;
		loc[2] = _offset.location.z;

		a_node["Rotation"] = _offset.rotation;
	}

	void Transform::Load(const YAML::Node& a_node)
	{
		if (auto loc = a_node["Location"]; loc.IsDefined() && loc.size() == 3) {
			_offset.location.x = loc[0].as<float>();
			_offset.location.y = loc[1].as<float>();
			_offset.location.z = loc[2].as<float>();
		}
		if (auto rot = a_node["Rotation"]; rot.IsDefined()) {
			_offset.rotation = rot.as<float>();
		}
	}

	bool Transform::HasChanges() const
	{
		return _offset != _raw;
	}

}
