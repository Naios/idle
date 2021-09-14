
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

#ifndef IDLE_CORE_UTIL_THREAD_NAME_HPP_INCLUDED
#define IDLE_CORE_UTIL_THREAD_NAME_HPP_INCLUDED

#include <thread>
#include <idle/core/api.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
/// Sets the name of the current thread, the name is limited to 64
/// characters and wil be cut accordingly.
IDLE_API(idle) void set_this_thread_name(StringView name = {}) noexcept;

/// Returns the name of the current thread if there is any assigned
IDLE_API(idle) StringView this_thread_name() noexcept;

/// Returns the native_handle_type of the current thread.
IDLE_API(idle) std::thread::native_handle_type this_thread_handle() noexcept;
} // namespace idle

#endif // IDLE_CORE_UTIL_THREAD_NAME_HPP_INCLUDED
