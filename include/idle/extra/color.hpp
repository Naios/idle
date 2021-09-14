
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

#ifndef IDLE_EXTRA_COLOR_HPP_INCLUDED
#define IDLE_EXTRA_COLOR_HPP_INCLUDED

#include <cstdint>
#include <iosfwd>
#include <idle/core/api.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
/// Encodes a compressed rgba color
class IDLE_API(idle) Color {
  static constexpr std::uint8_t read_hex(char value) noexcept {
    IDLE_ASSERT('0' <= value);
    IDLE_ASSERT('f' >= value);

    if (value <= '9') {
      return value - '0';
    } else {
      return std::uint8_t((value % 'a') % 'A') + 10;
    }
  }
  static constexpr std::uint8_t read_color(char const* data) noexcept {
    return read_hex(data[0]) << 4 | read_hex(data[1]);
  }

public:
  static constexpr std::uint8_t offset_r = 0;
  static constexpr std::uint8_t offset_g = 8;
  static constexpr std::uint8_t offset_b = 16;
  static constexpr std::uint8_t offset_a = 24;

  constexpr Color() noexcept
    : Color(std::uint8_t(0), std::uint8_t(0), std::uint8_t(0)) {}
  constexpr Color(std::uint32_t rgba) noexcept
    : Color(std::uint8_t((rgba >> offset_r) & 0xFF),
            std::uint8_t((rgba >> offset_g) & 0xFF),
            std::uint8_t((rgba >> offset_b) & 0xFF),
            std::uint8_t((rgba >> offset_a) & 0xFF)) {}
  constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                  std::uint8_t a = 255) noexcept
    : r_(r)
    , g_(g)
    , b_(b)
    , a_(a) {}
  constexpr Color(char const (&data)[7]) noexcept
    : Color(std::uint8_t(read_color(data + 0)),
            std::uint8_t(read_color(data + 2)),
            std::uint8_t(read_color(data + 4))) {
    IDLE_ASSERT(data[6] == '\0');
  }
  constexpr Color(char const (&data)[8]) noexcept
    : Color(std::uint8_t(read_color(data + 1)),
            std::uint8_t(read_color(data + 3)),
            std::uint8_t(read_color(data + 5))) {
    IDLE_ASSERT(data[0] == '#');
    IDLE_ASSERT(data[7] == '\0');
  }
  constexpr Color(char const (&data)[9]) noexcept
    : Color(std::uint8_t(read_color(data + 0)),
            std::uint8_t(read_color(data + 2)),
            std::uint8_t(read_color(data + 4)),
            std::uint8_t(read_color(data + 6))) {
    IDLE_ASSERT(data[8] == '\0');
  }
  constexpr Color(char const (&data)[10]) noexcept
    : Color(std::uint8_t(read_color(data + 1)),
            std::uint8_t(read_color(data + 3)),
            std::uint8_t(read_color(data + 5)),
            std::uint8_t(read_color(data + 7))) {
    IDLE_ASSERT(data[0] == '#');
    IDLE_ASSERT(data[9] == '\0');
  }

  constexpr Color(float r, float g, float b, float a = 1.f)
    : Color(std::uint8_t(255 * r), std::uint8_t(255 * g), std::uint8_t(255 * b),
            std::uint8_t(255 * a)) {}

  constexpr Color operator*(float transparency) const noexcept {
    std::int32_t const target = std::int32_t(double(transparency) * a_);
    return {r_, g_, b_,
            target > 255
                ? std::uint8_t(255)
                : (target < 0 ? std::uint8_t(0) : std::uint8_t(target))};
  }
  constexpr Color operator/(float transparency) const noexcept {
    return (*this) * (1.f / transparency);
  }

  constexpr std::uint8_t r() const noexcept {
    return r_;
  }
  constexpr std::uint8_t g() const noexcept {
    return g_;
  }
  constexpr std::uint8_t b() const noexcept {
    return b_;
  }
  constexpr std::uint8_t a() const noexcept {
    return a_;
  }

  /// Returns the rgba components packed as a uint32
  ///
  /// \note This can be used in Dear ImGui for example `ImColor(color.packed()`)
  constexpr std::uint32_t packed() const noexcept {
    return std::uint32_t(a_) << offset_a | std::uint32_t(b_) << offset_b |
           std::uint32_t(g_) << offset_g | std::uint32_t(r_) << offset_r;
  }

  constexpr std::uint32_t operator*() const noexcept {
    return packed();
  }

  constexpr bool operator==(Color right) const noexcept {
    return r_ == right.r_ && g_ == right.g_ && b_ == right.b_ && a_ == right.a_;
  }
  constexpr bool operator!=(Color right) const noexcept {
    return !(*this == right);
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os, Color color);

  static constexpr Color black() noexcept {
    return {std::uint8_t(0), 0, 0};
  }
  static constexpr Color white() noexcept {
    return {std::uint8_t(255), 255, 255};
  }
  static constexpr Color red() noexcept {
    return {std::uint8_t(255), 0, 0};
  }
  static constexpr Color green() noexcept {
    return {std::uint8_t(0), 255, 0};
  }
  static constexpr Color blue() noexcept {
    return {std::uint8_t(0), 0, 255};
  }

private:
  std::uint8_t r_;
  std::uint8_t g_;
  std::uint8_t b_;
  std::uint8_t a_;
};
} // namespace idle

#endif // IDLE_EXTRA_COLOR_HPP_INCLUDED
