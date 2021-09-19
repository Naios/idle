
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

#ifndef IDLE_CORE_DETAIL_UNORDERED_MAP_HPP_INCLUDED
#define IDLE_CORE_DETAIL_UNORDERED_MAP_HPP_INCLUDED

#include <memory>
#include <unordered_map>
#include <utility>

namespace idle {
namespace detail {
/// An unordered map with no enforced pointer stability
template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<Key const, Value>>>
using unordered_map = std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>;

/// An unordered map with guaranteed pointer stability
template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<Key const, Value>>>
using unordered_node_map = std::unordered_map<Key, Value, Hash, KeyEqual,
                                              Allocator>;
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_UNORDERED_MAP_HPP_INCLUDED
