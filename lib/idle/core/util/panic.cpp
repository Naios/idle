
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

#include <cstdio>
#include <cstdlib>
#include <thread>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/core/util/panic.hpp>
#include <idle/core/util/thread_name.hpp>

namespace idle {
namespace detail {
[[noreturn]] void panic(StringView msg) noexcept {
  StringView const thread_name = this_thread_name();
  auto const name = boost::dll::program_location().filename().generic_string();

  fmt::print(stderr,
             fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_magenta),
             FMT_STRING("{} paniced in thread {} ('{}'): {}"), name,
             std::this_thread::get_id(), thread_name ? thread_name : "?", msg);

  std::fflush(stderr);
  std::quick_exit(EXIT_FAILURE);
}
} // namespace detail
} // namespace idle
