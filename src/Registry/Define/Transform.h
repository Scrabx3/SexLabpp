#pragma once

namespace Registry
{
	enum Offset : uint8_t
	{
		X = 0,
		Y,
		Z,
		R,

		Total
	};

	class Transform
	{
	public:
		Transform(const std::array<float, Offset::Total>& a_rawoffset);
		Transform(std::ifstream& a_binarystream);
		Transform() = default;
		~Transform() = default;

	public:
		void Apply(std::array<float, 4>& a_coordinate) const;

		const std::array<float, Offset::Total>& GetRawOffset() const;
		const std::array<float, Offset::Total>& GetOffset() const;
		void UpdateOffset(const std::array<float, Offset::Total>& a_newoffset);

		void Save(YAML::Node& a_node) const;
		void Load(const YAML::Node& a_node);

	private:
		std::array<float, Offset::Total> _offset;
		std::array<float, Offset::Total> _raw;
	};

} // namespace Registry
