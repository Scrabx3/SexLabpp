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
		Coordinate(const RE::TESObjectREFR* a_ref);
		Coordinate(const std::vector<float>& a_coordinates);
		Coordinate(std::ifstream& a_stream);
		Coordinate() = default;
		~Coordinate() = default;

		void Apply(Coordinate& a_coordinate) const;

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
		void Apply(Coordinate& a_coordinate) const { _offset.Apply(a_coordinate); }
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
