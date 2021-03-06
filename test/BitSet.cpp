#include <toolbox/BitSet.h>

#include <doctest/doctest.h>

#include <bitset>
#include <random>
#include <type_traits>
#include <string>
#include <iostream>

namespace t = TOOLBOX_NAMESPACE;
namespace td = TOOLBOX_NAMESPACE::bitsetdetail;

template<
	typename Expected
	, size_t S
	, typename std::enable_if_t<(S <= sizeof(Expected) * CHAR_BIT), void>* = nullptr>
auto _storageTypeTesterImpl()
{
	static_assert(
			std::is_same_v<decltype(td::_findStorageType<S>()) , Expected>
			 , "storage size check");
	static_assert(sizeof(Expected) == sizeof(t::BitSet<S>), "BitSet size check");
};

template<
	typename Expected
	, size_t S
	, typename std::enable_if_t<(S > sizeof(Expected) * CHAR_BIT), void>* = nullptr>
auto _storageTypeTesterImpl()
{
	static_assert(
			std::is_same_v<decltype(td::_findStorageType<S>()) , Expected>
			 , "storage size check");
};

template<typename Expected, size_t offset, size_t... Is>
void _storageTypeTester(std::integer_sequence<size_t, Is...>)
{
	(_storageTypeTesterImpl<Expected, offset + Is + 1>(), ...);
}

TEST_CASE("StorageType")
{
	// Not really a doctest - everything done at compile time
	_storageTypeTester<uint8_t, 0>(std::make_integer_sequence<size_t, 8>{});
	_storageTypeTester<uint16_t, 8>(std::make_integer_sequence<size_t, 8>{});
	_storageTypeTester<uint32_t, 16>(std::make_integer_sequence<size_t, 16>{});
	// Above means: take all bitsets with size<17..32>
	// and check that the basic storage type matches

	_storageTypeTester<uint64_t, 32>(std::make_integer_sequence<size_t, 32>{});
	_storageTypeTester<__uint128_t, 64>(std::make_integer_sequence<size_t, 64>{});
	_storageTypeTester<uint32_t, 1024>(std::make_integer_sequence<size_t, 32>{});
	_storageTypeTester<uint64_t, 1024+32>(std::make_integer_sequence<size_t, 32>{});
	_storageTypeTester<uint32_t, 1024+64>(std::make_integer_sequence<size_t, 32>{});
	_storageTypeTester<__uint128_t, 1024+96>(std::make_integer_sequence<size_t, 32>{});
}

// It is nicer when doctest prints out some context information
// This little macro will dump out current bitset to string
// an instruct doctest to print some diagnostic on failures
// Assumes that a buffer of large enough size exists...
#define DO_DIAG \
	b.format_to(diagnostic.begin()); \
	INFO("Current state='" << diagnostic << "'");

TEST_CASE_TEMPLATE_DEFINE("Operations - part 1", T, test_id)
{
	using TestTraits = T;
	constexpr static auto kSize = TestTraits::kSize;
	constexpr static auto kTestBit = TestTraits::kTestBit;
	std::string diagnostic(kSize, ' ');

	t::BitSet<kSize> b;
	DO_DIAG;

	REQUIRE(b.size() == kSize);
	REQUIRE(b.none());
	REQUIRE(b.count() == 0);
	for(size_t i = 0; i != kSize; ++i) {
		REQUIRE(b[i] == false);
	}

	b.set(kTestBit);
	DO_DIAG;
	REQUIRE(b.any());
	REQUIRE(b[kTestBit]);
	REQUIRE(b.test(kTestBit));
	REQUIRE(b.count() == 1);

	b.flip(kTestBit);
	DO_DIAG;
	REQUIRE(b.none());
	REQUIRE(!b[kTestBit]);
	REQUIRE(!b.test(kTestBit));

	b.set();
	DO_DIAG;
	REQUIRE(b.all());
	REQUIRE(b.count() == kSize);
	for(size_t i = 0; i != kSize; ++i) {
		REQUIRE(b[i]);
	}

	b.reset();
	DO_DIAG;
	REQUIRE(b.none());

	b.set(kTestBit);
	b.flip();
	DO_DIAG;
	for(size_t i = 0; i != kSize; ++i) {
		if(i == kTestBit)
			REQUIRE(!b[i]);
		else
			REQUIRE(b[i]);
	}

	b.reset();
	DO_DIAG;
	REQUIRE(b.none());
}

template<size_t S, size_t B = S - 1>
struct TestTraits
{
	constexpr static auto kSize = S;
	constexpr static auto kTestBit = B;
};
TEST_CASE_TEMPLATE_INVOKE(test_id, TestTraits<7, 3>);
TEST_CASE_TEMPLATE_INVOKE(test_id, TestTraits<15, 13>);
TEST_CASE_TEMPLATE_INVOKE(test_id, TestTraits<31, 23>);
TEST_CASE_TEMPLATE_INVOKE(test_id, TestTraits<63, 43>);
TEST_CASE_TEMPLATE_INVOKE(test_id, TestTraits<127, 73>);
TEST_CASE_TEMPLATE_INVOKE(test_id, TestTraits<127, 3>);

#undef DO_DIAG
#define DO_DIAG \
	b.format_to(diagnostic.begin()); \
	INFO("Current state='" << diagnostic \
			<< "' expected='" << expected.to_string() << "'");


TEST_CASE_TEMPLATE_DEFINE("Operations - part 2", T, test_id2)
{
	using TestTraits = T;
	constexpr static auto kSize = TestTraits::kSize;
	std::bitset<kSize> expected;

	auto dist = std::uniform_int_distribution<>(0,1);
	auto eng = std::default_random_engine();
	for(size_t i = 0; i != kSize; ++i) {
		expected[i] = dist(eng);
	}
	std::string diagnostic(kSize, ' ');

	t::BitSet<kSize> b;
	DO_DIAG;

	REQUIRE(b.size() == kSize);
	REQUIRE(b.none());
	REQUIRE(b.count() == 0);

	for(size_t i = 0; i != kSize; ++i) {
		b.set(i, expected[i]);
	}
	DO_DIAG;
	for(size_t i = 0; i != kSize; ++i) {
		REQUIRE(b[i] == expected[i]);
	}
	REQUIRE(b.count() == expected.count());

	b.flip();
	expected.flip();
	DO_DIAG;
	for(size_t i = 0; i != kSize; ++i) {
		REQUIRE(b[i] == expected[i]);
	}
	REQUIRE(b.count() == expected.count());
}


#define TEST_IT(N) \
	TYPE_TO_STRING(TestTraits<N>); \
	TEST_CASE_TEMPLATE_INVOKE(test_id2, TestTraits<N>);

TEST_IT(7);
TEST_IT(15);
TEST_IT(31);
TEST_IT(63);
TEST_IT(127);
TEST_IT(128);
TEST_IT(1024);
TEST_IT(1024+15);
TEST_IT(1024+31);
TEST_IT(1024+47);
TEST_IT(1024+63);
TEST_IT(1024+95);
TEST_IT(1024+127);
TEST_IT(65535);

