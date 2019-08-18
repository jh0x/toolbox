#include <toolbox/BitSet.h>

#include <doctest/doctest.h>

#include <type_traits>
#include <string>

namespace t = TOOLBOX_NAMESPACE;
namespace td = TOOLBOX_NAMESPACE::bitsetdetail;

template<typename Expected, size_t S>
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
	_storageTypeTester<uint64_t, 32>(std::make_integer_sequence<size_t, 32>{});
	_storageTypeTester<__uint128_t, 64>(std::make_integer_sequence<size_t, 64>{});
}


TEST_CASE("Operations")
{
	constexpr static auto kSize = 7;
	constexpr static auto kTestBit = 3;
	std::string diagnostic(kSize, ' ');
	// It is nicer when doctest prints out some context information
	// This little macro will dump out current bitset to string
	// an instruct doctest to print some diagnostic on failures
#define DO_DIAG \
	b.format_to(diagnostic.begin()); \
	INFO("Current state='" << diagnostic << "'");

	t::BitSet<kSize> b;
	DO_DIAG;

	REQUIRE(b.size() == kSize);
	REQUIRE(b.none());
	for(size_t i = 0; i != kSize; ++i) {
		REQUIRE(b[i] == false);
	}

	b.set(kTestBit);
	DO_DIAG;
	REQUIRE(b.any());
	REQUIRE(b[kTestBit]);
	REQUIRE(b.test(kTestBit));

	b.flip(kTestBit);
	DO_DIAG;
	REQUIRE(b.none());
	REQUIRE(!b[kTestBit]);
	REQUIRE(!b.test(kTestBit));

	b.set();
	DO_DIAG;
	//REQUIRE(b.all());
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
