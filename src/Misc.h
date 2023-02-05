#pragma once

namespace SexLab
{

	bool IsFuta(const RE::Actor* a_actor);	 // Assumes female base sex
	bool IsNPC(const RE::Actor* a_actor);
	bool IsVampire(const RE::Actor* a_actor);

	bool IsBed(const RE::TESObjectREFR* a_reference);

	template <class T>
	std::string ToStringVec(T v)
	{
		std::stringstream ss;
		for (size_t i = 0; i < v.size(); ++i) {
			if (i != 0)
				ss << ",";
			ss << v[i];
		}
		std::string s = ss.str();
		return s;
	}

	template <class T>
	constexpr void ToLower(T& str)
	{
		std::transform(str.cbegin(), str.cend(), str.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
	}

	template <class T>
	bool IsEqualString(T lhs, T rhs)
	{
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
			[](char lhs, char rhs) { return tolower(lhs) == tolower(rhs); });
	}
}