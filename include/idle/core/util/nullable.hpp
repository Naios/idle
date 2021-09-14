
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

#ifndef IDLE_CORE_UTIL_NULLABLE_HPP_INCLUDED
#define IDLE_CORE_UTIL_NULLABLE_HPP_INCLUDED

#include <iterator>
#include <type_traits>
#include <idle/core/util/assert.hpp>

namespace idle {
template <typename>
class Nullable;

/// Makes a nullable from the given type
template <typename T>
constexpr Nullable<T> make_nullable(Nullable<T> nullable) noexcept {
  return nullable;
}
/// Makes a nullable from the given type
template <typename T>
constexpr Nullable<T> make_nullable(T& nullable) noexcept {
  return nullable;
}

/// An optional type specialized to wrap nullable pointers
///
/// This class has some advantages over llvm::Optional because it uses
/// nullptr as obvious absent state and returns a reference to the object
/// directly when calling the operator->.
/// This makes the nullable a near drop-in replacement for pointers
/// in order to catch misusage and null dereferencing early.
///
/// The nullable also supports the container requirements
/// for supporting possible one child iterations.
template <typename T>
class Nullable {
  template <typename>
  friend class Nullable;
  static_assert(!std::is_pointer<T>::value && !std::is_reference<T>::value,
                "Expected a plain type!");

public:
  using reference = T&;
  using pointer = T*;
  using iterator = pointer;
  using const_iterator = std::remove_const_t<
      std::remove_pointer_t<pointer>> const*;

  constexpr Nullable() = default;
  template <typename O, typename = std::enable_if_t<
                            std::is_convertible<O*, pointer>::value>>
  constexpr /*implicit*/ Nullable(O& ptr)
    : ptr_(std::addressof(ptr)) {}
  template <typename O, typename = std::enable_if_t<
                            std::is_convertible<O*, pointer>::value>>
  constexpr /*implicit*/ Nullable(O* ptr)
    : ptr_(ptr) {}
  template <typename O, typename = std::enable_if_t<
                            std::is_convertible<O*, pointer>::value>>
  constexpr /*implicit*/ Nullable(Nullable<O> right)
    : ptr_(right.ptr_) {}

  /// Returns true when the nullable is empty
  constexpr bool empty() const noexcept {
    return ptr_ == nullptr;
  }

  /// Returns true when the operator isn't null
  explicit constexpr operator bool() const noexcept {
    return !empty();
  }
  /// Is convertible to the pointer
  explicit constexpr operator pointer() const noexcept {
    return ptr_;
  }

  /// Returns the beginning of the nullable
  constexpr iterator begin() noexcept {
    return ptr_;
  }
  /// Returns the beginning of the nullable
  constexpr const_iterator begin() const noexcept {
    return ptr_;
  }
  /// Returns the end of the nullable
  constexpr iterator end() noexcept {
    return !empty() ? std::next(begin()) : nullptr;
  }
  /// Returns the end of the nullable
  constexpr const_iterator end() const noexcept {
    return !empty() ? std::next(begin()) : nullptr;
  }

  /// Returns the pointer of the nullable while asserting that it isn't null
  constexpr pointer operator->() const noexcept {
    IDLE_ASSERT(ptr_);
    return ptr_;
  }
  /// Returns the reference of the nullable while asserting that it isn't null
  constexpr reference operator*() const noexcept {
    IDLE_ASSERT(ptr_);
    return *ptr_;
  }

  /// Maps the nullable with the given function to a plain type or
  /// to a new nullable which can be absent again.
  template <typename F>
  constexpr auto map(F&& fn) -> decltype(make_nullable(
      std::forward<F>(fn)(std::declval<reference>()))) {
    if (!empty()) {
      return std::forward<F>(fn)(*ptr_);
    } else {
      return {};
    }
  }

private:
  pointer ptr_{nullptr};
};
} // namespace idle

#endif // IDLE_CORE_UTIL_NULLABLE_HPP_INCLUDED
