#pragma once

namespace Registry
{
	inline void PrintConsole(std::string_view a_msg) { RE::ConsoleLog::GetSingleton()->Print(a_msg.data()); }
	inline void PrintConsole(const char* a_msg) { RE::ConsoleLog::GetSingleton()->Print(a_msg); }
	template <typename... Args>
	inline void PrintConsole(fmt::format_string<Args...> a_fmt, Args&&... args)
	{
		const auto msg = fmt::format(a_fmt, std::forward<Args>(args)...);
		RE::ConsoleLog::GetSingleton()->Print(msg.data());
	}


	bool IsFuta(RE::Actor* a_actor);	 // Assumes female base sex
	bool IsNPC(const RE::Actor* a_actor);
	bool IsVampire(const RE::Actor* a_actor);

	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor);
	void SetVehicle(RE::Actor* a_this, RE::TESObjectREFR* a_vehicle);

	template <class T>
	constexpr void ToLower(T& str)
	{
		std::transform(str.cbegin(), str.cend(), str.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
	}
	template <class T>
	constexpr T AsLower(T str)
	{
		std::transform(str.cbegin(), str.cend(), str.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
		return str;
	}
	template <class T, class U>
	bool StringCmpCI(T lhs, U rhs)
	{
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
			[](char lhs, char rhs) { return tolower(lhs) == tolower(rhs); });
	}

	std::vector<std::string_view> StringSplit(const std::string_view a_view, const char a_delim = ',');
}