
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

#ifndef IDLE_CORE_DETAIL_PREPROCESSOR_HPP_INCLUDED
#define IDLE_CORE_DETAIL_PREPROCESSOR_HPP_INCLUDED

// Defines C/C++ specific macro tricks, some (but not all) of them described in:
// https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
// and implemented in the cloak library by Paul Fultz II and licensed
// under the Boost 1.0 license.

#define IDLE_DETAIL_CAT(a, ...) IDLE_DETAIL_PRIMITIVE_CAT(a, __VA_ARGS__)
#define IDLE_DETAIL_PRIMITIVE_CAT(a, ...) a##__VA_ARGS__

#define IDLE_DETAIL_IIF(c) IDLE_DETAIL_CAT(IDLE_DETAIL_IIF_, c)
#define IDLE_DETAIL_IIF_0(t, f) f
#define IDLE_DETAIL_IIF_1(t, f) t

#define IDLE_DETAIL_CHECK_N(x, n, ...) n
#define IDLE_DETAIL_CHECK(...) IDLE_DETAIL_CHECK_N(__VA_ARGS__, 0, )
#define IDLE_DETAIL_PROBE(x) x, 1,

#define IDLE_DETAIL_IS_PAREN(x) IDLE_DETAIL_CHECK(IDLE_DETAIL_IS_PAREN_PROBE x)
#define IDLE_DETAIL_IS_PAREN_PROBE(...) IDLE_DETAIL_PROBE(~)

#define IDLE_DETAIL_ARGS_FIRST(FIRST, ...) FIRST
#define IDLE_DETAIL_ARGS_REST(FIRST, ...) __VA_ARGS__

/// Deduces to a 1 if the definition is defined and 1, otherwise this
/// macro will deduce to 0.
///
/// \attention On MSVC this macro requires the new GCC/Clang compatible
/// preprocessor which can be enabled through '/Zc:preprocessor' or
/// '/experimental:preprocessor' depending on your MSVC version.
#define IDLE_DETAIL_IS_DEFINED(x) IDLE_DETAIL_IS_DEFINED_IMPL1(x)

/// Converts the given macro to a string
#define IDLE_DETAIL_STRINGIFY(NAME) IDLE_DETAIL_STRINGIFY_IMPL(NAME)

// Implementation part
#define IDLE_DETAIL_STRINGIFY_IMPL(NAME) #NAME

#define IDLE_DETAIL_IS_DEFINED_IMPL4_1 0,
#define IDLE_DETAIL_IS_DEFINED_IMPL2(ARG) IDLE_DETAIL_CHECK_N(ARG 1, 0, )
#define IDLE_DETAIL_IS_DEFINED_IMPL1(ARG)                                      \
  IDLE_DETAIL_IS_DEFINED_IMPL2(IDLE_DETAIL_IS_DEFINED_IMPL4_##ARG)

#endif // IDLE_CORE_DETAIL_PREPROCESSOR_HPP_INCLUDED
