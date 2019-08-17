#pragma once

#include <fmt/ostream.h>

#include <toolbox/Preprocessor.h>

constexpr int _find_basename(
		const char* const p
		, const int pos = 0
		, const int slash_pos = -1)
{
	return p[pos]
		? (p[pos] == '/'
				? _find_basename(p, pos + 1, pos)
				: _find_basename(p, pos + 1, slash_pos)
			)
		: (slash_pos + 1);
}

#define __FILELINE__ \
({ \
	static const int pos = _find_basename(__FILE__); \
	static_assert (pos >= 0, "_find_basename"); \
	(__FILE__ ":" XYZ_STRINGIFY(__LINE__) ": ") + pos; \
})

#define LOG_LINE(sink, prefix, format, ...) \
	fmt::print(sink, prefix " {} " format "\n", __FILELINE__, ##__VA_ARGS__);

#define LOG_CERR(format, ...) \
	LOG_LINE(std::cerr, "", format, ##__VA_ARGS__);

#define LOG_COUT_VRB(format, ...) \
	LOG_LINE(std::cout, "[vrb]: ", format, ##__VA_ARGS__);
#define LOG_COUT_DBG(format, ...) \
	LOG_LINE(std::cout, "[dbg]: ", format, ##__VA_ARGS__);
