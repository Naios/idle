
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

#ifndef IDLE_CORE_UTIL_ARRAY_HPP_INCLUDED
#define IDLE_CORE_UTIL_ARRAY_HPP_INCLUDED

#include <cstddef>
#include <initializer_list>
#include <idle/core/util/assert.hpp>

namespace idle {
/// A C++14 constexpr capable improved polyfill for std::array
template <typename T, std::size_t Size>
class Array {
public:
  constexpr Array() noexcept
    : values_{} {}
  explicit constexpr Array(T values[Size]) noexcept
    : values_(values) {}
  template <typename Iterator>
  constexpr Array(Iterator begin, Iterator end) noexcept
    : values_{} {
    std::size_t i = 0;
    for (; i < Size && begin != end; ++i, ++begin) {
      values_[i] = *begin;
    }
    for (; i < Size; ++i) {
      values_[i] = {};
    }
  }
  constexpr Array(std::initializer_list<T> list) noexcept
    : Array(list.begin(), list.end()) {}

  constexpr T* begin() noexcept {
    return values_;
  }
  constexpr T const* begin() const noexcept {
    return values_;
  }
  constexpr T* end() noexcept {
    return values_ + Size;
  }
  constexpr T const* end() const noexcept {
    return values_ + Size;
  }
  constexpr T* data() noexcept {
    return values_;
  }
  constexpr T const* data() const noexcept {
    return values_;
  }
  constexpr T& operator[](std::size_t i) noexcept {
    return *(values_ + i);
  }
  constexpr T const& operator[](std::size_t i) const noexcept {
    return *(values_ + i);
  }
  constexpr std::size_t size() const noexcept {
    return Size;
  }
  constexpr bool empty() const noexcept {
    return false;
  }

private:
  T values_[Size];
};

namespace detail {
struct AssignDummy {
  template <typename O>
  constexpr AssignDummy& operator=(O&&) noexcept {
    return *this;
  }
};
} // namespace detail

template <typename T>
class Array<T, 0> {
public:
  constexpr Array() noexcept {}
  template <typename Iterator>
  constexpr Array(Iterator begin, Iterator end) noexcept {
    IDLE_ASSERT(begin == end);

    (void)begin;
    (void)end;
  }
  constexpr Array(std::initializer_list<T> list) noexcept
    : Array(list.begin(), list.end()) {}

  constexpr T* begin() noexcept {
    return nullptr;
  }
  constexpr T const* begin() const noexcept {
    return nullptr;
  }
  constexpr T* end() noexcept {
    return nullptr;
  }
  constexpr T const* end() const noexcept {
    return nullptr;
  }
  constexpr T* data() noexcept {
    return nullptr;
  }
  constexpr T const* data() const noexcept {
    return nullptr;
  }
  constexpr std::size_t size() const noexcept {
    return 0;
  }
  constexpr detail::AssignDummy operator[](std::size_t i) const noexcept {
    IDLE_ASSERT(false && i);
    (void)i;
    return {};
  }
  constexpr bool empty() const noexcept {
    return true;
  }
};
} // namespace idle

#endif // IDLE_CORE_UTIL_ARRAY_HPP_INCLUDED
