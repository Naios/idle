
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

#ifndef IDLE_CORE_DETAIL_UNORDERED_SET_HPP_INCLUDED
#define IDLE_CORE_DETAIL_UNORDERED_SET_HPP_INCLUDED

#include <memory>
#include <unordered_set>
#include <utility>

namespace idle {
namespace detail {
/// An unordered set with no enforced pointer stability
template <typename Key, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<Key>>
using unordered_set = std::unordered_set<Key, Hash, KeyEqual, Allocator>;

/// An unordered set with guaranteed pointer stability
template <typename Key, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<Key>>
using unordered_node_set = std::unordered_set<Key, Hash, KeyEqual, Allocator>;
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_UNORDERED_SET_HPP_INCLUDED
