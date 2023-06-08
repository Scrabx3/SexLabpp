#include "Decode.h"

namespace Registry
{
	std::unique_ptr<AnimPackage> Decoder::Decode(const fs::path a_file)
	{
		std::ifstream stream(a_file, std::ios::binary);
		stream.unsetf(std::ios::skipws);
		stream.exceptions(std::fstream::eofbit);
		stream.exceptions(std::fstream::badbit);
		stream.exceptions(std::fstream::failbit);

		uint8_t version;
		stream.read(reinterpret_cast<char*>(&version), 1);
		switch (version) {
		case 1:
			return Version1(stream);
		default:
			throw std::runtime_error(fmt::format("Invalid version {}", version).c_str());
		}
	}

	std::unique_ptr<AnimPackage> Decoder::Version1(std::ifstream& a_stream)
	{
		constexpr auto hashcount = 4;
		constexpr auto idcount = 8;

		const auto readNumeric = [&]<typename T>(T& a_out) {
			constexpr size_t n = sizeof(T);
			uint8_t buffer[n] = {};
			a_stream.read(reinterpret_cast<char*>(buffer), n);
			a_out = 0;
			for (int32_t i = 0; i < n; i++) {
				a_out = (a_out << 8) | buffer[i];
			}
		};
		const auto readFloat = [&](float& a_out) {
			int32_t tmp;
			readNumeric(tmp);
			a_out = static_cast<float>(tmp) / 1000.0f;
		};
		const auto readString = [&](std::string& a_out) {
			uint64_t u64;
			readNumeric(u64);
			std::vector<char> buffer;
			buffer.resize(u64);
			a_stream.read(buffer.data(), u64);
			a_out = std::string{ buffer.begin(), buffer.end() };
		};
		const auto getLoopCountAndReserve = [&]<typename V>(std::vector<V>& a_vec) -> uint64_t {
			uint64_t count;
			readNumeric(count);
			a_vec.reserve(count);
			return count;
		};

		auto package = std::make_unique<AnimPackage>();
		readString(package->name);
		readString(package->author);
		package->hash.resize(hashcount);
		a_stream.read(package->hash.data(), hashcount);

		uint64_t scene_count = getLoopCountAndReserve(package->scenes);
		for (size_t i = 0; i < scene_count; i++) {
			// ------------------------- SCENE
			package->scenes.push_back(std::make_unique<Scene>(package->author, package->hash));
			auto& scene = package->scenes.back();
			scene->id.resize(idcount);
			a_stream.read(scene->id.data(), idcount);
			readString(scene->name);
			uint64_t position_info_count = getLoopCountAndReserve(scene->positions);
			for (size_t n = 0; n < position_info_count; n++) {
				// ------------------------- POSITION INFO
				auto& info = scene->positions.emplace_back();
				a_stream.read(reinterpret_cast<char*>(&info.race), 1);
				a_stream.read(reinterpret_cast<char*>(&info.sex), 1);
				readFloat(info.scale);
				a_stream.read(reinterpret_cast<char*>(&info.extra), 1);
			}
			std::string startstage(idcount, 'X');
			a_stream.read(startstage.data(), idcount);

			uint64_t stage_count = getLoopCountAndReserve(scene->stages);
			for (size_t n = 0; n < stage_count; n++) {
				// ------------------------- STAGE
				scene->stages.push_back(std::make_unique<Stage>());
				auto& stage = scene->stages.back();
				stage->id.resize(idcount);
				a_stream.read(stage->id.data(), idcount);
				if (startstage == stage->id)
					scene->start_animation = stage.get();

				uint64_t position_count = getLoopCountAndReserve(stage->positions);
				if (position_count != position_info_count)
					throw std::runtime_error(fmt::format("Invalid position count; expected {} but got {}", position_info_count, position_count).c_str());
				for (size_t j = 0; j < position_count; j++) {
					// ------------------------- POSITION
					auto& position = stage->positions.emplace_back();
					readString(position.event);
					a_stream.read(reinterpret_cast<char*>(&position.climax), 1);
					readFloat(position.offset[Offset::X]);
					readFloat(position.offset[Offset::Y]);
					readFloat(position.offset[Offset::Z]);
					readFloat(position.offset[Offset::R]);
				}
				readFloat(stage->fixedlength);
				readString(stage->navtext);
				uint64_t tag_count;
				readNumeric(tag_count);
				for (size_t j = 0; j < tag_count; j++) {
					// ------------------------- TAGS
					std::string tag;
					readString(tag);
					if (TagHandler::AddTag(stage->tags, tag))
						continue;
					stage->extratags.push_back(tag);
				}
			}
			if (!scene->start_animation)
				throw std::runtime_error(fmt::format("Start animation is not defined for scene {}", scene->id).c_str());

			const auto getStage = [&](const std::string_view id) -> Stage* {
				for (auto&& stage : scene->stages)
					if (stage->id == id)
						return stage.get();
				throw std::runtime_error(fmt::format("Unrecognized stage referenced in graph: {}", id).c_str());
			};

			uint64_t graph_count = getLoopCountAndReserve(scene->graph);
			if (graph_count != stage_count)
				throw std::runtime_error(fmt::format("Invalid stage count; expected {} but got {}", stage_count, graph_count).c_str());
			for (size_t n = 0; n < graph_count; n++) {
				// ------------------------- GRAPH
				std::string keystage(idcount, 'X');
				a_stream.read(keystage.data(), idcount);
				auto& list = scene->graph.emplace_back(std::make_pair(
					getStage(keystage),
					std::forward_list<Stage*>{}));

				uint64_t edge_count;
				readNumeric(edge_count);
				for (size_t j = 0; j < edge_count; j++) {
					std::string edgestage(idcount, 'X');
					a_stream.read(edgestage.data(), idcount);
					list.second.push_front(getStage(edgestage));
				}
			}
			a_stream.read(reinterpret_cast<char*>(&scene->furnituredata.allowbed), 1);
		}
		return package;
	}

}	 // namespace Registry
