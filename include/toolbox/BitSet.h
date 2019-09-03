#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <climits>

#include <toolbox/BitHacks.h>
#include <toolbox/Config.h>

TOOLBOX_NAMESPACE_BEGIN

namespace bitsetdetail {

template<size_t S>
static constexpr auto _findStorageType() {
	static_assert(S > 0, "Invalid storage size");

	if constexpr (S <= sizeof(uint8_t) * CHAR_BIT) return uint8_t{};
	else if constexpr (S <= sizeof(uint16_t) * CHAR_BIT) return uint16_t{};
	else if constexpr (S <= sizeof(uint32_t) * CHAR_BIT) return uint32_t{};
	else if constexpr (S <= sizeof(uint64_t) * CHAR_BIT) return uint64_t{};
	else if constexpr (S <= sizeof(__uint128_t) * CHAR_BIT) return __uint128_t{};
	// Try to minimise waste (but don't overdo it - don't want smaller types)
	else if constexpr (S % 128 == 0) return __uint128_t{};
	else if constexpr (S % 128 <= 32) return uint32_t{};
	else if constexpr (S % 128 <= 64) return uint64_t{};
	else if constexpr (S % 128 <= 96) return uint32_t{};
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
	constexpr static auto kAggStorageSize =
		(S / kStorageTypeBits) + ( (S % kStorageTypeBits) != 0 );
	constexpr static StorageType kUsedBitsMask = S % kStorageTypeBits
		? (StorageType{1} << S % (kStorageTypeBits)) - 1
		: StorageType{0} - 1;
	using AggStorageType = std::array<StorageType, kAggStorageSize>;
	struct Index { size_t idx; unsigned bit; };

	constexpr BitSet() = default;

	// accesses specific bit
	constexpr bool operator[](size_t bit) const noexcept {
		return test(bit);
	}
	//accesses specific bit
	constexpr bool test(size_t bit) const noexcept {
		const auto idx = _index(bit);
		return (_storage[idx.idx] >> idx.bit) & 1;
	}

	// checks if all, any or none of the bits are set to true
	constexpr bool all() const noexcept {
		for(size_t idx = 0; idx != _storage.size() - 1; ++idx) {
			if(_storage[idx] != ~StorageType{0}) {
				return false;
			}
		}
		return (_storage.back() & kUsedBitsMask) == kUsedBitsMask;
	}
	constexpr bool any() const noexcept {
		return !none();
	}
	constexpr bool none() const noexcept {
		for(auto& e: _storage) {
			if(e) return false;
		}
		return true;
	}

	// returns the number of bits set to true
	constexpr size_t count() const noexcept {
		size_t r = 0;
		for(auto& e: _storage) {
			r += popcount(e);
		}
		return r;
	}

	// returns the size number of bits that the bitset can hold
	constexpr size_t size() const noexcept {
		return S;
	}

	// sets all bits to true
	constexpr BitSet& set() noexcept {
		for(size_t idx = 0; idx != _storage.size() - 1; ++idx) {
			_storage[idx] = ~StorageType{0};
		}
		_storage.back() = kUsedBitsMask;
		return *this;
	}
	// sets the bit to a given value
	constexpr BitSet& set(size_t bit, bool value = true) noexcept {
		const auto idx = _index(bit);
		if(value) {
			_storage[idx.idx] |= StorageType{1} << idx.bit;
		} else {
			_storage[idx.idx] &= ~(StorageType{1} << idx.bit);
		}
		return *this;
	}

	// sets all bits to false
	constexpr BitSet& reset() noexcept {
		for(auto& e: _storage) {
			e = StorageType{0};
		}
		return *this;
	}
	// sets bit to false
	constexpr BitSet& reset(size_t bit) noexcept {
		return set(bit, false);
	}

	// toggles the values of all bits
	constexpr BitSet& flip() noexcept {
		for(size_t idx = 0; idx != _storage.size() - 1; ++idx) {
			_storage[idx] = ~_storage[idx];
		}
		_storage.back() = ~_storage.back() & kUsedBitsMask;
		return *this;
	}
	// toggles the values of a single bit
	constexpr BitSet& flip(size_t bit) noexcept {
		const auto idx = _index(bit);
		_storage[idx.idx] ^= StorageType{1} << idx.bit;
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
	constexpr Index _index(size_t bit) const noexcept {
		return {bit / kStorageTypeBits
			, static_cast<unsigned>(bit % kStorageTypeBits)};
	}

	AggStorageType _storage{};
};

TOOLBOX_NAMESPACE_END

