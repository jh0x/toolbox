#include <doctest/doctest.h>

#include <toolbox/Demangler.h>

namespace t = TOOLBOX_NAMESPACE;

using namespace std::literals::string_literals;

TEST_CASE("Demangler") {
	constexpr static auto kInput =
		"_ZNK3MapI10StringName3RefI8GDScriptE10ComparatorIS0_E16"
		"DefaultAllocatorE3hasERKS0_";
	const static auto kExpected =
		"Map<StringName, Ref<GDScript>, Comparator<StringName>"
		", DefaultAllocator>::has(StringName const&) const"s;
    REQUIRE(t::demangle(kInput) == kExpected);
}
