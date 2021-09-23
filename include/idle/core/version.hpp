
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

#ifndef IDLE_CORE_VERSION_HPP_INCLUDED
#define IDLE_CORE_VERSION_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
namespace version {
extern IDLE_API(idle) StringView const cmake_command;
extern IDLE_API(idle) StringView const cmake_version;
extern IDLE_API(idle) StringView const cmake_generator;
extern IDLE_API(idle) StringView const cmake_build_type;
extern IDLE_API(idle) StringView const cmake_c_flags;
extern IDLE_API(idle) StringView const cmake_cxx_flags;
extern IDLE_API(idle) StringView const cmake_exe_linker_flags;
extern IDLE_API(idle) StringView const cmake_shared_linker_flags;
} // namespace version
} // namespace idle

#endif // IDLE_CORE_VERSION_HPP_INCLUDED
