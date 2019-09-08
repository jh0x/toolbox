#include <doctest/doctest.h>

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

#include <toolbox/NumberStringUtils.h>

namespace t = TOOLBOX_NAMESPACE;

TEST_CASE("NumberStringUtils - countDigits") {
	REQUIRE(t::countDigits(std::numeric_limits<uint8_t>::max()) == 3);
	REQUIRE(t::countDigits(std::numeric_limits<uint16_t>::max()) == 5);
	REQUIRE(t::countDigits(0u) == 1);
	REQUIRE(t::countDigits(1u) == 1);
	REQUIRE(t::countDigits(12u) == 2);
	REQUIRE(t::countDigits(123u) == 3);
	REQUIRE(t::countDigits(1234u) == 4);
	REQUIRE(t::countDigits(12345u) == 5);
	REQUIRE(t::countDigits(123456u) == 6);
	REQUIRE(t::countDigits(1234567u) == 7);
	REQUIRE(t::countDigits(12345678u) == 8);
	REQUIRE(t::countDigits(123456789u) == 9);
	REQUIRE(t::countDigits(1234567890u) == 10);
	REQUIRE(t::countDigits(std::numeric_limits<uint32_t>::max()) == 10);

	REQUIRE(t::countDigits(0ull) == 1);
	REQUIRE(t::countDigits(1ull) == 1);
	REQUIRE(t::countDigits(12ull) == 2);
	REQUIRE(t::countDigits(123ull) == 3);
	REQUIRE(t::countDigits(1234ull) == 4);
	REQUIRE(t::countDigits(12345ull) == 5);
	REQUIRE(t::countDigits(123456ull) == 6);
	REQUIRE(t::countDigits(1234567ull) == 7);
	REQUIRE(t::countDigits(12345678ull) == 8);
	REQUIRE(t::countDigits(123456789ull) == 9);
	REQUIRE(t::countDigits(1234567890ull) == 10);
	REQUIRE(t::countDigits(12345678901ull) == 11);
	REQUIRE(t::countDigits(123456789012ull) == 12);
	REQUIRE(t::countDigits(1234567890123ull) == 13);
	REQUIRE(t::countDigits(12345678901234ull) == 14);
	REQUIRE(t::countDigits(123456789012345ull) == 15);
	REQUIRE(t::countDigits(1234567890123456ull) == 16);
	REQUIRE(t::countDigits(12345678901234567ull) == 17);
	REQUIRE(t::countDigits(123456789012345678ull) == 18);
	REQUIRE(t::countDigits(1234567890123456789ull) == 19);
	REQUIRE(t::countDigits(12345678901234567890ull) == 20);
	REQUIRE(t::countDigits(std::numeric_limits<uint64_t>::max()) == 20);
	REQUIRE(t::countDigits(std::numeric_limits<__uint128_t>::max()) == 39);
}

TEST_CASE("NumberStringUtils - utoaFixedEven") {
	auto test = [](
			auto ntype
			, auto x
			, const std::string& expected
			, auto shouldWork) {
		constexpr size_t n = ntype();
		INFO("Width = " << n
				<< ", x = " << x
				<< ", expected = " << expected
				<< ", shouldWork = " << shouldWork);
		char buf[n];
		const auto res = t::utoaFixedEven<n>(buf, x);
		REQUIRE(res == shouldWork);
		if(shouldWork) {
			REQUIRE(std::string_view(buf, n) == expected);
		}
	};
	test(std::integral_constant<size_t, 2>{}, 0u, "00", true);
	test(std::integral_constant<size_t, 2>{}, 5u, "05", true);
	test(std::integral_constant<size_t, 2>{}, 52u, "52", true);
	test(std::integral_constant<size_t, 2>{}, 100u, "", false);

	test(std::integral_constant<size_t, 4>{}, 0u, "0000", true);
	test(std::integral_constant<size_t, 4>{}, 1u, "0001", true);
	test(std::integral_constant<size_t, 4>{}, 12u, "0012", true);
	test(std::integral_constant<size_t, 4>{}, 123u, "0123", true);
	test(std::integral_constant<size_t, 4>{}, 1234u, "1234", true);
	test(std::integral_constant<size_t, 4>{}, 10000u, "", false);

	test(std::integral_constant<size_t, 6>{}, 0u, "000000", true);
	test(std::integral_constant<size_t, 6>{}, 9u, "000009", true);
	test(std::integral_constant<size_t, 6>{}, 98u, "000098", true);
	test(std::integral_constant<size_t, 6>{}, 987u, "000987", true);
	test(std::integral_constant<size_t, 6>{}, 9876u, "009876", true);
	test(std::integral_constant<size_t, 6>{}, 987654u, "987654", true);
	test(std::integral_constant<size_t, 6>{}, 9876543u, "9876543", false);

	test(std::integral_constant<size_t, 10>{}, 0u, "0000000000", true);
	test(std::integral_constant<size_t, 10>{}, 1u, "0000000001", true);
	test(std::integral_constant<size_t, 10>{}, 12u, "0000000012", true);
	test(std::integral_constant<size_t, 10>{}, 123u, "0000000123", true);
	test(std::integral_constant<size_t, 10>{}, 1234u, "0000001234", true);
	test(std::integral_constant<size_t, 10>{}, 12345u, "0000012345", true);
	test(std::integral_constant<size_t, 10>{}, 123456u, "0000123456", true);
	test(std::integral_constant<size_t, 10>{}, 1234567u, "0001234567", true);
	test(std::integral_constant<size_t, 10>{}, 12345678u, "0012345678", true);
	test(std::integral_constant<size_t, 10>{}, 123456789u, "0123456789", true);
	test(std::integral_constant<size_t, 10>{}, 1234567890u, "1234567890", true);

	test(std::integral_constant<size_t, 20>{}, 12345678900987654321ull
			, "12345678900987654321", true);

	test(std::integral_constant<size_t, 40>{}
			, std::numeric_limits<__uint128_t>::max()
			, "0340282366920938463463374607431768211455", true);
}
