
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

#include <idle/core/detail/streams.hpp>

namespace idle {
namespace detail {
namespace transforms {
std::streambuf::int_type
alnum_fn::operator()(char value, std::streambuf& stream) const noexcept {
  if (std::isalnum(value)) {
    return stream.sputc(value);
  } else {
    return stream.sputc(' ');
  }
}

std::streambuf::int_type
indentation_fn::operator()(char value, std::streambuf& stream) noexcept {
  if (value != '\n') {
    if (after_newline) {
      after_newline = false;

      if (stream.sputn(" ", indentation) == EOF) {
        return EOF;
      }
    }

    return stream.sputc(value);
  } else {
    after_newline = true;
    return stream.sputc('\n');
  }
}

std::streambuf::int_type
guideline_fn::operator()(char value, std::streambuf& stream) noexcept {

  if (value == '\n') {
    current = 0;
  } else if (current < guideline) {
    ++current;
  } else {
    if (break_only_after_spaces) {
      if (value == ' ') {
        current = 0;
        return stream.sputc('\n');
      } else {
        ++current;
      }
    } else {
      current = 0;

      if (stream.sputc('\n') == EOF) {
        return EOF;
      }
    }
  }

  return stream.sputc(value);
}

std::streambuf::int_type
comma_break_fn::operator()(char value, std::streambuf& stream) noexcept {

  if (after_comma_ && (value == ' ') && (cur_width >= max_width)) {
    after_comma_ = false;
    cur_width = 0;
    return stream.sputc('\n');
  }

  after_comma_ = (value == ',');
  ++cur_width;
  return stream.sputc(value);
}
} // namespace transforms
} // namespace detail
} // namespace idle
