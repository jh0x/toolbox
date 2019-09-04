#pragma once

#include <toolbox/Config.h>

#include <cstddef>

// From https://old.reddit.com/r/cpp/comments/cthk2v/get_type_name_string_in_compile_time_c14_gccclang/

TOOLBOX_NAMESPACE_BEGIN

template<size_t N>
class StaticString
{
public:
	constexpr StaticString() = default;

	constexpr StaticString(const char* begin, const char* end) {
		char* p = _s;
		while(begin != end) {
			*p++ = *begin++;
		}
	}

	constexpr StaticString(const char* s) {
		for(size_t i = 0; i != N; ++i) {
			_s[i] = s[i];
		}
	}

	constexpr size_t size() const {
		return N;
	}

	constexpr const char* c_str() const {
		return _s;
	}

	constexpr operator const char*() const {
		return _s;
	}

	constexpr bool operator==(const StaticString<N>& o) const noexcept {
		const char* a = _s;
		const char* b = o._s;
		while(*a != '\0' && *b != '\0') {
			if(*a != *b) return false;
			++a; ++b;
		}
		return *a == '\0' && *b == '\0';
	}
	constexpr bool operator!=(const StaticString<N>& o) const noexcept {
		return !(*this == o);
	}
private:
	char _s[N + 1]{};
};

template<size_t N>
StaticString(const char (&)[N]) -> StaticString<N - 1>;

TOOLBOX_NAMESPACE_END


