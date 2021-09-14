
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

#include <array>
#include <ostream>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/core/guid.hpp>
#include <idle/core/part.hpp>
#include <idle/core/service.hpp>

namespace idle {
static_assert(sizeof(Guid) == sizeof(Guid::Low), "Bad guid size!");

Guid Guid::of(Service const& service) noexcept {
  return service.guid();
}

Guid Guid::of(Part const& part) noexcept {
  Guid const owner = part.owner().guid();
  return Guid(owner.low(), owner.high(), part.partOffset());
}

void Guid::hex(std::ostream& os) const {
  static constexpr char const* hex_digits = "0123456789ABCDEF";
  static constexpr std::size_t low_size = sizeof(Low) * 2;

  std::array<char, low_size + 2> buffer;

  buffer[0] = '0';
  buffer[1] = 'x';

  for (std::uint8_t i = 0; i < low_size; ++i) {
    std::uint8_t const current = (value() >> (i * 4)) & 0x0F;
    buffer[low_size - i + 1] = hex_digits[current];
  }

  os.write(buffer.data(), buffer.size());
}

void Guid::details(std::ostream& os) const {
  print(os, FMT_STRING("low: {}, high: {}, ordinal: {}"), low(), high(),
        ordinal());
}

void Guid::msb(std::ostream& os) const {
  if (isCluster()) {
    fmt::print(os, FMT_STRING("{{{:>4}}}"), low());
  } else if (isComponent()) {
    fmt::print(os, FMT_STRING("({:>2})"), high());
  } else {
    IDLE_ASSERT(isPart());
    fmt::print(os, FMT_STRING("{}."), ordinal());
  }
}

std::ostream& operator<<(std::ostream& os, Guid guid) {
  if (guid.isCluster()) {
    fmt::print(os, FMT_STRING("{{{:>4}}}"), guid.low());
  } else if (guid.isComponent()) {
    fmt::print(os, FMT_STRING("{{{:>4}.{}}}"), guid.low(), guid.high());
  } else {
    IDLE_ASSERT(guid.isPart());
    fmt::print(os, FMT_STRING("{{{:>4}.{}/{}}}"), guid.low(), guid.high(),
               guid.ordinal());
  }

  return os;
}
} // namespace idle
