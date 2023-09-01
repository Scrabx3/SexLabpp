#pragma once

namespace Registry
{
	enum class Sex : uint8_t
	{
    None = 0,

		Male = 1 << 0,
		Female = 1 << 1,
		Futa = 1 << 2,
	};

	Sex GetSex(RE::Actor* a_actor);
	Sex GetSex(RE::Actor* a_actor, bool a_skipfactions);

} // namespace Registry
