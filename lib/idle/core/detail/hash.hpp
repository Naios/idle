
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

#ifndef IDLE_CORE_DETAIL_HASH_HPP_INCLUDED
#define IDLE_CORE_DETAIL_HASH_HPP_INCLUDED

#include <cstdint>
#include <functional>
#include <utility>

namespace idle {
namespace detail {
// Adapted from:
// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
inline void hash_combine(std::size_t& /*seed*/) {}
template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, T&& v, Rest&&... rest) {
  std::hash<typename std::decay<T>::type> hasher;
  seed ^= hasher(std::forward<T>(v)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  hash_combine(seed, std::forward<Rest>(rest)...);
}

/// Returns a combined hash for all the given arguments
template <typename... T>
std::size_t hash_arguments(T&&... args) {
  std::size_t seed = 0;
  hash_combine(seed, std::forward<T>(args)...);
  return seed;
}

/// Returns a combined hash for all the given arguments
template <typename Iterator>
void hash_range_combine(std::size_t& seed, Iterator begin, Iterator end) {
  for (auto itr = begin; itr != end; ++itr) {
    hash_combine(seed, *itr);
  }
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_HASH_HPP_INCLUDED
