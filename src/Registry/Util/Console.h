#pragma once

#include "Scale.h"

namespace Registry::Console
{
  struct CompileAndRun
	{
		RE::TESObjectREFR* ParseTargetRef(std::string_view a_targetStr) const
		{
			if (a_targetStr == "player") {
				return RE::PlayerCharacter::GetSingleton();
			}
			try {
				return FormFromString<RE::TESObjectREFR*>(a_targetStr, 16);
			} catch (const std::exception&) {
				return nullptr;
			}
		}

		RE::Actor* GetTargetActor(std::string_view a_targetStr, RE::TESObjectREFR* a_targetRef) const
		{
			if (!a_targetStr.empty()) {
				const auto tmp = ParseTargetRef(a_targetStr);
				return tmp ? tmp->As<RE::Actor>() : nullptr;
			}
			if (a_targetRef) {
				return a_targetRef->As<RE::Actor>();
			}
			return nullptr;
		}

		static void thunk(RE::Script* a_script, RE::ScriptCompiler* a_compiler, RE::COMPILER_NAME a_name, RE::TESObjectREFR* a_targetRef)
		{
			std::string cmd{ a_script->GetCommand() };
			ToLower(cmd);
			if (!cmd.starts_with("sexlab")) {
        return func(a_script, a_compiler, a_name, a_targetRef);
      }

			if (!a_targetRef) {
				PrintConsole("Missing Target");
			}

			if (cmd.ends_with("getscale")) {
				auto actorRef = a_targetRef->As<RE::Actor>();
				if (!actorRef) {
					PrintConsole("Target should be an actor");
				}

				float scale = Scale::GetSingleton()->GetScale(a_targetRef);
        auto race = RaceHandler::GetRaceKey(actorRef);
				std::string output = fmt::format("Scale = {} | Race = {}", scale, RaceHandler::AsString(race));
				PrintConsole(output.c_str());
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(52065, 52952), OFFSET(0xE2, 0x52) };
		stl::write_thunk_call<CompileAndRun>(target.address());
	}
}