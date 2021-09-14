
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

#ifndef IDLE_CORE_VIEWS_TRANSFORM_HPP_INCLUDED
#define IDLE_CORE_VIEWS_TRANSFORM_HPP_INCLUDED

#include <type_traits>
#include <idle/core/detail/view_base.hpp>
#include <idle/core/detail/views/transform_impl.hpp>

namespace idle {
namespace views {
template <typename Transformer, typename Adapter = detail::transform_adapter<
                                    std::decay_t<Transformer>>>
detail::view_base<Adapter> transform(Transformer&& trans) {
  return detail::view_base<Adapter>(Adapter(std::forward<Transformer>(trans)));
}
} // namespace views
} // namespace idle

#endif // IDLE_CORE_VIEWS_TRANSFORM_HPP_INCLUDED
