
#pragma once

namespace Random
{
	template <class T>
	T draw(T a_min, T a_max)
	{
		if (a_min >= a_max)
			return a_min;

		static std::mt19937 eng{ std::random_device{}() };

		if constexpr (std::is_integral_v<T>)
			return std::uniform_int_distribution<T>{ a_min, a_max }(eng);
		else
			return std::uniform_real_distribution<T>{ a_min, a_max }(eng);
	}
};