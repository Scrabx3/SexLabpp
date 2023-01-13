#pragma once

#include "Settings.h"

namespace SexLab
{
	enum Gender : uint8_t
	{
		Male = 1 << 0,
		Female = 1 << 1,
		Futa = 1 << 2,

		Crt_Male = 1 << 3,
		Crt_Female = 1 << 4,

		Overwrite_Male = 1 << 5,
		Overwrite_Female = 1 << 6,

		UNDEFINED = 1 << 7
	};

	// Assume to only be called for human actors with female base sex
	inline bool IsFuta(const RE::Actor* a_actor)
	{
		static const auto sosfaction = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x00AFF8, "Schlongs of Skyrim.esp");
		if (!sosfaction)
			return false;

		const auto base = a_actor->GetActorBase();
		if (!base)
			return false;

		auto ret = false;
		for (auto&& f : base->factions) {
			if (f.faction == sosfaction)
				ret = true;
			if (std::find(Settings::SOS_ExcludeFactions.begin(), Settings::SOS_ExcludeFactions.end(), f.faction) != Settings::SOS_ExcludeFactions.end())
				return false;
		}
		return a_actor->IsInFaction(sosfaction);
	}

	inline bool IsNPC(const RE::Actor* a_actor)
	{
		static const auto ActorTypeNPC = RE::TESForm::LookupByID<RE::BGSKeyword>(0x13794);
		return a_actor->HasKeyword(ActorTypeNPC);
	}

	inline bool IsVampire(const RE::Actor* a_actor)
	{
		static const auto vampire = RE::TESForm::LookupByID<RE::BGSKeyword>(0xA82BB);
		return a_actor->HasKeyword(vampire);
	}

	inline SKSE::stl::enumeration<Gender, std::uint8_t> GetGender(const RE::Actor* a_actor)
	{
		const auto base = a_actor->GetActorBase();
		if (!base)
			return { Gender::UNDEFINED };

		SKSE::stl::enumeration<Gender, std::uint8_t> ret{};
		switch (base->GetSex()) {
		case RE::SEXES::kMale:
			ret.set(IsNPC(a_actor) ? Gender::Male : Gender::Crt_Male);
			break;
		case RE::SEXES::kFemale:
			if (IsNPC(a_actor)) {
				if (IsFuta(a_actor)) {
					ret.set(Gender::Futa);
				} else {
					ret.set(Gender::Female);
				}
			} else {
				ret.set(Gender::Crt_Female);
			}
			break;
		default:
			return { Gender::UNDEFINED };
		}

		for (auto&& f : base->factions) {
			if (f.faction == GameForms::GenderFaction) {
				switch (f.rank) {
				case 0:
					ret.set(Gender::Overwrite_Male);
					break;
				case 1:
					ret.set(Gender::Overwrite_Female);
					break;
				}
				break;
			}
		}

		return ret;
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

	inline bool GetIsBed(const RE::TESObjectREFR* a_reference)
	{
		if (a_reference->GetName()[0] == '\0')
			return false;
		const auto root = a_reference->Get3D();
		const auto extra = root ? root->GetExtraData("FRN") : nullptr;
		const auto node = extra ? netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra) : nullptr;
		return node && !node->markers.empty() && node->markers[0].animationType.all(RE::BSFurnitureMarker::AnimationType::kSleep);
	}

	template <class T>
	std::string ToStringVec(T v)
	{
		std::stringstream ss;
		for(size_t i = 0; i < v.size(); ++i)
		{
			if(i != 0)
				ss << ",";
			ss << v[i];
		}
		std::string s = ss.str();
		return s;
	}

}	 // namespace SexLab
