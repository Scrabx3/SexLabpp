#pragma once

namespace Papyrus
{
	class Tracking :
    public Singleton<Tracking>
	{
	public:
		std::map<RE::FormID, std::vector<RE::BSFixedString>> _factions;
    std::map<RE::FormID, std::vector<RE::BSFixedString>> _actors;

	public:
		void Add(std::map<RE::FormID, std::vector<RE::BSFixedString>>& a_list, RE::FormID a_keyvalue, const RE::BSFixedString& a_callback)
		{
			auto it = a_list.find(a_keyvalue);
			if (it == a_list.end()) {
				a_list.insert({ a_keyvalue, { a_callback } });
			} else if (std::find(it->second.begin(), it->second.end(), a_callback) == it->second.end()) {
				it->second.push_back(a_callback);
			}
		}

		void Remove(std::map<RE::FormID, std::vector<RE::BSFixedString>>& a_list, RE::FormID a_keyvalue, const RE::BSFixedString& a_callback)
		{
			auto it = a_list.find(a_keyvalue);
			if (it == a_list.end())
				return;

			const auto where = std::remove(it->second.begin(), it->second.end(), a_callback);
			if (where == it->second.begin()) {
				a_list.erase(it);
			} else {
				it->second.erase(where, it->second.end());
			}
		}

	public:
		void Save(SKSE::SerializationInterface* a_intfc)
    {
			const auto save = [&]<class T>(const T& tosave) {
				const size_t numRegs = tosave.size();
				if (!a_intfc->WriteRecordData(numRegs)) {
					logger::error("Failed to save number of regs ({})", numRegs);
					return;
				}
				for (auto&& [formid, list] : tosave) {
					a_intfc->WriteRecordData(formid);

					const size_t numList = list.size();
					if (!a_intfc->WriteRecordData(numList)) {
						logger::error("Failed to save number of events ({})", numList);
						return;
					}
					for (auto&& event : list) {
						stl::write_string(a_intfc, event);
					}

					a_intfc->WriteRecordData((std::numeric_limits<uint32_t>::max)());
				}
			};

			save(_factions);
			save(_actors);
			logger::info("Saved {} tracked factions and {} actors", _factions.size(), _actors.size());
		}

		void Load(SKSE::SerializationInterface* a_intfc)
		{
			_factions.clear();
			_actors.clear();

			const auto load = [&]<class T>(T& toload) {
				size_t numRegs;
				a_intfc->ReadRecordData(numRegs);

				for (size_t i = 0; i < numRegs; i++) {
					RE::FormID id, newid;
					a_intfc->ReadRecordData(id);
					if (!a_intfc->ResolveFormID(id, newid)) {
						while (a_intfc->ReadRecordData(id) && id != (std::numeric_limits<uint32_t>::max)()) {}
						continue;
					}

					std::vector<RE::BSFixedString> list{};
					size_t numEvents;
					a_intfc->ReadRecordData(numEvents);
					list.reserve(numEvents);

					for (size_t n = 0; n < numEvents; n++) {
						std::string next;
						stl::read_string(a_intfc, next);
						list.push_back(next);
					}
					toload.insert({ newid, list });
				}
			};

			load(_factions);
			load(_actors);
			logger::info("Loaded {} tracked factions and {} actors", _factions.size(), _actors.size());
		}

		void Revert(SKSE::SerializationInterface*)
    {
			_factions.clear();
			_actors.clear();
		}
	};

} // namespace Papyrus
