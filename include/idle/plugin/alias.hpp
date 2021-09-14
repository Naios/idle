
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

#ifndef IDLE_PLUGIN_ALIAS_HPP_INCLUDED
#define IDLE_PLUGIN_ALIAS_HPP_INCLUDED

#include <cstdint>
#include <idle/core/api.hpp>

#ifdef IDLE_HAS_DYNAMIC_LINKING
#  define IDLE_ALIAS(NAME)                                                     \
    namespace idle_alias {                                                     \
    extern "C" {                                                               \
    IDLE_API_EXPORT                                                            \
    const void* idle_plugin_alias_##NAME = reinterpret_cast<const void*>(      \
        reinterpret_cast<std::intptr_t>(&NAME));                               \
    }                                                                          \
    }
#else // IDLE_HAS_DYNAMIC_LINKING
#  define IDLE_ALIAS(...)
#endif // IDLE_HAS_DYNAMIC_LINKING

#endif // IDLE_PLUGIN_ALIAS_HPP_INCLUDED
