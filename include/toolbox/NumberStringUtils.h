#pragma once

#include <toolbox/Config.h>

#include <cstdint>
#include <type_traits>

TOOLBOX_NAMESPACE_BEGIN

struct CountWithUnroll{};

template<typename T>
inline constexpr int countDigits(T n, CountWithUnroll tag = {})
{
	static_assert(std::is_unsigned_v<T>, "Only unsigned");

	int result = 1;
	for (;;)
	{
		if(n < 10)	return result;
		if(n < 100) return result + 1;
		if(n < 1000) return result + 2;
		if(n < 10000) return result + 3;
		n /= 10000;
		result += 4;
	}
	return result;
}

TOOLBOX_NAMESPACE_END

