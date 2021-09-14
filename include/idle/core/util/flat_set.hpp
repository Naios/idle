
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

#ifndef IDLE_CORE_UTIL_FLAT_SET_HPP_INCLUDED
#define IDLE_CORE_UTIL_FLAT_SET_HPP_INCLUDED

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include <idle/core/util/algorithm.hpp>

namespace idle {
/// A simple flat set implemented on top of an arbitrary container
template <typename T, typename Predicate = std::less<>,
          typename Allocator = std::allocator<T>,
          template <typename, typename> class Container = std::vector>
class FlatSet : private Predicate {
  using storage_t = Container<T, Allocator>;

public:
  using const_iterator = typename storage_t::const_iterator;
  using const_pointer = typename storage_t::const_pointer;
  using const_reference = typename storage_t::const_reference;
  using const_reverse_iterator = typename storage_t::const_reverse_iterator;
  using difference_type = typename storage_t::difference_type;
  using iterator = typename storage_t::const_iterator;
  using pointer = typename storage_t::const_pointer;
  using reference = typename storage_t::const_reference;
  using reverse_iterator = typename storage_t::reverse_iterator;
  using size_type = typename storage_t::size_type;

  FlatSet() = default;

  bool empty() const noexcept {
    return storage_.empty();
  }
  size_type size() const noexcept {
    return storage_.size();
  }
  const_iterator begin() const noexcept {
    return const_iterator{storage_.begin()};
  }
  const_iterator cbegin() const noexcept {
    return const_iterator{storage_.cbegin()};
  }
  const_iterator end() const noexcept {
    return const_iterator{storage_.end()};
  }
  const_iterator cend() const noexcept {
    return const_iterator{storage_.cend()};
  }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator{storage_.rbegin()};
  }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator{storage_.rend()};
  }

  T const* data() const noexcept {
    return storage_.data();
  }

  T const& operator[](size_type pos) const noexcept {
    IDLE_ASSERT(pos < size());
    return *(storage_[pos]);
  }

  void clear() {
    storage_.clear();
  }

  std::pair<iterator, bool> insert(T const& value) {
    iterator lower = idle::lower_bound(storage_.begin(), storage_.end(), value,
                                       predicate());
    if (is_equal_element(lower, storage_.end(), value, predicate())) {
      return {lower, false};
    } else {
      return {storage_.insert(lower, value), true};
    }
  }
  std::pair<iterator, bool> insert(T&& value) {
    iterator lower = idle::lower_bound(storage_.begin(), storage_.end(), value,
                                       predicate());
    if (is_equal_element(lower, storage_.end(), value, predicate())) {
      return {lower, false};
    } else {
      return {storage_.insert(lower, value), true};
    }
  }

  template <class Itr>
  void insert(Itr begin, Itr end) {
    // TODO Maybe improve this
    for (Itr i = begin; i < end; ++i) {
      insert(*i);
    }
  }

  template <typename Comparable>
  const_iterator find(Comparable&& value) const noexcept {
    return find_first_lower_bound_of(storage_.cbegin(), storage_.cend(),
                                     std::forward<Comparable>(value),
                                     predicate());
  }

  const_iterator erase(const_iterator itr) {
    return storage_.erase(itr);
  }

  template <typename Comparable>
  size_type erase(Comparable&& value) {
    const_iterator lower = idle::lower_bound(storage_.begin(), storage_.end(),
                                             value, predicate());

    if ((lower != storage_.end()) && !(predicate())(value, *lower)) {
      storage_.erase(lower);
      return 1U;
    } else {
      return 0U;
    }
  }

  template <typename Comparable>
  bool contains(Comparable&& value) const noexcept {
    return find(std::forward<Comparable>(value)) != end();
  }

  void reserve(size_type size) {
    storage_.reserve(size);
  }

private:
  Predicate const& predicate() const noexcept {
    return *this;
  }

  storage_t storage_;
};

/// A mutable flat set implemented on top of an arbitrary container
///
/// \attention This implementation provides facilities for mutating the
///            value directly. Make sure not to mutate the values the ordering
///            function depends on, otherwise this set gets unsorted!
template <typename T, typename Predicate = std::less<>,
          typename Allocator = std::allocator<T>,
          template <typename, typename> class Container = std::vector>
