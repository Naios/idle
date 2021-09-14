
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

#ifndef IDLE_CORE_DETAIL_VIEW_BASE_HPP_INCLUDED
#define IDLE_CORE_DETAIL_VIEW_BASE_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
#include <utility>

namespace idle {
namespace detail {
template <typename First, typename Second>
struct combined : public std::pair<First, Second> {
  using std::pair<First, Second>::pair;

  template <typename Container>
  decltype(auto) operator()(Container&& container) && {
    return std::move(this->second)(std::move(this->first)(container));
  }
};

template <typename Adapter>
struct view_base : public Adapter {
  using Adapter::Adapter;

  /// Apply the view to a container immediatly
  template <typename Container>
  friend decltype(auto) operator|(Container&& container, view_base&& view) {
    Adapter&& upcastable = std::move(
        *static_cast<Adapter*>(std::addressof(view)));
    return std::move(upcastable)(std::forward<Container>(container));
  }

  /// Combine two ranges together
  template <typename Other>
  friend decltype(auto) operator|(view_base<Other>&& left, view_base&& view) {
    using type_t = combined<Other, Adapter>;
    Other&& other = std::move(*static_cast<Other*>(std::addressof(left)));
    Adapter&& upcastable = std::move(
        *static_cast<Adapter*>(std::addressof(view)));
    return view_base<type_t>(std::move(other), std::move(upcastable));
  }
};

template <typename Transform, typename Base = std::decay_t<Transform>>
view_base<Base> make_view(Transform&& trans) {
  return view_base<Base>(std::forward<Transform>(trans));
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_VIEW_BASE_HPP_INCLUDED
