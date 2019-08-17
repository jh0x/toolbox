#pragma once

#ifdef NDEBUG
#define XYZ_ASSERT(check, what) \
	((check) ? static_cast<void>(0) : __builtin_unreachable())
#else
#include <cassert>
#define XYZ_ASSERT(check, what) assert(check && what)
#endif