class MutableFlatSet : private Predicate {
  using storage_t = Container<T, Allocator>;

public:
  using const_iterator = typename storage_t::const_iterator;
  using const_pointer = typename storage_t::const_pointer;
  using const_reference = typename storage_t::const_reference;
  using const_reverse_iterator = typename storage_t::const_reverse_iterator;
  using difference_type = typename storage_t::difference_type;
  using iterator = typename storage_t::iterator;
  using pointer = typename storage_t::pointer;
  using reference = typename storage_t::reference;
  using reverse_iterator = typename storage_t::reverse_iterator;
  using size_type = typename storage_t::size_type;

  MutableFlatSet() = default;

  bool empty() const noexcept {
    return storage_.empty();
  }
  size_type size() const noexcept {
    return storage_.size();
  }
  iterator begin() noexcept {
    return iterator{storage_.begin()};
  }
  const_iterator begin() const noexcept {
    return const_iterator{storage_.begin()};
  }
  const_iterator cbegin() const noexcept {
    return const_iterator{storage_.cbegin()};
  }
  iterator end() noexcept {
    return iterator{storage_.end()};
  }
  const_iterator end() const noexcept {
    return const_iterator{storage_.end()};
  }
  const_iterator cend() const noexcept {
    return const_iterator{storage_.cend()};
  }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator{storage_.rbegin()};
  }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator{storage_.rend()};
  }

  T* data() noexcept {
    return storage_.data();
  }
  T const* data() const noexcept {
    return storage_.data();
  }

  T const& operator[](size_type pos) const noexcept {
    IDLE_ASSERT(pos < size());
    return *(storage_[pos]);
  }

  void clear() {
    storage_.clear();
  }

  std::pair<iterator, bool> insert(T const& value) {
    iterator lower = idle::lower_bound(storage_.begin(), storage_.end(), value,
                                       predicate());
    if (is_equal_element(lower, storage_.end(), value, predicate())) {
      return {lower, false};
    } else {
      return {storage_.insert(lower, value), true};
    }
  }
  std::pair<iterator, bool> insert(T&& value) {
    iterator lower = idle::lower_bound(storage_.begin(), storage_.end(), value,
                                       predicate());
    if (is_equal_element(lower, storage_.end(), value, predicate())) {
      return {lower, false};
    } else {
      return {storage_.insert(lower, value), true};
    }
  }
  void push_back_unsorted(T const& value) {
    storage_.push_back(value);
  }
  void push_back_unsorted(T&& value) {
    storage_.push_back(std::move(value));
  }

  template <class Itr>
  void insert(Itr begin, Itr end) {
    // TODO Maybe improve this
    for (Itr i = begin; i < end; ++i) {
      insert(*i);
    }
  }
  template <class Itr>
  void insert_unsorted(Itr begin, Itr end) {
    storage_.insert(begin, end);
  }

  template <typename Comparable>
  iterator find(Comparable const& value) noexcept {
    return find_first_lower_bound_of(storage_.begin(), storage_.end(), value,
                                     predicate());
  }
  template <typename Comparable>
  const_iterator find(Comparable&& value) const noexcept {
    return find_first_lower_bound_of(storage_.cbegin(), storage_.cend(),
                                     std::forward<Comparable>(value),
                                     predicate());
  }

  iterator erase(iterator itr) {
    return storage_.erase(itr);
  }
  const_iterator erase(const_iterator itr) {
    return storage_.erase(itr);
  }

  template <typename Comparable>
  size_type erase(Comparable&& value) {
    const_iterator lower = idle::lower_bound(storage_.begin(), storage_.end(),
                                             value, predicate());

    if ((lower != storage_.end()) && !(predicate())(value, *lower)) {
      storage_.erase(lower);
      return 1U;
    } else {
      return 0U;
    }
  }

  template <typename Comparable>
  bool contains(Comparable&& value) const noexcept {
    return find(std::forward<Comparable>(value)) != end();
  }

  void reserve(size_type size) {
    storage_.reserve(size);
  }

  void resize(size_type size) {
    storage_.resize(size);
  }

private:
  Predicate const& predicate() const noexcept {
    return *this;
  }

  storage_t storage_;
};
} // namespace idle

#endif // IDLE_CORE_UTIL_FLAT_SET_HPP_INCLUDED
