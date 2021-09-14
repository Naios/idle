
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

#ifndef IDLE_CORE_DETAIL_ALGORITHM_HPP_INCLUDED
#define IDLE_CORE_DETAIL_ALGORITHM_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
#include <utility>

// Light algorithm header so we don't have to pull <algorithm> in

namespace idle {
namespace detail {
template <typename Iterator>
constexpr std::size_t distance(Iterator begin, Iterator end) noexcept {
  static_assert(!std::is_pointer<Iterator>::value,
                "distance can be implemented faster for raw pointers!");
  std::size_t count = 0U;
  while (begin != end) {
    ++begin;
    ++count;
  }
  return count;
}

template <typename Iterator, typename Predicate>
constexpr void sort(Iterator begin, Iterator end, Predicate&& pred) {

  // Adapted from https://stackoverflow.com/a/55205857/2303378

  std::size_t const size = end - begin;
  std::size_t gap = size;

  bool swapped = false;

  while ((gap > std::size_t{1}) || swapped) {
    if (gap > std::size_t{1}) {
      gap = static_cast<std::size_t>(gap / 1.247330950103979);
    }

    swapped = false;

    for (std::size_t i = std::size_t{0}; gap + i < size; ++i) {
      auto& left = *(begin + i);
      auto& right = *(begin + i + gap);

      if (!pred(left, right)) {
        auto swap = std::move(left);
        left = std::move(right);
        right = std::move(swap);
        swapped = true;
      }
    }
  }
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_ALGORITHM_HPP_INCLUDED
