#include "Papyrus/sslActorLibrary.h"
#include "Papyrus/sslAnimationSlots.h"
#include "Papyrus/sslSystemConfig.h"
#include "Papyrus/sslThreadLibrary.h"
#include "Papyrus/sslThreadModel.h"
#include "Registry/Library.h"

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kSaveGame:
		// Settings::StripConfig::GetSingleton()->Save();
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		Registry::Library::GetSingleton()->Initialize();
		if (!GameForms::LoadData()) {
			logger::critical("Unable to load esp objects");
			if (SKSE::WinAPI::MessageBox(nullptr, "Some game objects could not be loaded. This is usually due to a required game plugin not being loaded in your game. Please ensure that you have all requirements installed\n\nExit Game now? (Recommended yes)", "SexLab p+ Load Data", 0x00000004) == 6)
				std::_Exit(EXIT_FAILURE);
			return;
		}
		// Settings::LoadData();
		// Settings::StripConfig::GetSingleton()->Load();
		break;
	case SKSE::MessagingInterface::kNewGame:
	case SKSE::MessagingInterface::kPostLoadGame:
		break;
	}
}

#ifdef SKYRIM_SUPPORT_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("Scrab Joséline"sv);
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });
	return v;
}();
#else
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION.pack();
	return true;
}
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	const auto InitLogger = []() -> bool {
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path)
			return false;
		*path /= fmt::format(FMT_STRING("{}.log"), Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif
		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
#ifndef NDEBUG
		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);
#else
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
#endif
		spdlog::set_default_logger(std::move(log));
#ifndef NDEBUG
		spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);
#else
		spdlog::set_pattern("[%^%l%$] %v"s);
#endif

		logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
		return true;
	};

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	} else if (!InitLogger()) {
		logger::critical("Failed to initialize logger"sv);
		return false;
	}

	SKSE::Init(a_skse);
	logger::info("{} loaded"sv, Plugin::NAME);

	const auto msging = SKSE::GetMessagingInterface();
	if (!msging->RegisterListener("SKSE", SKSEMessageHandler)) {
		logger::critical("Failed to register Listener");
		return false;
	}

	const auto papyrus = SKSE::GetPapyrusInterface();
	if (!papyrus) {
		logger::critical("Failed to get papyurs interface");
		return false;
	}
	papyrus->Register(Papyrus::ActorLibrary::Register);
	papyrus->Register(Papyrus::AnimationSlots::Register);
	papyrus->Register(Papyrus::ThreadLibrary::Register);
	papyrus->Register(Papyrus::ThreadModel::Register);
	papyrus->Register(Papyrus::SystemConfig::Register);

	// SexLab::Hooks::Install();


	logger::info("Initialization complete");

	return true;
}
