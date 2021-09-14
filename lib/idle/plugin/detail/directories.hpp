
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

#ifndef IDLE_PLUGIN_DETAIL_DIRECTORIES_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_DIRECTORIES_HPP_INCLUDED

#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <idle/core/detail/log.hpp>

namespace idle {
namespace detail {
/// Creates a directory or removes all files in it if it exists
inline void reset_directory(boost::filesystem::path const& dir,
                            bool clean = true) {
  if (exists(dir)) {
    if (clean) {
      IDLE_ASSERT(is_directory(dir)); // TODO Fixme

      IDLE_DETAIL_LOG_DEBUG("Clearing dir {}", dir);

      using boost::filesystem::directory_iterator;
      directory_iterator const end;

      for (directory_iterator itr(dir); itr != end; ++itr) {
        remove_all(itr->path());
      }
    }
  } else {
    boost::system::error_code ec;
    boost::filesystem::create_directories(dir, ec);

    if (ec) {
      IDLE_DETAIL_LOG_ERROR("Failed to create the directories of '{}' ('{}')!",
                            dir.generic_string(), ec.message());
    }
  }
}
} // namespace detail
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_DIRECTORIES_HPP_INCLUDED
