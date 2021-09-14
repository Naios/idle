
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

#ifndef IDLE_CORE_DETAIL_SECTION_HPP_INCLUDED
#define IDLE_CORE_DETAIL_SECTION_HPP_INCLUDED

// This file contains code of boost.dll partially,
// which was published with the following license:
//
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Copyright 2015 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>
#include <idle/core/api.hpp>

#ifdef IDLE_HAS_DYNAMIC_LINKING
#  ifdef _MSC_VER
/// Idle version of BOOST_DLL_SECTION
/// The credits are going to boost.dll for this mostly.
#    define IDLE_DETAIL_SECTION(IDLE_DETAIL_SECTION_NAME,                           \
                                IDLE_DETAIL_PERMISSIONS)                            \
      static_assert(sizeof(IDLE_DETAIL_STRINGIFY(IDLE_DETAIL_SECTION_NAME)) <       \
                        10,                                                         \
                    "Some platforms require section names to be at most 8 "         \
                    "bytes");                                                       \
      __pragma(section(                                                             \
          IDLE_DETAIL_STRINGIFY(IDLE_DETAIL_SECTION_NAME),                          \
          IDLE_DETAIL_PERMISSIONS)) __declspec(allocate(#IDLE_DETAIL_SECTION_NAME), \
                                               dllexport)
#  elif defined(__APPLE__)
/// Idle version of BOOST_DLL_SECTION
/// The credits are going to boost.dll for this mostly.
#    define IDLE_DETAIL_SECTION(IDLE_DETAIL_SECTION_NAME,                      \
                                IDLE_DETAIL_PERMISSIONS)                       \
      static_assert(sizeof(IDLE_DETAIL_STRINGIFY(IDLE_DETAIL_SECTION_NAME)) <  \
                        10,                                                    \
                    "Some platforms require section names to be at most 8 "    \
                    "bytes");                                                  \
      __attribute__((section("__DATA," IDLE_DETAIL_STRINGIFY(                  \
                         IDLE_DETAIL_SECTION_NAME)),                           \
                     visibility("default")))
#  else
/// Idle version of BOOST_DLL_SECTION
/// The credits are going to boost.dll for this mostly.
#    define IDLE_DETAIL_SECTION(IDLE_DETAIL_SECTION_NAME,                      \
                                IDLE_DETAIL_PERMISSIONS)                       \
      static_assert(sizeof(IDLE_DETAIL_STRINGIFY(IDLE_DETAIL_SECTION_NAME)) <  \
                        10,                                                    \
                    "Some platforms require section names to be at most 8 "    \
                    "bytes");                                                  \
      __attribute__((section(IDLE_DETAIL_STRINGIFY(IDLE_DETAIL_SECTION_NAME)), \
                     visibility("default")))
#  endif
/// Idle version of BOOST_DLL_ALIAS_SECTIONED
/// The credits are going to boost.dll for this mostly.
#  define IDLE_DETAIL_SECTIONED_ALIAS(NAME, ALIAS_NAME, SECTION)               \
    namespace idle_sectioned_alias {                                           \
    extern "C" {                                                               \
    IDLE_DETAIL_SECTION(SECTION, read)                                         \
    const void* ALIAS_NAME = reinterpret_cast<const void*>(                    \
        reinterpret_cast<std::intptr_t>(&NAME));                               \
    }                                                                          \
    }
#else // IDLE_HAS_DYNAMIC_LINKING
#  define IDLE_DETAIL_SECTION(...)
#  define IDLE_DETAIL_SECTIONED_ALIAS(...)
#endif // IDLE_HAS_DYNAMIC_LINKING

#endif // IDLE_CORE_DETAIL_SECTION_HPP_INCLUDED
