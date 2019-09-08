#pragma once

#include <toolbox/Config.h>

#include <cstddef>
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


// Based on: https://github.com/miloyip/itoa-benchmark

inline constexpr char _kHundredNumbers[200] = {
'0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9','0',
'0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9','1',
'0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9','2',
'0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9','3',
'0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9','4',
'0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9','5',
'0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9','6',
'0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9','7',
'0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9','8',
'0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9','9',
};

template<size_t N, typename T>
inline constexpr bool utoaFixedEven(char * buffer, T t)
{
	static_assert(std::is_unsigned_v<T>, "Only unsigned");
	static_assert(N % 2 == 0, "Only even length");
	buffer += N;
	size_t count = N;
	while(count > 2)
	{
		const auto i = (t % 100) << 1;
		t /= 100;
		*--buffer = _kHundredNumbers[i];
		*--buffer = _kHundredNumbers[i + 1];
		count -= 2;
	}
	{
		const auto i = t << 1;
		*--buffer = _kHundredNumbers[i];
		*--buffer = _kHundredNumbers[i + 1];
	}
	return !(t / 100);
}

TOOLBOX_NAMESPACE_END

