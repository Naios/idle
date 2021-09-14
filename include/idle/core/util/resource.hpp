
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

#ifndef IDLE_CORE_UTIL_RESOURCE_HPP_INCLUDED
#define IDLE_CORE_UTIL_RESOURCE_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <idle/core/util/string_view.hpp>

/// Returns an idle::StringView which references a statically
/// resource embedded through the idle_target_resource CMake command.
#define IDLE_RESOURCE(NAME)                                                    \
  (([]() -> ::idle::StringView {                                               \
    extern ::std::uint64_t _idle_resource_##NAME##_data[];                     \
    extern ::std::size_t _idle_resource_##NAME##_length;                       \
    return ::idle::StringView(reinterpret_cast<char const*>(                   \
                                  _idle_resource_##NAME##_data),               \
                              _idle_resource_##NAME##_length);                 \
  })())

#endif // IDLE_CORE_UTIL_RESOURCE_HPP_INCLUDED
