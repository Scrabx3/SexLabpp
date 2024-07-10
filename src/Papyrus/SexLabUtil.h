#pragma once

namespace Papyrus::SexLabUtil
{
	bool HasKeywordSub(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_form, std::string_view a_substring)
	{
		if (!a_form) {
			a_vm->TraceStack("Form is none", a_stackID);
			return false;
		}
		const auto& kwd = a_form->As<RE::BGSKeywordForm>();
		return kwd && kwd->ContainsKeywordString(a_substring);
	}
	RE::BSFixedString RemoveSubString(RE::StaticFunctionTag*, std::string a_str, std::string a_substring)
  {
    const auto where = a_str.find(a_substring);
    if (where == std::string::npos)
			return a_str;
		a_str.erase(where, a_substring.length());
		return a_str;
	}
	void PrintConsole(RE::StaticFunctionTag*, std::string a_str)
	{
		if (a_str.empty())
			return;
		else if (a_str.size() < 1000)
			Registry::PrintConsole(a_str.c_str());
		else {	// Large strings printed to console crash the game - truncate it
			size_t i = 0;
			do
			{
				constexpr auto maxchar = 950;
				auto print = a_str.substr(i, i + maxchar);
				print += '\n';
				i += maxchar;
				Registry::PrintConsole(print.c_str());
			} while (i < a_str.size());
		}
	}

	int IntMinMaxIndex(RE::StaticFunctionTag*, std::vector<int> arr, bool findHighestValue)
  {
		if (arr.empty()) {
			return 0;
		}
		size_t w = 0;
    int m = 0;
		for (size_t i = 0; i < arr.size(); i++) {
			if (findHighestValue && arr[i] > m || !findHighestValue && arr[i] < m) {
				w = i;
				m = arr[i];
			}
		}
    return static_cast<int>(w);
	}
	int IntMinMaxValue(RE::StaticFunctionTag*, std::vector<int> arr, bool findHighestValue)
	{
		if (arr.empty()) {
			return 0;
		}
		return arr[IntMinMaxIndex(nullptr, arr, findHighestValue)];
	}
	int FloatMinMaxIndex(RE::StaticFunctionTag*, std::vector<float> arr, bool findHighestValue)
	{
		if (arr.empty()) {
			return 0;
		}
		size_t w = 0;
    float m = 0;
		for (size_t i = 0; i < arr.size(); i++) {
			if (findHighestValue && arr[i] > m || !findHighestValue && arr[i] < m) {
				w = i;
				m = arr[i];
			}
		}
		return static_cast<int>(w);
	}
	float FloatMinMaxValue(RE::StaticFunctionTag*, std::vector<float> arr, bool findHighestValue)
	{
		if (arr.empty()) {
			return 0;
		}
		return arr[FloatMinMaxIndex(nullptr, arr, findHighestValue)];
	}
	std::vector<RE::Actor*> MakeActorArray(RE::StaticFunctionTag*, RE::Actor* a1, RE::Actor* a2, RE::Actor* a3, RE::Actor* a4, RE::Actor* a5)
	{
		std::vector<RE::Actor*> ret{};
		ret.reserve(5);
		if (a1)
			ret.push_back(a1);
		if (a2)
			ret.push_back(a2);
		if (a3)
			ret.push_back(a3);
		if (a4)
			ret.push_back(a4);
		if (a5)
			ret.push_back(a5);
		return ret;
	}

	float GetCurrentGameRealTime(RE::StaticFunctionTag*)
	{
    constexpr auto seconds_per_day = 86400.0f;
		const auto calendar = RE::Calendar::GetSingleton();
		const auto timescale = std::max<float>(1, calendar->GetTimescale());
		return (calendar->GetCurrentGameTime() / timescale) * seconds_per_day;
  }

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNC(HasKeywordSub, "SexLabUtil", true);
		REGISTERFUNC(RemoveSubString, "SexLabUtil", true);
		REGISTERFUNC(PrintConsole, "SexLabUtil", true);
		REGISTERFUNC(IntMinMaxIndex, "SexLabUtil", true);
		REGISTERFUNC(IntMinMaxValue, "SexLabUtil", true);
		REGISTERFUNC(FloatMinMaxIndex, "SexLabUtil", true);
		REGISTERFUNC(FloatMinMaxValue, "SexLabUtil", true);
		REGISTERFUNC(MakeActorArray, "SexLabUtil", true);
		REGISTERFUNC(GetCurrentGameRealTime, "SexLabUtil", true);

		return true;
	}

} // namespace Papyrus::SexLabUtil
