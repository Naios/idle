
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

// clang-format off
#ifndef IDLE_CORE_EMBED_LOCALITY_EXECUTABLE_HPP_INCLUDED
#define IDLE_CORE_EMBED_LOCALITY_EXECUTABLE_HPP_INCLUDED
// clang-format on

#  include <idle/core/locality.hpp>
#  include <idle/core/ref.hpp>

namespace idle {
Ref<Locality> this_locality() noexcept {
  return Locality::permanent();
}
} // namespace idle

// clang-format off
#else // IDLE_CORE_EMBED_LOCALITY_EXECUTABLE_HPP_INCLUDED
#  error 'idle/core/locality_executable.hpp' should only be included once \
       per shared library or executable in one compilation unit!
#endif // IDLE_CORE_EMBED_LOCALITY_EXECUTABLE_HPP_INCLUDED
// clang-format on
