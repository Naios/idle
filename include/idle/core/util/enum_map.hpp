
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

#ifndef IDLE_CORE_UTIL_ENUM_MAP_HPP_INCLUDED
#define IDLE_CORE_UTIL_ENUM_MAP_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <limits>
#include <idle/core/api.hpp>
#include <idle/core/detail/algorithm.hpp>
#include <idle/core/util/algorithm.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/enum.hpp>
#include <idle/core/util/enumerate.hpp>
#include <idle/core/util/meta.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
template <typename T, typename Trait = enum_trait<T>>
using is_enum_dense = std::integral_constant<
    bool, decltype(enumerate<T, Trait>())::is_dense>;

template <typename T, typename Trait = enum_trait<T>>
using is_enum_empty = std::integral_constant<
    bool, (decltype(enumerate<T, Trait>())::size != 0)>;

template <typename Enum>
struct ValueLess {
  constexpr bool operator()(std::size_t left,
                            typename Enum::type value) const noexcept {
    return (*enumeration)[left]->value() < value;
  }
  constexpr bool operator()(typename Enum::type value,
                            std::size_t right) const noexcept {
    return value < (*enumeration)[right]->value();
  }
  constexpr bool operator()(std::size_t left,
                            std::size_t right) const noexcept {
    return (*enumeration)[left]->value() < (*enumeration)[right]->value();
  }

  Enum const* enumeration;
};

/// Returns an array that contains the indexes of the enum with sorted after
/// their value
template <typename Enum>
constexpr auto enum_values(Enum const& enumeration) noexcept {
  Array<std::size_t, Enum::size> buffer;
  for (std::size_t i = 0; i < Enum::size; ++i) {
    buffer[i] = i;
  }

  if IDLE_CXX17_CONSTEXPR (!Enum::is_dense) {
    detail::sort(buffer.begin(), buffer.end(), ValueLess<Enum>{&enumeration});
  }
  return buffer;
}

template <typename Enum>
struct NameLess {
  constexpr bool operator()(std::size_t left,
                            std::size_t right) const noexcept {
    return enumeration->name(left) < enumeration->name(right);
  }
  constexpr bool operator()(std::size_t left, StringView str) const noexcept {
    return enumeration->name(left) < str;
  }
  constexpr bool operator()(StringView str, std::size_t right) const noexcept {
    return str < enumeration->name(right);
  }

  Enum const* enumeration;
};

/// Returns an array that contains the indexes of the enum with sorted after
/// their names
template <typename Enum>
constexpr auto enum_names(Enum const& enumeration) noexcept {
  Array<std::size_t, Enum::size> buffer;
  for (std::size_t i = 0; i < Enum::size; ++i) {
    buffer[i] = i;
  }

  detail::sort(buffer.begin(), buffer.end(), NameLess<Enum>{&enumeration});
  return buffer;
}

/// Returns the size of the given dense enum
template <typename T, typename Trait = enum_trait<T>>
constexpr std::size_t enum_size() noexcept {
  constexpr std::size_t size = enumerate<T, Trait>().size;
  return size;
}

/// Returns the enum name of the given enum value if there is any matching
///
/// Implementation optimized for dense enums
template <typename Enum, std::enable_if_t<Enum::is_dense>* = nullptr>
constexpr StringView enum_name(Enum const& enumeration,
                               typename Enum::type value) noexcept {
  return enumeration.name(static_cast<std::size_t>(value));
}
/// \copybrief enum_name
///
/// Implementation optimized for sparse enums
template <typename Enum, std::enable_if_t<!Enum::is_dense>* = nullptr>
constexpr StringView enum_name(Enum const& enumeration,
                               typename Enum::type value) noexcept {
  constexpr auto values = enum_values(enumerate<typename Enum::type, //
                                                typename Enum::trait>());

  auto const itr = find_first_lower_bound_of(
      values.begin(), values.end(), value,
      ValueLess<unrefcv_t<decltype(enumeration)>>{&enumeration});

  IDLE_ASSERT(itr != values.end());
  return enumeration.name(*itr);
}

class EnumIndex {
  static constexpr auto sentinel = std::numeric_limits<std::size_t>::max();

public:
  constexpr EnumIndex() noexcept {}
  constexpr EnumIndex(std::size_t index) noexcept
    : index_(index) {}

  constexpr explicit operator bool() const noexcept {
    return index_ != sentinel;
  }
  constexpr std::size_t operator*() const noexcept {
    return index_;
  }

private:
  std::size_t index_{sentinel};
};

/// Returns the EnumEntry of the given enum value if there is any matching
template <typename Enum>
constexpr EnumIndex enum_find(Enum const& enumeration,
                              StringView name) noexcept {
  constexpr auto names = enum_names(enumerate<typename Enum::type, //
                                              typename Enum::trait>());

  auto const itr = find_first_lower_bound_of(
      names.begin(), names.end(), name,
      NameLess<unrefcv_t<decltype(enumeration)>>{&enumeration});

  if (itr != names.end()) {
    return EnumIndex(*itr);
  } else {
    return {};
  }
}
} // namespace idle

#endif // IDLE_CORE_UTIL_ENUM_MAP_HPP_INCLUDED
