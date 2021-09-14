
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

#ifndef IDLE_CORE_UTIL_ENUM_HPP_INCLUDED
#define IDLE_CORE_UTIL_ENUM_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
#include <idle/core/util/array.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
template <typename T>
class EnumEntry {
public:
  constexpr EnumEntry() noexcept {}
  constexpr EnumEntry(T value, std::size_t offset, std::size_t length) noexcept
    : value_(static_cast<std::underlying_type_t<T>>(value))
    , offset_(offset)
    , length_(length) {}

  constexpr T value() const noexcept {
    return static_cast<T>(value_);
  }
  constexpr auto repr() const noexcept {
    return value_;
  }

  constexpr std::size_t offset() const noexcept {
    return offset_;
  }

  constexpr std::size_t length() const noexcept {
    return length_;
  }

  constexpr void const* ptr() const noexcept {
    return &value_;
  }

private:
  std::underlying_type_t<T> value_{};
  std::size_t offset_{};
  std::size_t length_{};
};

template <typename T, typename Trait, std::size_t Size, std::size_t Characters,
          bool IsDense>
class Enum {
  using Entries = Array<EnumEntry<T>, Size>;
  using Names = Array<char, Characters>;

public:
  constexpr Enum(Entries entries, Names names) noexcept
    : entries_(entries)
    , names_(names) {}

  using trait = Trait;
  using type = T;
  using underlying = std::underlying_type_t<T>;

  /// Specifies the entry count the enum contains
  static constexpr std::size_t size = Size;

  /// Specifies whether the enum is dense (index i represents value i)
  static constexpr std::size_t is_dense = IsDense;

  constexpr Entries const& entries() const noexcept {
    return entries_;
  }
  constexpr Names const& names() const noexcept {
    return names_;
  }

  constexpr StringView name(EnumEntry<T> const& entry) const noexcept {
    return StringView(names_.data() + entry.offset(), entry.length());
  }
  constexpr StringView name(std::size_t index) const noexcept {
    return name(*(*this)[index]);
  }

  constexpr StringView lines() const noexcept {
    return {names_.data(), size - 1};
  }

  constexpr T value(std::size_t index) const noexcept {
    return (*this)[index]->value();
  }

  constexpr EnumEntry<T> const* operator[](std::size_t index) const noexcept {
    return entries_.data() + index;
  }

private:
  Entries entries_;
  Names names_;
};
} // namespace idle

#endif // IDLE_CORE_UTIL_ENUM_HPP_INCLUDED
