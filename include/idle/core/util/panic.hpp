
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

#ifndef IDLE_CORE_UTIL_PANIC_HPP_INCLUDED
#define IDLE_CORE_UTIL_PANIC_HPP_INCLUDED

#include <cstddef>
#include <idle/core/api.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
namespace detail {
[[noreturn]] IDLE_API(idle) void panic(StringView msg) noexcept;
}
} // namespace idle

/// Defines a panic that guarantees that the expression EXPR evaluates to true,
/// otherwise the application is aborted with the given MESSAGE.
///
/// \note IDLE_PANIC checks on every build configuration
#define IDLE_PANIC(EXPR, MESSAGE)                                              \
  (IDLE_LIKELY(!!(EXPR))) ? void(0) : ::idle::detail::panic(MESSAGE)

#endif // IDLE_CORE_UTIL_PANIC_HPP_INCLUDED
