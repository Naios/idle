
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

#ifndef IDLE_CORE_DETAIL_REGISTRY_TRAITS_HPP_INCLUDED
#define IDLE_CORE_DETAIL_REGISTRY_TRAITS_HPP_INCLUDED

#include <type_traits>
#include <utility>
#include <idle/core/context.hpp>
#include <idle/core/detail/detection.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/ref.hpp>

namespace idle {
namespace detail {
template <typename T, typename... Args>
Ref<Interface> do_create_impl(std::true_type, Args&&... args) {
  return T::create(std::forward<Args>(args)...);
}
template <typename T, typename... Args>
Ref<Interface> do_create_impl(std::false_type, Args&&...) {
  IDLE_DETAIL_UNREACHABLE();
}

template <typename T, typename... Args>
Ref<Interface> do_create(Args&&... args) {
  return do_create_impl<T>(has_create<T>{}, std::forward<Args>(args)...);
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_REGISTRY_TRAITS_HPP_INCLUDED
