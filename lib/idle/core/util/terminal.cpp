
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

#include <idle/core/platform.hpp>
#include <idle/core/util/terminal.hpp>

#ifdef IDLE_PLATFORM_WINDOWS
#  define DEFINE_CONSOLEV2_PROPERTIES
#  include <Windows.h>
#endif // IDLE_PLATFORM_WINDOWS

namespace idle {
bool enableTerminalColorSupport() {
#if defined(IDLE_PLATFORM_WINDOWS) &&                                          \
    defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
  // Set output mode to handle virtual terminal sequences
  HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
  if (out == INVALID_HANDLE_VALUE) {
    return false;
  }

  DWORD mode = 0;
  if (!GetConsoleMode(out, &mode)) {
    return false;
  }

  return SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
  return true;
#endif // IDLE_PLATFORM_WINDOWS
}
} // namespace idle
