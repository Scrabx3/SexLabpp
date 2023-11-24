#include "Misc.h"

namespace Registry
{
	inline constexpr size_t MAX_RECURSE = 10;
	RE::NiPointer<RE::NiCollisionObject> GetCollisionNodeRecurse(RE::NiNode* a_node, size_t a_recurse)
	{
		if (a_node->collisionObject) {
			return a_node->collisionObject;
		}
		if (a_recurse > MAX_RECURSE) {
			return nullptr;
		}
		for (auto child : a_node->children) {
			if (!child)
				continue;
			if (child->collisionObject)
				return child->collisionObject;
			if (auto node = child->AsNode()) {
				if (auto rec = GetCollisionNodeRecurse(node, a_recurse + 1)) {
					return rec;
				}
			}
		}
		return nullptr;
	}

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
