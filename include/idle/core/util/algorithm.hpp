
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

#ifndef IDLE_CORE_UTIL_ALGORITHM_HPP_INCLUDED
#define IDLE_CORE_UTIL_ALGORITHM_HPP_INCLUDED

#include <algorithm>
#include <utility>

namespace idle {
/// Returns true if the value matches the given predicate
template <typename Iterator, typename End, typename Comparable,
          typename Predicate>
constexpr bool is_equal_element(Iterator itr, End end, Comparable const& value,
                                Predicate&& predicate) {
  return (itr != end) && !(predicate)(value, *itr);
}

template <typename Begin, typename End, typename Comparable, typename Predicate>
constexpr auto lower_bound(Begin begin, End end, Comparable const& value,
                           Predicate&& predicate) {

  std::ptrdiff_t count = end - begin;

  while (count > 0) {
    std::ptrdiff_t const step = count / 2;
    Begin const itr = begin + step;

    if (predicate(*itr, value)) {
      begin = itr + 1;
      count -= step + 1;
    } else {
      count = step;
    }
  }
  return begin;
}

/// Finds the first value matching the given predicate
template <typename Begin, typename End, typename Comparable, typename Predicate>
constexpr auto find_first_lower_bound_of(Begin begin, End end,
                                         Comparable const& value,
                                         Predicate&& predicate) {
  auto const lower = lower_bound(begin, end, value, predicate);
  if (is_equal_element(lower, end, value, std::forward<Predicate>(predicate))) {
    return lower;
  } else {
    return end;
  }
}
} // namespace idle

#endif // IDLE_CORE_UTIL_ALGORITHM_HPP_INCLUDED
