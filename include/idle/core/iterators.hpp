
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

#ifndef IDLE_CORE_ITERATORS_HPP_INCLUDED
#define IDLE_CORE_ITERATORS_HPP_INCLUDED

#include <cstddef>
#include <idle/core/api.hpp>
#include <idle/core/detail/service_iterator.hpp>
#include <idle/core/util/iterator_facade.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
class IDLE_API(idle) transitive_service_iterator
  : public detail::service_iterator<transitive_service_iterator> {

public:
  using service_iterator<transitive_service_iterator>::service_iterator;

  void increment() noexcept;
};

/// Returns a range view over the services contained in this service
/// and all its parent services (all services in this system).
///
/// The iteration is reflexive such that the current service itself is
/// contained in the iteration range.
IDLE_API(idle)
Range<transitive_service_iterator>
transitive_services(Service& current) noexcept;

class IDLE_API(idle) transitive_postorder_service_iterator
  : public detail::service_iterator<transitive_postorder_service_iterator> {

public:
  using service_iterator<
      transitive_postorder_service_iterator>::service_iterator;

  void increment() noexcept;
};

/// Returns a range view over the services contained in this service
/// and all its parent services (all services in this system) but the
/// services are returned in post-order.
///
/// The iteration is reflexive such that the current service itself is
/// contained in the iteration range.
IDLE_API(idle)
Range<transitive_postorder_service_iterator>
transitive_postorder_services(Service& current) noexcept;

class IDLE_API(idle) cluster_service_iterator
  : public detail::service_iterator<cluster_service_iterator> {

public:
  using service_iterator<cluster_service_iterator>::service_iterator;

  void increment() noexcept;
};

/// Returns an iterable range over the services that are inside the cluster
/// of the current services.
///
/// If the given service is not the cluster head, the range will not
/// iterate over the complete cluster.
IDLE_API(idle)
Range<cluster_service_iterator> cluster_members(Service& current) noexcept;

/// Returns an iterable range over the services that are inside the cluster
/// of the current services.
///
/// The iteration will start from the cluster head,
/// if any cluster child is given.
IDLE_API(idle)
Range<cluster_service_iterator> all_cluster_members(Service& current) noexcept;

class IDLE_API(idle) cluster_iterator
  : public detail::service_iterator<cluster_iterator> {

public:
  using service_iterator<cluster_iterator>::service_iterator;

  void increment() noexcept;
};

/// Returns an iterable range over all cluster heads transitively contained
/// in the given service.
IDLE_API(idle) Range<cluster_iterator> clusters(Service& current) noexcept;

class IDLE_API(idle) parent_service_iterator
  : public detail::service_iterator<parent_service_iterator> {

public:
  using service_iterator<parent_service_iterator>::service_iterator;

  void increment() noexcept;
};

/// Returns an iterable range over the current service and the parent up
/// to the corresponding root_service.
///
/// The iteration is reflexive such that the current service itself is
/// contained in the iteration range.
IDLE_API(idle)
Range<parent_service_iterator> parent_services(Service& current) noexcept;

class IDLE_API(idle) cluster_parent_service_iterator
  : public detail::service_iterator<cluster_parent_service_iterator> {

public:
  using service_iterator<cluster_parent_service_iterator>::service_iterator;

  void increment() noexcept;
};

/// Returns an iterable range over the current service and the parent up
/// to the corresponding cluster head.
///
/// The iteration is reflexive such that the current service itself is
/// contained in the iteration range.
IDLE_API(idle)
Range<cluster_parent_service_iterator>
cluster_parent_services(Service& current) noexcept;
} // namespace idle

#endif // IDLE_CORE_ITERATORS_HPP_INCLUDED
