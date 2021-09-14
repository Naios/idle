
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

#ifndef IDLE_CORE_DETAIL_TYPE_NAME_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_TYPE_NAME_IMPL_HPP_INCLUDED

#include <cstdint>
#include <idle/core/detail/macros.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
namespace detail {
template <typename T>
constexpr StringView type_name_impl() noexcept {
  constexpr StringView str(IDLE_DETAIL_PRETTY_FUNCTION_NAME);
#ifdef IDLE_COMPILER_MSVC
  // MSVC produces something like:
  // clang-format off
    // class idle::StringView __cdecl idle::detail::type_name_impl<struct idle::MyType>(void) noexcept
  // clang-format on
  constexpr StringView name = str.substr(60, str.size() - 60 - 16);

  constexpr StringView enum_str("enum ");
  constexpr StringView class_str("class ");
  constexpr StringView struct_str("struct ");

  if (name.starts_with(enum_str)) {
    return name.substr(enum_str.size());
  } else if (name.starts_with(class_str)) {
    return name.substr(class_str.size());
  } else if (name.starts_with(struct_str)) {
    return name.substr(struct_str.size());
  } else {
    return name;
  }
#else
  // Clang produces something like:
  // clang-format off
    // idle::StringView idle::detail::type_name_impl() [T = idle::MyType]
  // clang-format on
  return str.substr(53, str.size() - 53 - 1);
#endif
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_TYPE_NAME_IMPL_HPP_INCLUDED
