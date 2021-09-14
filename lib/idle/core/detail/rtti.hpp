
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

#ifndef IDLE_CORE_DETAIL_RTTI_HPP_INCLUDED
#define IDLE_CORE_DETAIL_RTTI_HPP_INCLUDED

#include <ostream>
#include <string>
#include <boost/type_index.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
namespace detail {
inline bool print_first_not_of(std::ostream& os, StringView name,
                               StringView prefix) {
  if (name.starts_with(prefix)) {
    os << name.substr(prefix.size());
    return true;
  } else {
    return false;
  }
}

template <typename T>
std::ostream& print_class_type(std::ostream& os, T& obj) {
#ifdef BOOST_NO_RTTI
  return os; // Don't print any name on disabled RTTI
#else        // BOOST_NO_RTTI

  static constexpr StringView class_prefix("class ");
  static constexpr StringView struct_prefix("struct ");

  std::string const str = boost::typeindex::type_id_runtime(obj).pretty_name();
  if (print_first_not_of(os, str, class_prefix)) {
    return os;
  } else if (print_first_not_of(os, str, struct_prefix)) {
    return os;
  } else {
    return os << str;
  }
#endif       // BOOST_NO_RTTI
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_RTTI_HPP_INCLUDED
