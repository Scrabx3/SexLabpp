#pragma once

#pragma warning(push)
#pragma warning(disable : 4200)
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#pragma warning(pop)

#include <atomic>
#include <glm/glm.hpp>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <ranges>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
static_assert(magic_enum::is_magic_enum_supported);

#pragma warning(push)
#ifdef NDEBUG
#include <spdlog/sinks/basic_file_sink.h>
#else
#include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

namespace logger = SKSE::log;
namespace fs = std::filesystem;
using namespace std::literals;

#include "Random.h"
#include "GameForms.h"
#include "Registry/Misc.h"
#include "Script.h"
#include "Singleton.h"
#include "UserData/Settings.h"

#define ESPNAME "SexLab.esm"
constexpr auto LEGACY_CONFIG { "Data\\SKSE\\Plugins\\SexLab\\" };
constexpr auto YAMLPATH{ "Data\\SKSE\\SexLab\\Settings.yaml" };
constexpr auto INIPATH{ "Data\\SKSE\\Plugins\\SexLab.ini" };

#define CONFIGPATH(path) "Data\\SKSE\\SexLab\\" path
#define SCENEPATH CONFIGPATH("Registry")
#define FURNITUREPATH CONFIGPATH("Furniture")
#define VOICEPATH CONFIGPATH("Voices")
#define EXPRESSIONPATH CONFIGPATH("Expressions")
#define STRIPPINGPATH CONFIGPATH("Stripping.yaml")
#define SCHLONGPATH CONFIGPATH("SchlongsOfSkyrim.yaml")

#define USERDATAPATH(path) CONFIGPATH("UserData\\") path
#define VOICESETTINGPATH USERDATAPATH("Voices.yaml")
#define VOICE_NPCPATH USERDATAPATH("Voices_NPC.yaml")
#define SCENESETTINGPATH USERDATAPATH("Scenes")

#define MESSAGEBOX REX::W32::MessageBoxA

#ifdef SKYRIM_SUPPORT_AE
#define OFFSET(SE, AE) AE
#else
#define OFFSET(SE, AE) SE
#endif

namespace Decode
{
	static inline constexpr size_t HASH_SIZE = 4;
	static inline constexpr size_t ID_SIZE = 8;

	template <typename I, std::enable_if_t<std::is_integral<I>::value, bool> = true>
	void Read(std::ifstream& a_stream, I& a_out)
	{
		constexpr size_t n = sizeof(I);
		uint8_t buffer[n];
		a_stream.read(reinterpret_cast<char*>(buffer), n);
		a_out = 0;
		for (int32_t i = 0; i < n; i++) {
			a_out = (a_out << 8) | buffer[i];
		}
	}

	template <typename F, std::enable_if_t<std::is_floating_point<F>::value, bool> = true>
	void Read(std::ifstream& a_stream, F& a_out)
	{
		int32_t tmp;
		Read(a_stream, tmp);
		a_out = static_cast<float>(tmp) / 1000.0f;
	}

	template <typename S, std::enable_if_t<std::is_same<S, std::string>::value ||
																					 std::is_same<S, RE::BSFixedString>::value,
													bool> = true>
	void Read(std::ifstream& a_stream, S& a_out)
	{
		uint64_t u64;
		Read(a_stream, u64);
		std::vector<char> buffer;
		buffer.resize(u64);
		a_stream.read(buffer.data(), u64);
		a_out = std::string{ buffer.begin(), buffer.end() };
	}

	template <typename T>
	T Read(std::ifstream& a_stream)
	{
		T ret;
		Read(a_stream, ret);
		return ret;
	}
}

namespace stl
{
	using namespace SKSE::stl;

	constexpr std::uint32_t version_pack(REL::Version a_version) noexcept
	{
		return static_cast<std::uint32_t>(
			(a_version[0] & 0x0FF) << 24u |
			(a_version[1] & 0x0FF) << 16u |
			(a_version[2] & 0xFFF) << 4u |
			(a_version[3] & 0x00F) << 0u);
	}

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = SKSE::GetTrampoline();
		SKSE::AllocTrampoline(14);

		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

	template <class F, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[0] };
		T::func = vtbl.write_vfunc(T::size, T::thunk);
	}

	inline bool read_string(SKSE::SerializationInterface* a_intfc, std::string& a_str)
	{
		std::size_t size = 0;
		if (!a_intfc->ReadRecordData(size)) {
			return false;
		}
		a_str.reserve(size);
		if (!a_intfc->ReadRecordData(a_str.data(), static_cast<std::uint32_t>(size))) {
			return false;
		}
		return true;
	}

	template <class S>
	inline bool write_string(SKSE::SerializationInterface* a_intfc, const S& a_str)
	{
		std::size_t size = a_str.length() + 1;
		return a_intfc->WriteRecordData(size) && a_intfc->WriteRecordData(a_str.data(), static_cast<std::uint32_t>(size));
	}
}

namespace Papyrus
{
#define REGISTERFUNC(func, class, dont_delay) a_vm->RegisterFunction(#func##sv, class, func, dont_delay)

	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
}

namespace Registry
{
	struct FixedStringCompare
	{
		bool operator()(const RE::BSFixedString& lhs, const RE::BSFixedString& rhs) const
		{
			return strcmp(lhs.data(), rhs.data()) < 0;
		}
	};

	template <typename E>
	constexpr std::vector<E> FlagToComponents(E a_enum)
	{
		using underlying = typename std::underlying_type<E>::type;
		constexpr auto iterations = sizeof(underlying) * 8;
		auto number = static_cast<underlying>(a_enum);
		std::vector<E> ret{};
		for (size_t n = 0; n < iterations; n++) {
			size_t i = 1ULL << n;
			if (number & i) {
				ret.push_back(E(i));
			}
		}
		return ret;
	}

	template <typename E>
	constexpr size_t FlagIndex(E a_enum)
	{
		using underlying = typename std::underlying_type<E>::type;
		constexpr auto iterations = sizeof(underlying) * 8;
		auto number = static_cast<underlying>(a_enum);
		for (size_t n = 0; n < iterations; n++) {
			size_t i = 1ULL << n;
			if (number & i) {
				return n;
			}
		}
		return 0;
	}

	template <typename E>
	constexpr size_t CountFlagSize()
	{
		auto max = static_cast<size_t>(E::Total) - 1, ret = 0;
		while ((1 << ret++) < max) {}
		return ret;
	}
}

template <>
struct std::formatter<RE::BSFixedString> : std::formatter<const char*>
{
	template <typename FormatContext>
	auto format(const RE::BSFixedString& myStr, FormatContext& ctx)
	{
		return std::formatter<const char*>::format(myStr.data(), ctx);
	}
};

template <typename T>
struct std::formatter<std::vector<T>> : std::formatter<T>
{
	template <typename FormatContext>
	auto format(const std::vector<T>& vec, FormatContext& ctx)
	{
		auto out = ctx.out();
		for (size_t i = 0; i < vec.size(); ++i) {
			if (i > 0) {
				out = std::format_to(out, ",");
			}
			out = std::formatter<T>::format(vec[i], ctx);
		}
		return out;
	}
};

#define DLLEXPORT __declspec(dllexport)

// #include "Plugin.h"
