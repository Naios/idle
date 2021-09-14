
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

#include <ostream>
#include <idle/extra/color.hpp>

namespace idle {
std::ostream& operator<<(std::ostream& os, Color color) {
  static constexpr char const* hex_digits = "0123456789ABCDEF";

  os.put('#');

  os.put(hex_digits[color.r() >> 4]);
  os.put(hex_digits[color.r() & 0x0F]);

  os.put(hex_digits[color.g() >> 4]);
  os.put(hex_digits[color.g() & 0x0F]);

  os.put(hex_digits[color.b() >> 4]);
  os.put(hex_digits[color.b() & 0x0F]);

  if (color.a() != 255) {
    os.put(hex_digits[color.a() >> 4]);
    os.put(hex_digits[color.a() & 0x0F]);
  }

  return os;
}

static constexpr Color color_test = "#b9Aee2D8";
static_assert(color_test.r() == 185, "r");
static_assert(color_test.g() == 174, "g");
static_assert(color_test.b() == 226, "b");
static_assert(color_test.a() == 216, "a");
} // namespace idle
