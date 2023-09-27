#pragma once

namespace Registry
{
	enum CoordinateType : uint8_t
	{
		X = 0,
		Y,
		Z,
		R,

		Total
	};

	struct Coordinate
	{
		Coordinate(const RE::TESObjectREFR* a_ref) :
			location(a_ref->data.location.x, a_ref->data.location.y, a_ref->data.location.z), rotation(a_ref->data.angle.z) {}
		Coordinate(const std::vector<float>& a_coordinates) :
			location(glm::vec3{ a_coordinates[0], a_coordinates[1], a_coordinates[2] }), rotation(a_coordinates[3]) {}
		Coordinate(std::ifstream& a_stream) :
			location([&]() {
				glm::vec3 ret{};
				Decode::Read(a_stream, ret.x);
				Decode::Read(a_stream, ret.y);
				Decode::Read(a_stream, ret.z);
				return ret;
			}()),
			rotation([&]() {
				float ret;
				Decode::Read(a_stream, ret);
				return ret;
			}()) {}
		Coordinate() :
			location(glm::vec3{}), rotation(0.0f) {}
		~Coordinate() = default;

		template <typename T>
		void ToContainer(T& a_out) const
		{
			assert(a_out.size() >= 4);
			a_out[0] = location.x;
			a_out[1] = location.y;
			a_out[2] = location.z;
			a_out[3] = rotation;
		}
		RE::NiPoint3 AsNiPoint() const { return { location.x, location.y, location.z }; }
		float GetDistance(const Coordinate& a_other) const { return glm::distance(location, a_other.location); }

	public:
		bool operator==(const Coordinate& a_rhs) const { return location == a_rhs.location && rotation == a_rhs.rotation; }

	public:
		glm::vec3 location;
		float rotation;
	};

	class Transform
	{
	public:
		Transform(const Coordinate& a_rawcoordinates);
		Transform(std::ifstream& a_binarystream);
		Transform() = default;
		~Transform() = default;

	public:
		void Apply(Coordinate& a_coordinate) const;
		Coordinate ApplyCopy(const Coordinate& a_coordinate) const;
		bool HasChanges() const;

		const Coordinate& GetRawOffset() const;
		const Coordinate& GetOffset() const;
		void UpdateOffset(const Coordinate& a_coordinate);
		void UpdateOffset(float x, float y, float z, float rot);
		void UpdateOffset(float a_value, CoordinateType a_where);
		void ResetOffset();

		void Save(YAML::Node& a_node) const;
		void Load(const YAML::Node& a_node);

	private:
		Coordinate _offset;
		Coordinate _raw;
	};

}	 // namespace Registry
