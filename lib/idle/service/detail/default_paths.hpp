
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

#ifndef IDLE_SERVICE_DETAIL_DEFAULT_PATHS_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_DEFAULT_PATHS_HPP_INCLUDED

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace idle {
namespace detail {
inline std::string default_program_name() {
  using path = boost::filesystem::path;
  path const executable = boost::dll::program_location();
  return executable.stem().generic_string();
}

inline boost::filesystem::path
get_default_path(boost::filesystem::path const& extension) {
  using path = boost::filesystem::path;

  path const executable = boost::dll::program_location();
  // TODO executable.replace_extension(extension);

  path const name = executable.stem();
  path const root = executable.parent_path().parent_path();

  return (root / "etc" /
          fmt::format(FMT_STRING("{}.{}"), name.generic_string(),
                      extension.generic_string()))
      .generic_string();
}

} // namespace detail
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_DEFAULT_PATHS_HPP_INCLUDED
