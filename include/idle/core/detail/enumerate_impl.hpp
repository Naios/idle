
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

#ifndef IDLE_CORE_DETAIL_ENUMERATE_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_ENUMERATE_IMPL_HPP_INCLUDED

#include <cstdint>
#include <type_traits>
#include <idle/core/detail/macros.hpp>
#include <idle/core/detail/symbol.hpp>
#include <idle/core/platform.hpp>
#include <idle/core/util/array.hpp>
#include <idle/core/util/enum.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
namespace detail {
template <typename T, T Value>
constexpr StringView e() noexcept {
  constexpr StringView str(IDLE_DETAIL_PRETTY_FUNCTION_NAME);

#ifdef IDLE_COMPILER_MSVC
  // MSVC produces something like:
  // clang-format off
  // class idle::StringView __cdecl idle::detail::p<enum idle::myenum,idle::myenum::Two>(void) noexcept
  // class idle::StringView __cdecl idle::detail::p<enum idle::myenum,(enum idle::myenum)0x64>(void) noexcept
  // clang-format on
  std::size_t const begin = str.find(',', 22) + 1;

  if (str[begin] == '(') {
    return {};
  } else {
    return symbol::current_namespace(
        str.substr(begin, str.size() - 16 - begin));
  }
#else
  // Clang produces something like:
  // clang-format off
  // idle::StringView idle::detail::e() [T = idle::myenum, Value = idle::myenum::Two]
  // idle::StringView idle::detail::e() [T = idle::myenum, Value = 100]
  // clang-format on
  std::size_t const begin = str.find("Value = ") + 8;

  if ((str[begin] >= '0') && (str[begin] <= '9')) {
    return {};
  } else {
    return symbol::current_namespace(str.substr(begin, str.size() - 1 - begin));
  }
#endif
}

struct enum_info {
  std::size_t size{0};
  std::size_t chars{1};
  bool is_continuous{true};
};

constexpr enum_info enum_info_of(StringView const* data, std::size_t size,
                                 std::ptrdiff_t min) noexcept {
  auto const ssize = static_cast<std::ptrdiff_t>(size);

  enum_info count;
  bool interleaved = false;

  for (std::ptrdiff_t i = 0; i < ssize; ++i) {
    if (data[i]) {
      count.size += 1;
      count.chars += data[i].size() + 1;

      if (count.is_continuous && interleaved) {
        count.is_continuous = false;
      }
    } else if (count.is_continuous && !interleaved) {
      if ((i + min) >= 0) {
        interleaved = true;
      }
    }
  }
  return count;
}

template <typename T, typename Trait, std::size_t... I>
constexpr auto expand_linear_impl(std::index_sequence<I...>) noexcept {
  using underlying_type = std::underlying_type_t<T>;

  constexpr Trait trait;
  constexpr StringView seq[] = {
      (e<T, static_cast<T>(trait.value(static_cast<underlying_type>(I) +
                                       Trait::min))>())...};
  constexpr enum_info info = enum_info_of(seq, sizeof...(I),
                                          static_cast<std::ptrdiff_t>(
                                              Trait::min));

  Array<char, info.chars> chars;
  Array<EnumEntry<T>, info.size> entries;

  for (std::size_t i = 0, offset = 0, pos = 0; i < sizeof...(I); ++i) {
    if (StringView current = seq[i]) {
      std::size_t const size = current.size();

      entries[pos] = EnumEntry<T>{static_cast<T>(trait.value(
                                      static_cast<underlying_type>(i) +
                                      Trait::min)),
                                  offset, size};

      for (std::size_t cur = 0; cur != size; ++cur) {
        chars[offset + cur] = trait.transform(current[cur]);
      }

      chars[offset + size] = '\n';

      offset += size + 1;
      ++pos;
    }
  }

  chars[info.chars - 1] = '\0';

  return Enum<T, Trait, info.size, info.chars, //
              (info.is_continuous && Trait::min == 0)>{entries, chars};
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_ENUMERATE_IMPL_HPP_INCLUDED
