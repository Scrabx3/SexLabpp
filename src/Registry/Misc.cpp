#include "Misc.h"

namespace Registry
{
	bool IsNPC(const RE::Actor* a_actor)
	{
		return a_actor->HasKeyword(GameForms::ActorTypeNPC);
	}

	bool IsVampire(const RE::Actor* a_actor)
	{
		return a_actor->HasKeyword(GameForms::Vampire);
	}

	RE::TESActorBase* GetLeveledActorBase(RE::Actor* a_actor)
	{
		const auto base = a_actor->GetTemplateActorBase();
		return base ? base : a_actor->GetActorBase();
	}

	std::vector<std::string_view> StringSplit(const std::string_view a_view, const char a_delim)
	{
		const auto trim = [&](size_t from, size_t len) {
			auto word = a_view.substr(from, len);
			while (!word.empty() && std::isspace(word[0]))
				word.remove_prefix(1);
			while (!word.empty() && std::isspace(word.back()))
				word.remove_suffix(1);

			return word;
		};
		std::vector<std::string_view> result;
		size_t previous = 0;
		for (size_t i = 0; i < a_view.size(); i++) {
			if (a_view[i] != a_delim)
				continue;

			auto word = trim(previous, i - previous);
			if (!word.empty()) {
				result.push_back(word);
			}
			previous = i + 1;
		}
		if (auto last = trim(previous, std::string_view::npos); !last.empty()) {
			result.push_back(last);
		}
		return result;
	}
}
