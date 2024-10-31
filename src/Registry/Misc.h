#pragma once

namespace Registry
{
	template <typename T>
	static T FormFromString(const std::string_view a_string, int base)
	{
		const auto split = a_string.find("|");
		const auto formid = std::stoi(a_string.substr(0, split).data(), nullptr, base);
		if (split == std::string_view::npos) {
			if constexpr (std::is_same<T, RE::FormID>::value) {
				return formid;
			} else {
				static_assert(std::is_pointer<T>::value);
				using U = std::remove_pointer<T>::type;
				return RE::TESForm::LookupByID<U>(formid);
			}
		}

		const auto esp = a_string.substr(split + 1);
		if constexpr (std::is_same<T, RE::FormID>::value) {
			return RE::TESDataHandler::GetSingleton()->LookupFormID(formid, esp);
		} else {
			static_assert(std::is_pointer<T>::value);
			using U = std::remove_pointer<T>::type;
			return RE::TESDataHandler::GetSingleton()->LookupForm<U>(formid, esp);
		}
	}
	template <typename T>
	static T FormFromString(const std::string_view a_string)
	{
		const size_t base = a_string.starts_with("0x") ? 16 : 10;
		return FormFromString<T>(a_string, base);
	}
	
	static std::string FormToString(RE::TESForm* a_form)
	{
		auto file = a_form->GetFile(0);
		if (!file) {
			return std::format("{}", a_form->GetFormID());
		}
		return std::format("{}|{}", a_form->GetLocalFormID(), file->GetFilename());
	}

	inline void PrintConsole(std::string_view a_msg) { RE::ConsoleLog::GetSingleton()->Print(a_msg.data()); }
	inline void PrintConsole(const char* a_msg) { RE::ConsoleLog::GetSingleton()->Print(a_msg); }
	template <typename... Args>
	inline void PrintConsole(std::format_string<Args...> a_fmt, Args&&... args)
	{
		const auto msg = std::format(a_fmt, std::forward<Args>(args)...);
		RE::ConsoleLog::GetSingleton()->Print(msg.data());
	}

	bool IsNPC(const RE::Actor* a_actor);
	bool IsVampire(const RE::Actor* a_actor);
	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor);

	template <class T>
	constexpr void ToLower(T& str)
	{
		std::transform(str.cbegin(), str.cend(), str.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
	}
	template <class T>
	constexpr T AsLower(T str)
	{
		ToLower(str);
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
