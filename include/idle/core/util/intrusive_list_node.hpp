
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

#ifndef IDLE_CORE_UTIL_INTRUSIVE_LIST_NODE_HPP_INCLUDED
#define IDLE_CORE_UTIL_INTRUSIVE_LIST_NODE_HPP_INCLUDED

#include <type_traits>
#include <idle/core/util/assert.hpp>

namespace idle {
template <typename T, typename Tag>
class intrusive_forward_list_node {
  template <typename, typename>
  friend class intrusive_list_node;
  template <typename, typename, typename>
  friend struct intrusive_list_trait;

public:
  intrusive_forward_list_node() = default;
  intrusive_forward_list_node(intrusive_forward_list_node&&) = delete;
  intrusive_forward_list_node(intrusive_forward_list_node const&) = delete;
  intrusive_forward_list_node&
  operator=(intrusive_forward_list_node&&) = delete;
  intrusive_forward_list_node&
  operator=(intrusive_forward_list_node const&) = delete;

private:
  T* next_{nullptr};
};

template <typename T, typename Tag>
class intrusive_list_node : public intrusive_forward_list_node<T, Tag> {
  template <typename, typename, typename>
  friend struct intrusive_list_trait;

public:
  intrusive_list_node() = default;

#ifndef NDEBUG
  ~intrusive_list_node() noexcept {
    IDLE_ASSERT(this->next_ == nullptr);
    IDLE_ASSERT(previous_ == nullptr);
  }
#endif // NDEBUG

private:
  T* previous_{nullptr};
};

template <typename T, typename Tag, typename Node>
struct intrusive_list_trait {
  static_assert(always_false<T>::value,
                "intrusive_list_trait is not specialized for this node!");
};
template <typename T, typename Tag>
struct intrusive_list_trait<T, Tag, intrusive_forward_list_node<T, Tag>> {
  static constexpr void set_next(intrusive_forward_list_node<T, Tag>& node,
                                 T* next) noexcept {
    node.next_ = next;
  }
  static constexpr T*
  get_next(intrusive_forward_list_node<T, Tag> const& node) noexcept {
    return node.next_;
  }
};
template <typename T, typename Tag>
struct intrusive_list_trait<T, Tag, intrusive_list_node<T, Tag>> {
  static constexpr void set_next(intrusive_list_node<T, Tag>& node,
                                 T* next) noexcept {
    node.next_ = next;
  }
  static constexpr T*
  get_next(intrusive_list_node<T, Tag> const& node) noexcept {
    return node.next_;
  }
  static constexpr void set_previous(intrusive_list_node<T, Tag>& node,
                                     T* previous) noexcept {
    node.previous_ = previous;
  }
  static constexpr T*
  get_previous(intrusive_list_node<T, Tag> const& node) noexcept {
    return node.previous_;
  }
};
} // namespace idle

#endif // IDLE_CORE_UTIL_INTRUSIVE_LIST_NODE_HPP_INCLUDED
