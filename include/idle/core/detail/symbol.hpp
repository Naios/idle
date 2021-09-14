
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

#ifndef IDLE_CORE_DETAIL_SYMBOL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_SYMBOL_HPP_INCLUDED

#include <idle/core/platform.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
namespace detail {
namespace symbol {
#ifdef IDLE_COMPILER_MSVC
// Specifies the separator provided by the MSVC __FILE__ macro
static constexpr char path_separator = '\\';
#else
// Specifies the separator provided by the Clang & GCC __FILE__ macro
static constexpr char path_separator = '/';
#endif

constexpr StringView filename(StringView const& s) noexcept {
  StringView::size_type const pos = s.rfind(path_separator);
  if (pos != StringView::npos) {
    return s.substr(pos + 1);
  } else {
    return s;
  }
}

constexpr StringView
undecorate_pretty_function_name(StringView const& s) noexcept {
  StringView::size_type const end = s.find('(');
  if (end == StringView::npos) {
    return s;
  }
  StringView::size_type const begin = s.rfind(' ', end);
  if (begin == StringView::npos) {
    return s.substr(0, end);
  } else {
    return s.substr(begin + 1, end - begin - 1);
  }
}

constexpr StringView current_namespace(StringView const& s) noexcept {
  StringView::size_type const begin = s.rfind(':');
  if ((begin == StringView::npos) || (begin + 1 >= s.size())) {
    return s;
  } else {
    return s.substr(begin + 1);
  }
}

constexpr StringView pop_namespace(StringView const& s) noexcept {
  StringView::size_type const begin = s.rfind(':');
  if ((begin == StringView::npos) || (begin == 0)) {
    return {};
  } else {
    return s.substr(0, begin - 1);
  }
}
} // namespace symbol
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_SYMBOL_HPP_INCLUDED
