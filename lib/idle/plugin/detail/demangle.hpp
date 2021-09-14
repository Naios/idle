
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

#ifndef IDLE_PLUGIN_DETAIL_DEMANGLE_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_DEMANGLE_HPP_INCLUDED

#include <string>
#include <unordered_map>
#include <idle/core/dep/optional.hpp>

namespace idle {
enum class mangeling {
  itanium,  //< Uses itanium name mangeling
  microsoft //< Uses microsoft name mangeling
};

/// Demangles the given C++ function or class name
optional<std::string> demangle(mangeling selected, char const* name);

/// Demangles the given C++ function or class names
std::unordered_map<std::string, std::string>
demangle_all(mangeling selected, std::vector<std::string> names);
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_DEMANGLE_HPP_INCLUDED
