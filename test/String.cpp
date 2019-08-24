#include <algorithm>
#include <doctest/doctest.h>

#include <toolbox/String.h>


namespace t = TOOLBOX_NAMESPACE;


//template<t::StaticString T>
//struct S
//{
//	static constexpr const char* value = T;
//};
//
//TEST_CASE("StaticStringParam")
//{
//	S<"Test"> s;
//}

TEST_CASE("StaticString")
{
	auto s = t::StaticString("Test");
	REQUIRE(s.size() == 4);

	constexpr static auto expected = "Test";
	for(size_t i = 0; i != s.size() + 1; ++i) {
		REQUIRE(s[i] == expected[i]);
	}
}
