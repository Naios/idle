
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

#ifndef IDLE_CORE_DETAIL_POINTER_UNION_HPP_INCLUDED
#define IDLE_CORE_DETAIL_POINTER_UNION_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <idle/core/detail/traits.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/nullable.hpp>

namespace idle {
namespace detail {
template <typename T, typename... Args>
struct is_element_of;
template <typename T, typename... Args>
struct is_element_of<T, T, Args...> : std::true_type {};
template <typename T>
struct is_element_of<T> : std::false_type {};
template <typename T, typename U, typename... Args>
struct is_element_of<T, U, Args...> : is_element_of<T, Args...> {};

template <typename... Args>
class pointer_union {
public:
  using index_t = std::uint8_t;

  template <typename T>
  static constexpr index_t index_of() noexcept {
    return 1 + index_of_t<T, Args...>::value;
  }

  pointer_union() = default;
  ~pointer_union() = default;
  explicit pointer_union(std::nullptr_t) {}

  template <typename T,
            std::enable_if_t<is_element_of<T, Args...>::value>* = nullptr>
  explicit pointer_union(T& ptr)
    : ptr_(std::addressof(ptr))
    , index_(index_of<T>()) {}

  pointer_union(pointer_union const&) = default;
  pointer_union& operator=(pointer_union const&) = default;
  pointer_union(pointer_union&& other) noexcept
    : ptr_(std::exchange(other.ptr_, {}))
    , index_(std::exchange(other.index_, {})) {}
  pointer_union& operator=(pointer_union&& other) noexcept {
    ptr_ = std::exchange(other.ptr_, {});
    index_ = std::exchange(other.index_, {});
    return *this;
  }

  pointer_union& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  index_t index() const noexcept {
    IDLE_ASSERT(index_ <= sizeof...(Args));
    return index_;
  }

  void set(std::nullptr_t) noexcept {
    reset();
  }

  template <typename T>
  void set(T& ptr) noexcept {
    ptr_ = std::addressof(ptr);
    index_ = index_of<std::decay_t<T>>();
  }

  template <typename T>
  bool is() const noexcept {
    return index() == index_of<T>();
  }

  bool empty() const noexcept {
    return index() == 0U;
  }

  explicit operator bool() const noexcept {
    return !empty();
  }

  void reset() noexcept {
    ptr_ = nullptr;
    index_ = 0U;
  }

  template <typename T>
  T& get() const noexcept {
    IDLE_ASSERT(is<T>());
    IDLE_ASSERT(raw());
    return *static_cast<T*>(raw());
  }

  template <typename T>
  Nullable<T> try_get() const noexcept {
    if (is<T>()) {
      IDLE_ASSERT(raw());
      return *static_cast<T*>(raw());
    } else {
      return {};
    }
  }

  void* raw() const noexcept {
    return ptr_;
  }

  bool operator==(pointer_union const& other) const noexcept {
    return raw() == other.raw();
  }
  bool operator!=(pointer_union const& other) const noexcept {
    return raw() != other.raw();
  }
  bool operator>=(pointer_union const& other) const noexcept {
    return raw() >= other.raw();
  }
  bool operator<=(pointer_union const& other) const noexcept {
    return raw() <= other.raw();
  }
  bool operator>(pointer_union const& other) const noexcept {
    return raw() > other.raw();
  }
  bool operator<(pointer_union const& other) const noexcept {
    return raw() < other.raw();
  }

private:
  void* ptr_{nullptr};
  index_t index_{0U}; // Maybe embed the index inside the pointer
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_POINTER_UNION_HPP_INCLUDED
