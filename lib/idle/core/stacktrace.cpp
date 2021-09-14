
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

#include <csignal>
#include <iostream>
#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include <boost/stacktrace.hpp>
#include <idle/core/stacktrace.hpp>

static std::string generate_backtrace_path() {
  boost::filesystem::path path = boost::dll::program_location().parent_path();
  path /= "backtrace.dump";
  return path.generic_string();
}

static std::string const backtrace_path = generate_backtrace_path();

IDLE_API(idle) void idle_signal_handler(int signal) {
  ::signal(signal, SIG_DFL);
  boost::stacktrace::safe_dump_to(backtrace_path.c_str());
  ::raise(SIGABRT);
}

static void init_signal_handler() {
  ::signal(SIGSEGV, &idle_signal_handler);
  ::signal(SIGABRT, &idle_signal_handler);
}

namespace idle {
bool stacktrace_init() {
  if (boost::filesystem::exists(backtrace_path)) {
    // there is a backtrace
    std::ifstream ifs(backtrace_path);

    boost::stacktrace::stacktrace st = boost::stacktrace::stacktrace::from_dump(
        ifs);
    std::cout << "Previous run crashed:\n" << st << std::endl;

    // cleaning up
    ifs.close();
    boost::filesystem::remove(backtrace_path);
    return false;
  }

  init_signal_handler();
  return true;
}
} // namespace idle
