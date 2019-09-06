#include <doctest/doctest.h>

#include <cstdint>
#include <limits>

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
