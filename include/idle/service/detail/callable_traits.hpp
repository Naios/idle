
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

#ifndef IDLE_SERVICE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED

#include <memory>
#include <type_traits>
#include <utility>
#include <idle/core/util/meta.hpp>

namespace idle {
namespace detail {
template <typename T, typename = void>
struct callable_present_trait {
  static constexpr bool is_present(T const&) noexcept {
    return true;
  }
};
template <typename T>
struct callable_present_trait<
    T, void_t<decltype(static_cast<bool>(std::declval<T const&>()))>> {
  static constexpr bool is_present(T const& obj) noexcept {
    return static_cast<bool>(obj);
  }
};
} // namespace detail
} // namespace idle
#endif // IDLE_SERVICE_DETAIL_CALLABLE_TRAITS_HPP_INCLUDED
