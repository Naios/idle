
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

#ifndef IDLE_CORE_UTIL_ENUMERATE_HPP_INCLUDED
#define IDLE_CORE_UTIL_ENUMERATE_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
#include <idle/core/detail/enumerate_impl.hpp>

namespace idle {
/// Feel free to specialize your own min and max enum size for adapting the
/// enum boundary properly to compile times for a specific enum.
///
/// It is perfectly fine not to specify the real min and max of the enum,
/// however values below or above the range will not be reflected.
///
/// If you choose a range which is too high or low then the compile
/// time will suffer from the \see enumerate.
template <typename T, std::underlying_type_t<T> Min = 0,
          std::underlying_type_t<T> Max = 64>
struct enum_trait {
  /// Specifies the minimum enum value search range
  static constexpr std::underlying_type_t<T> min = Min;

  /// Specifies the maximum enum value search range
  static constexpr std::underlying_type_t<T> max = Max;

  /// Specifies a mapping function for mapping the index to a specific value
  constexpr std::underlying_type_t<T>
  value(std::underlying_type_t<T> i) const noexcept {
    return i;
  }

  /// Specifies a mapping function for mapping the source code name
  /// to a represented enum constant.
  constexpr char transform(char c) const noexcept {
    return c;
  }
};
/// Transform enum names to uppercase
template <typename T, std::underlying_type_t<T> Min = 0,
          std::underlying_type_t<T> Max = 64>
struct enum_uppercase : enum_trait<T, Min, Max> {
  constexpr char transform(char c) const noexcept {
    if ('a' <= c && c <= 'z') {
      return c - ('a' - 'A');
    } else {
      return c;
    }
  }
};
/// Transform enum names to lowercase
template <typename T, std::underlying_type_t<T> Min = 0,
          std::underlying_type_t<T> Max = 64>
struct enum_lowercase : enum_trait<T, Min, Max> {
  constexpr char transform(char c) const noexcept {
    if ('A' <= c && c <= 'Z') {
      return c + ('a' - 'A');
    } else {
      return c;
    }
  }
};

template <typename T>
struct enum_mask {
  static constexpr std::underlying_type_t<T> min = 0;

  static constexpr std::underlying_type_t<T>
      max = sizeof(std::underlying_type_t<T>) * 8;

  constexpr std::underlying_type_t<T>
  value(std::underlying_type_t<T> i) const noexcept {
    return std::underlying_type_t<T>(1) << i;
  }

  constexpr char transform(char c) const noexcept {
    return c;
  }
};

/// Returns a reflected representation of the given enum
template <typename T, typename Trait = enum_trait<T>>
constexpr auto enumerate() noexcept {
  static_assert(std::is_enum<T>::value, "T must be an enum!");

  constexpr auto enumeration = detail::expand_linear_impl<T, Trait>(
      std::make_index_sequence<Trait::max - Trait::min>{});

  return enumeration;
}
} // namespace idle

#endif // IDLE_CORE_UTIL_ENUMERATE_HPP_INCLUDED
