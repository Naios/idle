
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

#ifndef IDLE_CORE_GUID_HPP_INCLUDED
#define IDLE_CORE_GUID_HPP_INCLUDED

#include <cstddef>
#include <idle/core/api.hpp>
#include <idle/core/util/printable.hpp>

namespace idle {
class Part;
class Service;

/// Uniquely identifies a particular service in the system.
///
/// - The low guid part is used to identify the cluster.
/// - The high guid part identifies a service inside a particular cluster,
///   the cluster head will always have the high guid part 0!
/// - The ordinal guid part can be used to address a part of a service
///   through its ordinal ordinal.
///
/// \attention The guid is not unique across different
///            \see context objects!
class IDLE_API(idle) Guid {
  static constexpr std::uint8_t bits_low = 40;
  static constexpr std::uint8_t bits_high = 16;
  static constexpr std::uint8_t bits_ordinal = 8;

public:
  using Value = std::uint64_t;
  using Low = Value;
  using High = std::uint16_t;
  using Offset = std::uint8_t;

  static constexpr Low max_low() noexcept {
    return (Low(1) << bits_low) - 1U;
  }
  static constexpr Low min_low() noexcept {
    return 0U;
  }
  static constexpr High max_high() noexcept {
    return (Low(1) << bits_high) - 1U;
  }
  static constexpr High min_high() noexcept {
    return 0U;
  }
  static constexpr Offset max_ordinal() noexcept {
    return (Low(1) << bits_ordinal) - 1U;
  }
  static constexpr Offset min_ordinal() noexcept {
    return 0U;
  }

  constexpr Guid() noexcept
    : low_(max_low())
    , high_(max_high())
    , ordinal_(max_ordinal()) {}

  explicit constexpr Guid(Low low, High high = min_high(),
                          Offset ordinal = min_ordinal()) noexcept
    : low_(low)
    , high_(high)
    , ordinal_(ordinal) {}

  Guid(Guid const&) = default;
  Guid(Guid&&) = default;
  Guid& operator=(Guid const&) = default;
  Guid& operator=(Guid&&) = default;

  static Guid fromValue(Value value) noexcept {
    return Guid(value & max_low(), (value >> bits_low) & max_high(),
                (value >> (bits_high + bits_low)) & max_ordinal());
  }

  constexpr bool operator==(Guid const& other) const noexcept {
    return std::make_tuple(low(), high(), ordinal()) ==
           std::make_tuple(other.low(), other.high(), other.ordinal());
  }
  constexpr bool operator!=(Guid const& other) const noexcept {
    return std::make_tuple(low(), high(), ordinal()) !=
           std::make_tuple(other.low(), other.high(), other.ordinal());
  }
  constexpr bool operator<(Guid const& other) const noexcept {
    return std::make_tuple(low(), high(), ordinal()) <
           std::make_tuple(other.low(), other.high(), other.ordinal());
  }
  constexpr bool operator<=(Guid const& other) const noexcept {
    return std::make_tuple(low(), high(), ordinal()) <=
           std::make_tuple(other.low(), other.high(), other.ordinal());
  }
  constexpr bool operator>(Guid const& other) const noexcept {
    return std::make_tuple(low(), high(), ordinal()) >
           std::make_tuple(other.low(), other.high(), other.ordinal());
  }
  constexpr bool operator>=(Guid const& other) const noexcept {
    return std::make_tuple(low(), high(), ordinal()) >=
           std::make_tuple(other.low(), other.high(), other.ordinal());
  }

  constexpr Low low() const noexcept {
    return Low(low_);
  }
  constexpr High high() const noexcept {
    return high_;
  }
  constexpr Offset ordinal() const noexcept {
    return ordinal_;
  }
  constexpr Value value() const noexcept {
    return (Low(ordinal_) << (bits_high + bits_low)) |
           (Low(high_) << bits_low) | low_;
  }

  constexpr bool empty() const noexcept {
    return low() == max_low();
  }
  constexpr explicit operator bool() const noexcept {
    return !empty();
  }

  constexpr bool isCluster() const noexcept {
    return low_ != max_low() && !high_ && !ordinal_;
  }
  constexpr bool isComponent() const noexcept {
    return high_ && !ordinal_;
  }
  constexpr bool isPart() const noexcept {
    return ordinal_;
  }

  constexpr Guid cluster() const noexcept {
    return Guid(low());
  }
  constexpr Guid owner() const noexcept {
    return Guid(low(), high());
  }

  static Guid of(Service const& service) noexcept;
  static Guid of(Part const& part) noexcept;

  void hex(std::ostream& os) const;
  auto hex() const {
    return printable([this](std::ostream& os) {
      hex(os);
    });
  }

  /// Prints a numerical detailed output
  void details(std::ostream& os) const;
  auto details() const {
    return printable([this](std::ostream& os) {
      details(os);
    });
  }

  /// Prints the most significant bits
  void msb(std::ostream& os) const;
  auto msb() const {
    return printable([this](std::ostream& os) {
      msb(os);
    });
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os, Guid guid);

private:
  Low low_ : bits_low;
  Low high_ : bits_high;
  Low ordinal_ : bits_ordinal;
};
} // namespace idle

#endif // IDLE_CORE_GUID_HPP_INCLUDED
