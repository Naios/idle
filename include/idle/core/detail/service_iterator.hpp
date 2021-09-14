
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

#ifndef IDLE_CORE_DETAIL_SERVICE_ITERATOR_HPP_INCLUDED
#define IDLE_CORE_DETAIL_SERVICE_ITERATOR_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/util/iterator_facade.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
class Service;

namespace detail {
template <typename T>
class service_iterator
  : public iterator_facade<T, std::forward_iterator_tag, Service&,
                           std::ptrdiff_t, Service*, Service&> {
public:
  service_iterator() = default;

  explicit service_iterator(Service& current) noexcept
    : current_(&current) {}

  Service& dereference() const noexcept {
    return *current_;
  }

  bool equal(T const& other) const noexcept {
    return current_ == other.current_;
  }

protected:
  Service* current_{nullptr};
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_SERVICE_ITERATOR_HPP_INCLUDED
