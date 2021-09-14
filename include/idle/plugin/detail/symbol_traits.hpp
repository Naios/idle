
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

#ifndef IDLE_PLUGIN_DETAIL_SYMBOL_TRAITS_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_SYMBOL_TRAITS_HPP_INCLUDED

namespace idle {
namespace detail {
template <typename T>
struct symbol_trait {
  using ptr_type = T*;

  static ptr_type symbol_cast(void* ptr) {
    return static_cast<T*>(ptr);
  }

  static ptr_type alias_cast(void* ptr) {
    return *static_cast<T**>(ptr);
  }
};
template <typename Ret, typename... Args>
struct symbol_trait<Ret(Args...)> {
  using ptr_type = Ret (*)(Args...);

  static ptr_type symbol_cast(void* ptr) {
    return reinterpret_cast<ptr_type>(ptr);
  }

  static ptr_type alias_cast(void* ptr) {
    return *static_cast<ptr_type*>(ptr);
  }
};
} // namespace detail
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_SYMBOL_TRAITS_HPP_INCLUDED
