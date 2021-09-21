
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

#ifndef IDLE_CORE_API_HPP_INCLUDED
#define IDLE_CORE_API_HPP_INCLUDED

#include <idle/core/detail/preprocessor.hpp>
#include <idle/core/platform.hpp>

#ifdef IDLE_HAS_DYNAMIC_LINKING
#  ifdef IDLE_PLATFORM_WINDOWS
#    define IDLE_API_EXPORT __declspec(dllexport)
#    define IDLE_API_IMPORT __declspec(dllimport)
#    define IDLE_API_HIDDEN
#    define IDLE_API_SELECTANY __declspec(selectany)
#  else
#    define IDLE_API_EXPORT __attribute__((visibility("default")))
#    define IDLE_API_IMPORT
#    define IDLE_API_HIDDEN __attribute__((visibility("hidden")))
#    define IDLE_API_SELECTANY __attribute__((weak))
#  endif
#else // IDLE_HAS_DYNAMIC_LINKING
#  define IDLE_API_EXPORT
#  define IDLE_API_IMPORT
#  define IDLE_API_HIDDEN
#  define IDLE_API_SELECTANY
#endif // IDLE_HAS_DYNAMIC_LINKING

#define IDLE_DETAIL_API_DEF_1 IDLE_API_EXPORT
#define IDLE_DETAIL_API_DEF_0 IDLE_API_IMPORT
#define IDLE_DETAIL_API_IMPL2(NAME, DEF)                                       \
  IDLE_DETAIL_PRIMITIVE_CAT(IDLE_DETAIL_API_DEF_, DEF)
#define IDLE_DETAIL_API_IMPL1(NAME)                                            \
  IDLE_DETAIL_API_IMPL2(NAME, IDLE_DETAIL_IS_DEFINED(NAME))

/// Generates the compiler intrinsics for importing or exporting the symbol
/// to or from a shared library correctly.
///
/// This macro makes use of the automatically defined CMake `${target}_EXPORTS`
/// variable that is present if the target is built.
#define IDLE_API(TARGET_NAME) IDLE_DETAIL_API_IMPL1(TARGET_NAME##_EXPORTS)

// idle_EXPORTS is defined by CMake automatically
#if !defined(IDLE_HAS_DYNAMIC_LINKING) || defined(idle_EXPORTS)
#  define IDLE_API_IMPORT_IF_IMPORTED
#else
#  define IDLE_API_IMPORT_IF_IMPORTED IDLE_API_IMPORT
#endif

// idle_EXPORTS is defined by CMake automatically
#if !defined(IDLE_HAS_DYNAMIC_LINKING) || defined(idle_EXPORTS)
#  define IDLE_API_EXTERN_IF_NOT_IMPORTED extern
#else
#  define IDLE_API_EXTERN_IF_NOT_IMPORTED
#endif

#if defined(IDLE_COMPILER_CLANG) || defined(IDLE_COMPILER_GCC)
#  define IDLE_LIKELY(EXPR) __builtin_expect(!!(EXPR), 1)
#  define IDLE_UNLIKELY(EXPR) __builtin_expect(!!(EXPR), 0)
#else
#  define IDLE_LIKELY(EXPR) !!(EXPR)
#  define IDLE_UNLIKELY(EXPR) !!(EXPR)
#endif

#define IDLE_CONSTEVAL constexpr
#define IDLE_CXX17_CONSTEXPR

#endif // IDLE_CORE_API_HPP_INCLUDED
