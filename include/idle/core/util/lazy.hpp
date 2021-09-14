
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

#ifndef IDLE_CORE_UTIL_LAZY_HPP_INCLUDED
#define IDLE_CORE_UTIL_LAZY_HPP_INCLUDED

#include <memory>
#include <utility>
#include <idle/core/util/assert.hpp>

namespace idle {
/// A simple non movable and non copyable implementation of an optional
template <typename T>
class Lazy {
public:
  Lazy() = default;
  ~Lazy() {
    reset();
  }

  Lazy(Lazy const&) = default;
  Lazy(Lazy&&) = default;
  Lazy& operator=(Lazy const&) = default;
  Lazy& operator=(Lazy&&) = default;

  constexpr bool empty() const noexcept {
    return !has_value_;
  }
  explicit constexpr operator bool() const noexcept {
    return has_value_;
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    reset();

    new (reinterpret_cast<T*>(&storage_)) T(std::forward<Args>(args)...);
    has_value_ = true;
  }

  void reset() {
    if (!empty()) {
      reinterpret_cast<T*>(&storage_)->~T();
      has_value_ = false;
    }
  }

  T& value() noexcept {
    IDLE_ASSERT(!empty());
    return *reinterpret_cast<T*>(&storage_);
  }
  T const& value() const noexcept {
    IDLE_ASSERT(!empty());
    return *reinterpret_cast<T const*>(&storage_);
  }
  T* operator->() noexcept {
    return std::addressof(value());
  }
  T const* operator->() const noexcept {
    return std::addressof(value());
  }
  T& operator*() noexcept {
    return value();
  }
  T const& operator*() const noexcept {
    return value();
  }

private:
  bool has_value_{false};
  std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};
} // namespace idle

#endif // IDLE_CORE_UTIL_LAZY_HPP_INCLUDED
