#pragma once

#include <fmt/ostream.h>

#define LOG_LINE(where, format, ...) \
	fmt::print(where, format "\n", ##__VA_ARGS__);

#define LOG_CERR(format, ...) \
	LOG_LINE(std::cerr, format, ##__VA_ARGS__);

#define LOG_COUT_VRB(format, ...) \
	LOG_LINE(std::cout, "[vrb]: " format, ##__VA_ARGS__);
#define LOG_COUT_DBG(format, ...) \
	LOG_LINE(std::cout, "[dbg]: " format, ##__VA_ARGS__);
