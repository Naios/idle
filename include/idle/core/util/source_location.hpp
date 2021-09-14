
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

#ifndef IDLE_CORE_UTIL_SOURCE_LOCATION_HPP_INCLUDED
#define IDLE_CORE_UTIL_SOURCE_LOCATION_HPP_INCLUDED

#include <cstddef>
#include <iosfwd>
#include <idle/core/api.hpp>
#include <idle/core/detail/macros.hpp>
#include <idle/core/detail/symbol.hpp>
#include <idle/core/util/printable.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
class IDLE_API(idle) SourceLocation {
public:
  constexpr SourceLocation() noexcept {}

  template <std::size_t FileNameLength, std::size_t FunctionNameLength>
  constexpr SourceLocation(
      char const (&file_name)[FileNameLength], std::uint_least32_t line,
      char const (&pretty_function_name)[FunctionNameLength])
    : file_path_(StringView(file_name, FileNameLength - 1))
    , line_(line)
    , pretty_function_(StringView(pretty_function_name, //
                                  FunctionNameLength - 1))
    , undecorated_(
          detail::symbol::undecorate_pretty_function_name(pretty_function_)) {}

  constexpr bool empty() const noexcept {
    return line_ == 0U;
  }
  constexpr explicit operator bool() const noexcept {
    return !empty();
  }

  constexpr StringView file_path() const noexcept {
    return file_path_;
  }
  constexpr StringView file_name() const noexcept {
    return detail::symbol::filename(file_path_);
  }
  constexpr std::uint_least32_t line() const noexcept {
    return line_;
  }
  constexpr StringView pretty_function() const noexcept {
    return pretty_function_;
  }
  constexpr StringView namespace_class_function_name() const noexcept {
    return undecorated_;
  }
  constexpr StringView function_name() const noexcept {
    return detail::symbol::current_namespace(undecorated_);
  }
  constexpr StringView namespace_class_name() const noexcept {
    return detail::symbol::pop_namespace(undecorated_);
  }
  constexpr StringView class_name() const noexcept {
    return detail::symbol::current_namespace(namespace_class_name());
  }
  constexpr StringView namespace_name() const noexcept {
    return detail::symbol::pop_namespace(namespace_class_name());
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 SourceLocation const& sl);

  auto details() const {
    return printable([this](std::ostream& os) {
      printDetails(os);
    });
  }

private:
  void printDetails(std::ostream& os) const;

  StringView file_path_;
  std::uint_least32_t line_{0};
  StringView pretty_function_;
  StringView undecorated_;
};
} // namespace idle

#define IDLE_CURRENT_SOURCE_LOCATION()                                         \
  ::idle::SourceLocation {                                                     \
    IDLE_DETAIL_FILE_NAME, IDLE_DETAIL_FILE_LINE,                              \
        IDLE_DETAIL_PRETTY_FUNCTION_NAME                                       \
  }

#endif // IDLE_CORE_UTIL_SOURCE_LOCATION_HPP_INCLUDED
