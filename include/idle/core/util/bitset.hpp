
/*
 *   _____    _ _        .      .    .
 *  |_   _|  | | |  .       .           .
 *    | |  __| | | ___         .    .        .
 *    | | / _` | |/ _ \                .
 *   _| || (_| | |  __/ github.com/Naios/idle
 *  |_____\__,_|_|\___| AGPL v3 (Early Access)
 *
 * Copyright(c) 2018 - 2021 Denis Blank <denis.blank at outlook dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IDLE_CORE_UTIL_BITSET_HPP_INCLUDED
#define IDLE_CORE_UTIL_BITSET_HPP_INCLUDED

#include <atomic>
#include <initializer_list>
#include <limits>
#include <type_traits>
#include <utility>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/iterator_facade.hpp>

namespace idle {
namespace detail {
template <typename MaskType, typename Store>
struct bitset_trait {
  static constexpr MaskType get(Store const& store) noexcept {
    return store;
  }
  static constexpr void set(Store& store, MaskType mask) noexcept {
    store = mask;
  }
  static constexpr bool set_or(Store& store, MaskType mask) noexcept {
    return (std::exchange(store, static_cast<MaskType>(store | mask)) & mask) !=
           mask;
  }
  static constexpr bool set_and(Store& store, MaskType mask) noexcept {
    return std::exchange(store, static_cast<MaskType>(store & mask)) & ~mask;
  }
};

template <typename MaskType>
struct atomic_bitset_trait {
  static constexpr MaskType get(std::atomic<MaskType> const& store) noexcept {
    return store.load(std::memory_order_acquire);
  }
  static constexpr void set(std::atomic<MaskType>& store,
                            MaskType mask) noexcept {
    store.store(mask, std::memory_order_release);
  }
  static constexpr bool set_or(std::atomic<MaskType>& store,
                               MaskType mask) noexcept {

    return (std::atomic_fetch_or(&store, mask) & mask) != mask;
  }
  static constexpr bool set_and(std::atomic<MaskType>& store,
                                MaskType mask) noexcept {

    return std::atomic_fetch_and(&store, mask) & ~mask;
  }
};
} // namespace detail

struct bitset_all_arg_t {};

/// A non thread safe set for bit flags
///
/// The interface of this class is similar to std::set rather than std::bitset
///
/// \note \see atomic_bitset for a threadsafe version of this class
template <typename T, typename MaskType = std::underlying_type_t<T>,
          typename Store = MaskType,
          typename Trait = detail::bitset_trait<MaskType, Store>>
class BitSet {
  static constexpr std::size_t bits = sizeof(MaskType) * 8U;
  using bit_index_t = std::uint8_t;

  constexpr static MaskType mask_of(T value) noexcept {
    IDLE_ASSERT(std::size_t(value) <= bits);
    return 1 << MaskType(value);
  }

  constexpr static MaskType full_mask() noexcept {
    return std::numeric_limits<MaskType>::max();
  }
  constexpr static MaskType empty_mask() noexcept {
    return MaskType{};
  }

public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T;
  using reference = T;

  class const_iterator
    : public iterator_facade<const_iterator, std::forward_iterator_tag,
                             value_type, difference_type, pointer, reference> {

    friend class BitSet;

  public:
    constexpr const_iterator() noexcept {}

    explicit constexpr const_iterator(BitSet const* current, bit_index_t i)
      : current_(current)
      , index_(i) {}

    constexpr bool equal(const_iterator const& other) const noexcept {
      return index_ == other.index_;
    }

    constexpr void increment() noexcept {
      IDLE_ASSERT(index_ < bits);

      index_ = current_->scan_next(index_ + 1);
    }

    constexpr reference dereference() const noexcept {
      // Invalidated iterator
      IDLE_ASSERT(current_->contains(static_cast<T>(index_)));

      return static_cast<T>(index_);
    }

  private:
    BitSet const* current_{nullptr};
    bit_index_t index_{bits};
  };

  using iterator = const_iterator;

  constexpr BitSet() noexcept
    : BitSet(empty_mask()) {}
  /* implicit */ constexpr BitSet(std::initializer_list<T> list) noexcept
    : store_(empty_mask()) {
    for (T value : list) {
      insert(value);
    }
  }
  template <typename Iterator>
  explicit constexpr BitSet(Iterator begin, Iterator end) noexcept
    : store_(empty_mask()) {
    for (; begin != end; ++begin) {
      insert(*begin);
    }
  }
  explicit constexpr BitSet(MaskType initial) noexcept
    : store_(initial) {}
  explicit constexpr BitSet(bitset_all_arg_t) noexcept
    : store_(full_mask()) {}

  constexpr BitSet(BitSet const&) = default;
  constexpr BitSet& operator=(BitSet const&) = default;

  static constexpr BitSet all() noexcept {
    return BitSet(bitset_all_arg_t{});
  }
  static constexpr BitSet none() noexcept {
    return BitSet{};
  }

  constexpr MaskType value() const noexcept {
    return Trait::get(store_);
  }

  explicit operator MaskType() const noexcept {
    return value();
  }

  constexpr bool empty() const noexcept {
    return value() == empty_mask();
  }

  constexpr const_iterator begin() const noexcept {
    return const_iterator{this, scan_next(0)};
  }
  constexpr const_iterator end() const noexcept {
    return const_iterator{this, bits};
  }

  std::size_t size() const noexcept {
    std::size_t s = 0;

    // TODO This can be improved through a lookup table
    for (bit_index_t i = 0; i < bits; ++i) {
      if (Trait::get(store_) & mask_of(static_cast<T>(i))) {
        ++s;
      }
    }

    return s;
  }

  constexpr void clear(MaskType mask = MaskType{}) noexcept {
    Trait::set(store_, mask);
  }

  constexpr void fill() noexcept {
    reset(full_mask());
  }

  constexpr bool is_mask(MaskType mask) const noexcept {
    return value() == mask;
  }
  constexpr bool is(T value) const noexcept {
    return is_mask(mask_of(value));
  }

  constexpr bool has_mask(MaskType mask) const noexcept {
    return (value() & mask) == mask;
  }
  constexpr bool contains(T value) const noexcept {
    return has_mask(mask_of(value));
  }

  constexpr bool contains_any(BitSet other) const noexcept {
    return value() & other.value();
  }

  constexpr bool contains_all(BitSet other) const noexcept {
    return has_mask(other.value());
  }

  constexpr bool insert(MaskType mask) noexcept {
    return Trait::set_or(store_, mask);
  }
  constexpr std::pair<iterator, bool> insert(T value) noexcept {
    return {iterator{this, static_cast<bit_index_t>(value)},
            insert(mask_of(value))};
  }

  constexpr const_iterator find(T value) const noexcept {
    if (contains(value)) {
      return iterator{this, static_cast<bit_index_t>(value)};
    } else {
      return end();
    }
  }

  constexpr std::size_t erase(MaskType mask) noexcept {
    return Trait::set_and(store_, ~mask) ? 1U : 0U;
  }
  constexpr std::size_t erase(T value) noexcept {
    return erase(mask_of(value));
  }
  constexpr std::size_t erase(const_iterator itr) noexcept {
    IDLE_ASSERT(itr.current_ == this);
    IDLE_ASSERT(itr != end());
    return erase(static_cast<T>(itr.index_));
  }

  constexpr BitSet operator|(BitSet right) const noexcept {
    return BitSet{MaskType(value() | right.value())};
  }
  constexpr BitSet operator&(BitSet right) const noexcept {
    return BitSet{MaskType(value() & right.value())};
  }

private:
  constexpr bit_index_t scan_next(bit_index_t i) const noexcept {
    IDLE_ASSERT(i < bits);

    // TODO This can be improved through intrinsics (bit-scan instructions)
    for (; i < bits; ++i) {
      if (Trait::get(store_) & (1 << i)) {
        return i;
      }
    }

    return bits;
  }

  Store store_;
};

/// A thread safe bitset with an underlying atomic storage
template <typename T, typename MaskType = std::underlying_type_t<T>>
using AtomicBitSet = BitSet<T, MaskType, std::atomic<MaskType>,
                            detail::atomic_bitset_trait<MaskType>>;
} // namespace idle

#endif // IDLE_CORE_UTIL_BITSET_HPP_INCLUDED
