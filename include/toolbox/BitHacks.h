#pragma once

#include <type_traits>

#include <toolbox/Config.h>

TOOLBOX_NAMESPACE_BEGIN

template<typename T>
constexpr size_t popcount(T x) {
	static_assert(std::is_integral_v<T>, "Only for integral types");
	if constexpr (sizeof(T) <= sizeof(unsigned)) {
		return __builtin_popcount(static_cast<unsigned>(x));
	} else if constexpr (sizeof(T) == sizeof(unsigned long)) {
		return __builtin_popcountl(static_cast<unsigned long>(x));
	} else if constexpr (sizeof(T) == sizeof(unsigned long long)) {
		return __builtin_popcountll(static_cast<unsigned long long>(x));
	} else if constexpr (sizeof(T) == sizeof(__uint128_t)) {
		union {
			T t;
			struct {
				unsigned long long m;
				unsigned long long n;
			} s;
		} u = {x};
		return __builtin_popcountll(u.s.m) + __builtin_popcountll(u.s.n);
	} else {
		static_assert(true, "Unsupported type");
	}
}

TOOLBOX_NAMESPACE_END

