
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

#ifndef IDLE_PLUGIN_DETAIL_SHARED_LIBRARY_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_SHARED_LIBRARY_HPP_INCLUDED

#include <string>
#include <boost/dll/shared_library.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/detail/unordered_set.hpp>

namespace idle {
namespace detail {
namespace shared_library {
/// Represents the native system handle for shared libraries
using handle_t = boost::dll::shared_library::native_handle_t;

/// Loads the given shared library, returns a present handle on success
optional<handle_t> load(char const* path, boost::dll::fs::error_code& ec);

/// Lookups the given symbol in the shared library
void* lookup(handle_t handle, char const* name);

/// Unloads the given shared library, returns true on success.
bool unload(handle_t handle);

/// Returns all shared libraries loaded by this application
unordered_set<std::string> currently_loaded();

/// Converts the given plugin name to lowercase in case the filesystem is
/// case insensitive like on Windows.
///
/// Windows/MSVC really likes to mix the cases for the same DLL such
/// as KERNEL32.DLL, KERNEL32.dll and kernel32.dll.
void normalize_name(std::string& path);
} // namespace shared_library
} // namespace detail
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_SHARED_LIBRARY_HPP_INCLUDED
