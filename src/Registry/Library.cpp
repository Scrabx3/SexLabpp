#include "Library.h"

#include "Decode.h"

namespace Registry
{
  void Library::Initialize()
  {
    logger::info("Loading files..");

		const auto path = fs::path{ CONFIGPATH("Registry") };
		std::error_code ec{};
		if (!fs::exists(path, ec) || fs::is_empty(path, ec)) {
			const auto msg = ec ? fmt::format("An error occured while initializing SexLab animations.\nError: {}", ec.message()) :
														fmt::format("Unable to load SexLab animations. Folder path {} is empty or does not exist.", path.string());
			logger::critical("{}", msg);
			if (MessageBox(nullptr, fmt::format("{}\n\nExit game now?", msg).c_str(), "SexLab p+ Registry", 0x00000004) == 6)
				std::_Exit(EXIT_FAILURE);
			return;
		}

		const auto lock = std::shared_lock{ read_write_lock };
		std::vector<std::thread> threads;
		for (auto& file : fs::directory_iterator{ path }) {
			threads.emplace_back([this, file]() {
				const auto package = Decoder::Decode(file);
				packages.push_back(package);
			});
		}
		for (auto&& thread : threads) {
			thread.join();
		}
		// logger::info("Loaded {} Packages");
	}
}