
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

#ifndef IDLE_CORE_UTIL_DEREF_HPP_INCLUDED
#define IDLE_CORE_UTIL_DEREF_HPP_INCLUDED

#include <utility>
#include <idle/core/detail/chaining.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
namespace detail {
template <typename T>
constexpr auto& deref_impl(T* obj) noexcept {
  IDLE_ASSERT(obj);
  return *obj;
}
template <typename T>
constexpr auto& deref_impl(T& obj) noexcept(noexcept(obj.operator->())) {
  return deref_impl(chain(obj.operator->()));
}
} // namespace detail

/// Dereferences a given object or pointer correctly and returns a reference
/// such that the rules of operator-> are taken into account.
template <typename T>
constexpr auto& deref(T&& obj) noexcept(
    noexcept(detail::deref_impl(detail::chain(std::forward<T>(obj))))) {
  return detail::deref_impl(detail::chain(std::forward<T>(obj)));
}
} // namespace idle

#endif // IDLE_CORE_UTIL_DEREF_HPP_INCLUDED
