#include "CumFx.h"


RE::BSFixedString Registry::CumFx::PickRandomFxSet(FxType a_type) const
{
	if (fxList[a_type].empty()) {
		return "";
	}
	const auto i = Random::draw<size_t>(0, fxList[a_type].size() - 1);
	return fxList[a_type][i].first;
}

uint8_t Registry::CumFx::GetFxCount(FxType a_type, RE::BSFixedString a_set) const
{
	const auto it = std::find_if(fxList[a_type].begin(), fxList[a_type].end(), [&](const auto& pair) {
		return pair.first == a_set;
	});
	if (it != fxList[a_type].end()) {
		return it->second;
	}
	logger::error("FX set {} not found", a_set.c_str());
	return 0;
}

void Registry::CumFx::Initialize()
{
	if (!fs::exists(CUM_FX_PATH) || fs::is_empty(CUM_FX_PATH)) {
		auto code = REX::W32::MessageBoxA(nullptr, "CumFx path is empty or does not exist. Please check your installation.\n\nExit game now?", "SexLab p+ CumFx", 0x00000004);
		logger::error("CumFx path is empty or does not exist: {}", CUM_FX_PATH);
		if (code == 6) {
			std::_Exit(EXIT_FAILURE);
		}
		return;
	}
	for (size_t i = 0; i < FxType::Total; i++) {
		const auto fxName = magic_enum::enum_name(static_cast<FxType>(i));
		const auto path = std::format("{}{}", CUM_FX_PATH, fxName);
		if (!fs::exists(path) || fs::is_empty(path)) {
			logger::error("FX type path does not exist or is empty: {}", path);
			goto AFTER_DIRECTORY_ITERATOR;
		}
		for (const auto& profileEntry : fs::directory_iterator(path)) {
			if (!profileEntry.is_directory())
				continue;
			const auto typeCount = ParseType(profileEntry);
			if (!typeCount.has_value()) {
				logger::error("Failed to parse profile: {}", profileEntry.path().string());
				continue;
			}
			const auto profileName = profileEntry.path().filename().string();
			fxList[i].emplace_back(RE::BSFixedString(profileName), typeCount.value());
			logger::info("Loaded profile: {}", profileName);
		}
AFTER_DIRECTORY_ITERATOR:
		if (fxList[i].empty()) {
			logger::info("No profiles found for FX type: {}", fxName);
			if (i <= FxType::MainThree) {
				logger::error("Mandatory FX type {} is missing in {}", fxName, path);
				auto code = REX::W32::MessageBoxA(nullptr, "Mandatory Cum FX type is missing. Please check your installation.", "SexLab p+ CumFx", 0x00000004);
				if (code == 6) {
					std::_Exit(EXIT_FAILURE);
				}
			}
			continue;
		}
		logger::info("Loaded {} cum fx profiles of type {}", fxList[i].size(), fxName);
	}
}

std::optional<uint8_t> Registry::CumFx::ParseType(const fs::directory_entry& a_typePath)
{
	std::vector<uint8_t> fxFiles;
	for (const auto& file : fs::directory_iterator(a_typePath.path())) {
		if (!file.is_regular_file() || file.path().extension() != ".dds") {
			logger::warn("Invalid file type: {}. Expected .dds", file.path().string());
			continue;
		}
		std::string fileName = file.path().filename().string();
		size_t dotPos = fileName.find_last_of('.');
		std::string numberPart = fileName.substr(0, dotPos);
		try {
			size_t number = std::stoul(numberPart);
			if (number >= std::numeric_limits<uint8_t>::max()) {
				logger::warn("File number {} exceeds maximum value of 255", number);
				continue;
			}
			fxFiles.push_back(static_cast<uint8_t>(number));
		} catch (const std::exception& e) {
			logger::warn("Invalid number in file name: {}. Error: {}", numberPart, e.what());
			continue;
		}
	}
	if (fxFiles.empty()) {
		logger::error("No valid files found in directory: {}", a_typePath.path().string());
		return std::nullopt;
	}
	std::sort(fxFiles.begin(), fxFiles.end());
	if (fxFiles.front() != 1) {
		logger::error("First file number is not 1 in directory: {}", a_typePath.path().string());
		return std::nullopt;
	}
	for (size_t i = 0; i < fxFiles.size(); i++) {
		if (fxFiles[i] != (i + 1)) {
			logger::error("Missing file number {} in directory: {}", (i + 1), a_typePath.path().string());
			return std::nullopt;
		}
	}
	return static_cast<uint8_t>(fxFiles.size());
}
