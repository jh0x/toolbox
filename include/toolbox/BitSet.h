#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <climits>

#include <toolbox/Config.h>

TOOLBOX_NAMESPACE_BEGIN

namespace bitsetdetail {

template<size_t S>
static constexpr auto _findStorageType() {
	static_assert(S > 0, "Invalid storage size");
	static_assert(S <= sizeof(__uint128_t) * CHAR_BIT, "Storage not supported");

	if constexpr (S <= sizeof(uint8_t) * CHAR_BIT) return uint8_t{};
	else if constexpr (S <= sizeof(uint16_t) * CHAR_BIT) return uint16_t{};
	else if constexpr (S <= sizeof(uint32_t) * CHAR_BIT) return uint32_t{};
	else if constexpr (S <= sizeof(uint64_t) * CHAR_BIT) return uint64_t{};
	else return __uint128_t{};
}

template<size_t S>
struct DefaultTraits
{
	using StorageType = decltype(_findStorageType<S>());
};

}

template<size_t S, typename Traits = bitsetdetail::DefaultTraits<S>>
class BitSet
{
public:
	using StorageType = typename Traits::StorageType;
	constexpr static auto kStorageTypeBits = sizeof(StorageType) * CHAR_BIT;

	constexpr BitSet() = default;

	// accesses specific bit
	constexpr bool operator[](size_t bit) const noexcept {
		return test(bit);
	}
	//accesses specific bit
	constexpr bool test(size_t bit) const noexcept {
		const auto idx = _index(bit);
		return (_storage >> idx) & 1;
	}

	// checks if all, any or none of the bits are set to true
	constexpr bool all() const noexcept; // TODO
	constexpr bool any() const noexcept {
		return !none();
	}
	constexpr bool none() const noexcept {
		return !_storage;
	}

	// returns the number of bits set to true
	constexpr bool count() const noexcept; // TODO

	// returns the size number of bits that the bitset can hold
	constexpr size_t size() const noexcept {
		return S;
	}

	// sets all bits to true
	constexpr BitSet& set() noexcept {
		_storage = ~(StorageType{0});
		return *this;
	}
	// sets the bit to a given value
	constexpr BitSet& set(size_t bit, bool value = true) noexcept {
		const auto idx = _index(bit);
		if(value) {
			_storage |= StorageType{1} << bit;
		} else {
			_storage &= ~(StorageType{1} << bit);
		}
		return *this;
	}

	// sets all bits to false
	constexpr BitSet& reset() noexcept {
		_storage = StorageType{0};
		return *this;
	}
	// sets bit to false
	constexpr BitSet& reset(size_t bit) noexcept {
		return set(bit, false);
	}

	// toggles the values of all bits
	constexpr BitSet& flip() noexcept {
		_storage = ~_storage;
		return *this;
	}
	// toggles the values of a single bit
	constexpr BitSet& flip(size_t bit) noexcept {
		const auto idx = _index(bit);
		_storage ^= StorageType{1} << bit;
		return *this;
	}

	// Write human readable representation of the bitset into the given space
	template<typename It>
	void format_to(It iter) const {
		for(size_t i = 0; i != size(); ++i) {
			*iter++ = test(i) ? '1' : '0';
		}
	}

private:
	constexpr size_t _index(size_t bit) const noexcept {
		return bit % kStorageTypeBits;
	}

	StorageType _storage{0};
};

TOOLBOX_NAMESPACE_END

