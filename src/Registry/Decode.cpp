#include "Decode.h"

namespace Registry
{
	AnimPackage Decoder::Decode(const fs::path a_file)
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

	AnimPackage Decoder::Version1(std::ifstream& a_stream)
	{
		constexpr auto idcount = 8;
		constexpr auto hashcount = 4;

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

		AnimPackage package{};
		readString(package.name);
		readString(package.author);
		package.hash.resize(hashcount);
		a_stream.read(package.hash.data(), hashcount);
		uint64_t scene_count;
		readNumeric(scene_count);
		package.scenes.reserve(scene_count);
		for (size_t i = 0; i < scene_count; i++) {
			// --- SCENE START
			Scene scene{ package.author, package.hash };
			scene.id.resize(idcount);
			a_stream.read(scene.id.data(), idcount);
			readString(scene.name);
			uint64_t position_info_count;
			readNumeric(position_info_count);
			scene.positions.reserve(position_info_count);
			for (size_t n = 0; n < position_info_count; n++) {
				// ------ POSITION INFO START
				PositionInfo info{};
				a_stream.read(reinterpret_cast<char*>(&info.race.first), 1);
				a_stream.read(reinterpret_cast<char*>(&info.race.second), 1);
				uint8_t sex;
				a_stream.read(reinterpret_cast<char*>(&sex), 1);
				info.sex = Sex(sex);
				readFloat(info.scale);
				uint8_t extra;
				a_stream.read(reinterpret_cast<char*>(&extra), 1);
				info.extra = PositionInfo::Extra(extra);
				// ------ POSITION INFO END
				scene.positions.push_back(info);
			}
			// Start stage ID; Validated after all stages are loaded
			std::string startstage(idcount, 'X');
			a_stream.read(startstage.data(), idcount);

			uint64_t stage_count;
			readNumeric(stage_count);
			std::vector<StagePtr> stages{};
			stages.reserve(stage_count);
			for (size_t n = 0; n < stage_count; n++) {
				// ------ STAGE START
				auto stage = std::make_shared<Stage>();
				stage->id.resize(idcount);
				a_stream.read(stage->id.data(), idcount);
				uint64_t position_count;
				readNumeric(position_count);
				if (position_count != position_info_count) {
					throw std::runtime_error(fmt::format("Invalid position count; expected {} but got {}", position_info_count, position_count).c_str());
				}
				stage->positions.reserve(position_count);
				for (size_t j = 0; j < position_count; j++) {
					// --------- Position START
					Position position{};
					readString(position.event);
					a_stream.read(reinterpret_cast<char*>(&position.climax), 1);
					readFloat(position.offset[Offset::X]);
					readFloat(position.offset[Offset::Y]);
					readFloat(position.offset[Offset::Z]);
					readFloat(position.offset[Offset::R]);
					// --------- Position END
					stage->positions.push_back(position);
				}
				readFloat(stage->fixedlength);
				readString(stage->navtext);
				uint64_t tag_count;
				readNumeric(tag_count);
				for (size_t j = 0; j < tag_count; j++) {
					// --------- Tags START
					std::string tag;
					readString(tag);
					if (!TagHandler::AddTag(stage->tags, tag)) {
						stage->extratags.push_back(tag);
					}
					// --------- Tags END
				}
				// ------ STAGE END
				stages.push_back(stage);
				if (startstage == stage->id) {
					scene.start_animation = stage;
				}
			}
			if (!scene.start_animation) {
				throw std::runtime_error(fmt::format("Start animation is not defined for scene {}", scene.id).c_str());
			}

			const auto getStage = [&](const std::string_view id) {
				for (auto&& stage : stages) {
					if (stage->id == id) {
						return stage;
					}
				}
				throw std::runtime_error(fmt::format("Unrecognized stage referenced in graph: {}", id).c_str());
			};

			uint64_t graph_count;
			readNumeric(graph_count);
			if (graph_count != stage_count) {
				throw std::runtime_error(fmt::format("Invalid stage count; expected {} but got {}", stage_count, graph_count).c_str());
			}
			scene.graph.reserve(graph_count);
			for (size_t n = 0; n < graph_count; n++) {
				// --------- Graph START
				std::string keystage(idcount, 'X');
				a_stream.read(keystage.data(), idcount);
				const auto key = getStage(keystage);
				std::forward_list<StagePtr> destinations{};

				uint64_t edge_count;
				readNumeric(edge_count);
				for (size_t j = 0; j < edge_count; j++) {
					std::string edgestage(idcount, 'X');
					a_stream.read(edgestage.data(), idcount);
					destinations.push_front(getStage(edgestage));
				}
				// --------- Graph END
				scene.graph.push_back({ key, destinations });
			}
			a_stream.read(reinterpret_cast<char*>(&scene.furnituredata.allowbed), 1);
			// --- SCENE END
			package.scenes.push_back(scene);
		}
		return package;
	}

}	 // namespace Registry
