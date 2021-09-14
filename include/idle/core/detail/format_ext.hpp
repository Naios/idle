
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

#ifndef IDLE_CORE_DETAIL_FORMAT_EXT_HPP_INCLUDED
#define IDLE_CORE_DETAIL_FORMAT_EXT_HPP_INCLUDED

#include <fmt/color.h>
#include <fmt/format.h>

FMT_BEGIN_NAMESPACE

template <typename S, typename... Args, size_t SIZE = inline_buffer_size,
          typename Char = enable_if_t<detail::is_string<S>::value, char_t<S>>>
inline typename buffer_context<Char>::iterator
format_to(basic_memory_buffer<Char, SIZE>& buf, const text_style& ts,
          const S& format_str, Args&&... args) {
  detail::vformat_to(buf, ts, to_string_view(format_str),
                     fmt::make_args_checked<Args...>(format_str, args...));
  return detail::buffer_appender<Char>(buf);
}

FMT_END_NAMESPACE

#endif // IDLE_CORE_DETAIL_FORMAT_EXT_HPP_INCLUDED
