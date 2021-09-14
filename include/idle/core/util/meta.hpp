
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

#ifndef IDLE_CORE_UTIL_META_HPP_INCLUDED
#define IDLE_CORE_UTIL_META_HPP_INCLUDED

#include <type_traits>

namespace idle {
/// C++17 identity
template <typename...>
struct identity {};

/// C++20 type_identity
template <typename T>
struct type_identity {
  using type = T;
};

/// C++20 type_identity_t
template <typename T>
using type_identity_t = typename type_identity<T>::type;

namespace detail {
// Equivalent to C++17's std::void_t which targets a bug in GCC,
// that prevents correct SFINAE behavior.
// See http://stackoverflow.com/questions/35753920 for details.
template <typename...>
struct deduce_to_void : type_identity<void> {};
} // namespace detail

/// C++17 void_t
template <typename... T>
using void_t = typename detail::deduce_to_void<T...>::type;

// Removes constant, volatile and reference qualifiers
template <typename T>
using unrefcv_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T>
using always_false = std::integral_constant<bool, !std::is_same<T, T>::value>;
} // namespace idle

#endif // IDLE_CORE_UTIL_META_HPP_INCLUDED
