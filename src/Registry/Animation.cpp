#include "Animation.h"

namespace Registry
{
	StagePtr Scene::GetStageByKey(std::string_view a_key) const
	{
		for (auto&& [key, dest] : graph) {
			if (key->id == a_key)
				return key;
		}
		return nullptr;
	}

}	 // namespace Registry
