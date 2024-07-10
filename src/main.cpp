#include "Papyrus/Papyrus.h"
#include "Registry/Expression.h"
#include "Registry/Library.h"
#include "Registry/Stats.h"
#include "Registry/Util/Console.h"
#include "Registry/Voice.h"
#include "Serialization.h"
#include "UserData/StripData.h"

// class EventHandler :
// 	public Singleton<EventHandler>,
// 	public RE::BSTEventSink<RE::BSAnimationGraphEvent>
// {
// public:
// 	using EventResult = RE::BSEventNotifyControl;

// 	void Register()
// 	{
// 		RE::PlayerCharacter::GetSingleton()->AddAnimationGraphEventSink(this);
// 	}

// public:
// 	EventResult ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*) override
// 	{
// 		if (!a_event || a_event->holder->IsNot(RE::FormType::ActorCharacter))
// 			return EventResult::kContinue;

// 		auto source = const_cast<RE::Actor*>(a_event->holder->As<RE::Actor>());
// 		if (source->IsWeaponDrawn())
// 			logger::info("Tag = {} | Payload = {}", a_event->tag, a_event->payload);
// 		return EventResult::kContinue;
// 	}
// };

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kPostLoad:
		Settings::Initialize();
		Registry::Library::GetSingleton()->Initialize();
		Registry::Library::GetSingleton()->Load();
		Registry::Expression::GetSingleton()->Initialize();
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		Registry::Voice::GetSingleton()->Initialize();
		if (!GameForms::LoadData()) {
			logger::critical("Unable to load esp objects");
			if (MESSAGEBOX(nullptr, "Some game objects could not be loaded. This is usually due to a required game plugin not being loaded in your game. Please ensure that you have all requirements installed\n\nExit Game now? (Recommended yes)", "SexLab p+ Load Data", 0x00000004) == 6)
				std::_Exit(EXIT_FAILURE);
			return;
		}
		UserData::StripData::GetSingleton()->Load();
		Settings::InitializeData();
		break;
	case SKSE::MessagingInterface::kSaveGame:
		std::thread([]() {
			Settings::Save();
			Registry::Library::GetSingleton()->Save();
			Registry::Expression::GetSingleton()->Save();
			UserData::StripData::GetSingleton()->Save();
			Registry::Voice::GetSingleton()->Save();
		}).detach();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		// EventHandler::GetSingleton()->Register();
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

	if (!Papyrus::Register()) {
		logger::critical("Failed to register papyrus functions");
		return false;
	}

	// Registry::Console::Install();

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
