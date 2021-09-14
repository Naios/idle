
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

#ifndef IDLE_CORE_UTIL_ASSERT_HPP_INCLUDED
#define IDLE_CORE_UTIL_ASSERT_HPP_INCLUDED

#include <cassert>
#include <idle/core/api.hpp>

#ifndef NDEBUG
namespace idle {
namespace detail {
// This function is explicitly not marked as constexpr to trigger a failure
// when the context gets constant evaluated.
template <typename T>
void assert_failed(T&& cb) noexcept {
  cb();
}
} // namespace detail
} // namespace idle

/// Defines a constexpr compatible `assert`
#  define IDLE_ASSERT(EXPR)                                                    \
    (IDLE_LIKELY(!!(EXPR)) ? void(0) : (::idle::detail::assert_failed([]() {   \
      assert(!#EXPR);                                                          \
    })))
#else
#  define IDLE_ASSERT(EXPR) void(0)
#endif

/// Defines a constexpr compatible check, that also executes the expression
/// unchecked when NDEBUG is defined.
#ifndef NDEBUG
#  define IDLE_CHECK(EXPR) IDLE_ASSERT(EXPR)
#else
#  define IDLE_CHECK(EXPR) ((void)(EXPR))
#endif

#endif // IDLE_CORE_UTIL_ASSERT_HPP_INCLUDED
