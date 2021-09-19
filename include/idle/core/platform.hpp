
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

#ifndef IDLE_CORE_PLATFORM_HPP_INCLUDED
#define IDLE_CORE_PLATFORM_HPP_INCLUDED

#ifdef _WIN32
#  define IDLE_PLATFORM_WINDOWS

#  if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
#    error MSVC requires the '/experimental:preprocessor' conformance mode!
#  endif
#elif __APPLE_CC__
#  define IDLE_PLATFORM_MACOS
#else
#  define IDLE_PLATFORM_UNIX
#endif

#if defined(_MSC_VER)
#  define IDLE_COMPILER_MSVC
#elif defined(__clang__)
#  define IDLE_COMPILER_CLANG
#elif defined(__GNUC__)
#  define IDLE_COMPILER_GCC
#elif !defined(IDLE_HAS_SUPPRESSED_UNKNOWN_COMPILER)
#  error Unknown compiler!
#endif

#endif // IDLE_CORE_PLATFORM_HPP_INCLUDED
