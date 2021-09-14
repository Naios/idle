
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

#ifndef IDLE_CORE_DETAIL_FORMATTER_HPP_INCLUDED
#define IDLE_CORE_DETAIL_FORMATTER_HPP_INCLUDED

#include <fmt/format.h>
#include <idle/core/dep/optional.hpp>
#include <idle/core/ref.hpp>

namespace fmt {
/*template <>
struct formatter<idle::StringView> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(idle::StringView const& opt, FormatContext& ctx) {
    return formatter<StringView>::format(StringView{opt.data(), opt.size()},
                                          ctx);
  }
};*/
template <typename T>
struct formatter<idle::optional<T>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(idle::optional<T> const& opt, FormatContext& ctx) {
    if (opt) {
      return format_to(ctx.begin(), "{}", *opt);
    } else {
      return format_to(ctx.begin(), "<none>");
    }
  }
};
} // namespace fmt

#endif // IDLE_CORE_DETAIL_FORMATTER_HPP_INCLUDED
