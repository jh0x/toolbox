#include <toolbox/Demangler.h>

#ifdef __GNUG__
#include <cxxabi.h>
#include <memory>
#endif

TOOLBOX_NAMESPACE_BEGIN

std::string demangle(const char* s) {
#ifdef __GNUG__
	int status = 0;
	auto realname = std::unique_ptr<char, decltype(free)*>(
			abi::__cxa_demangle(s, 0, 0, &status)
			, std::free);
	return !status ? realname.get() : s;
#else
	return s;
#endif
}

TOOLBOX_NAMESPACE_END
