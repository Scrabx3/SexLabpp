#pragma once

#include <ranges>

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

inline bool GetIsBed(const RE::TESObjectREFR* a_reference)
{
	if (a_reference->GetName()[0] == '\0')
		return false;
	const auto root = a_reference->Get3D();
	const auto extra = root ? root->GetExtraData("FRN") : nullptr;
	const auto node = extra ? netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra) : nullptr;
	return node && !node->markers.empty() && node->markers[0].animationType.all(RE::BSFurnitureMarker::AnimationType::kSleep);
}

// int StringCompare()
