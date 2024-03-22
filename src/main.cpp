#include "Papyrus/SexLabRegistry.h"
#include "Papyrus/SexLabUtil.h"
#include "Papyrus/sslActorAlias.h"
#include "Papyrus/sslActorLibrary.h"
#include "Papyrus/sslActorStats.h"
#include "Papyrus/sslAnimationSlots.h"
#include "Papyrus/sslCreatureAnimationSlots.h"
#include "Papyrus/sslSystemConfig.h"
#include "Papyrus/sslThreadLibrary.h"
#include "Papyrus/sslThreadModel.h"
#include "Registry/Library.h"
#include "Registry/Stats.h"
#include "Serialization.h"
#include "UserData/StripData.h"

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kPostLoad:
#ifdef NDEBUG
		Registry::Library::GetSingleton()->Initialize();
		Registry::Library::GetSingleton()->Load();
		Settings::Initialize();
#endif
		break;
	case SKSE::MessagingInterface::kDataLoaded:
#ifndef NDEBUG
		Registry::Library::GetSingleton()->Initialize();
		Registry::Library::GetSingleton()->Load();
		Settings::Initialize();
#endif
		if (!GameForms::LoadData()) {
			logger::critical("Unable to load esp objects");
			if (MessageBox(nullptr, "Some game objects could not be loaded. This is usually due to a required game plugin not being loaded in your game. Please ensure that you have all requirements installed\n\nExit Game now? (Recommended yes)", "SexLab p+ Load Data", 0x00000004) == 6)
				std::_Exit(EXIT_FAILURE);
			return;
		}
		UserData::StripData::GetSingleton()->Load();
		Settings::InitializeData();
		break;
	case SKSE::MessagingInterface::kSaveGame:
		Settings::Save();
		Registry::Library::GetSingleton()->Save();
		UserData::StripData::GetSingleton()->Save();
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		break;
	}
}


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
		spdlog::set_pattern("%s(%#): [%T] [%^%l%$] %v"s);
#else
		spdlog::set_pattern("[%T] [%^%l%$] %v"s);
#endif

		logger::info("{} v{}", Plugin::NAME, Plugin::VERSION.string());
		return true;
	};

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible");
		return false;
	} else if (!InitLogger()) {
		logger::critical("Failed to initialize logger");
		return false;
	}

	SKSE::Init(a_skse);
	logger::info("{} loaded", Plugin::NAME);

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
	papyrus->Register(Papyrus::SexLabRegistry::Register);
	papyrus->Register(Papyrus::ActorAlias::Register);
	papyrus->Register(Papyrus::ActorLibrary::Register);
	papyrus->Register(Papyrus::AnimationSlots::Register);
	papyrus->Register(Papyrus::CreatureAnimationSlots::Register);
	papyrus->Register(Papyrus::ThreadLibrary::Register);
	papyrus->Register(Papyrus::ThreadModel::Register);
	papyrus->Register(Papyrus::SystemConfig::Register);
	papyrus->Register(Papyrus::ActorStats::Register);
	papyrus->Register(Papyrus::SexLabUtil::Register);

	// Hooks::Install();

	const auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID('slpp');
	serialization->SetSaveCallback(Serialization::Serialize::SaveCallback);
	serialization->SetLoadCallback(Serialization::Serialize::LoadCallback);
	serialization->SetRevertCallback(Serialization::Serialize::RevertCallback);
	serialization->SetFormDeleteCallback(Serialization::Serialize::FormDeleteCallback);

	Registry::Statistics::StatisticsData::GetSingleton()->Register();

	logger::info("Initialization complete");

	return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginName(Plugin::NAME.data());
	v.PluginVersion(Plugin::VERSION);
	v.AuthorName("Scrab Joséline"sv);
	v.UsesAddressLibrary(true);
	v.UsesStructsPost629();
	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}
