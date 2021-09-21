
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

#include <cstdint>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/iterators.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/bitset.hpp>

namespace idle {
enum class iteration_flags_t : std::uint8_t {
  iterate_imports, // Step into children of importers
  iterate_exports, // Step into children of importers
};

// Go down
static Service* next_child_of_parts(Range<PartList::iterator> parts,
                                    BitSet<iteration_flags_t> flags) noexcept {

  for (Part& current : parts) {
    if (isa<Export>(current) &&
        !flags.contains(iteration_flags_t::iterate_exports)) {
      continue;
    }

    if (isa<Import>(current) &&
        !flags.contains(iteration_flags_t::iterate_imports)) {
      continue;
    }

    if (auto children = current.children()) {
      return &(children.front());
    }
  }

  return nullptr;
}

// Go right
static Service* next_neighbour_or_child(Service* current,
                                        BitSet<iteration_flags_t> flags) {

  // 2. If not possible go to the right neighbor of the services
  //    contained inside the current part.
  if (auto const neighbors = current->neighbors().next()) {
    return &(neighbors.front());
  }

  // 3. If we are at the right most neighbor check for the services inside
  //    the neighboring part.
  if (Service* child = next_child_of_parts(current->parent().neighbors().next(),
                                           flags)) {
    return child;
  }

  return nullptr;
}

// Go up
static Service* next_parent_or_neighbour(Service* current,
                                         BitSet<iteration_flags_t> flags) {

  for (; !current->isRoot(); current = &(current->parent().owner())) {
    if (Service* next = next_neighbour_or_child(current, flags)) {
      return next;
    }
  }

  // 4. We have finished the traversal and reached the end,
  //    which means `current == current.root() == current.parent()`.
  return nullptr;
}

static Service* next_child_or_up(Service* current,
                                 BitSet<iteration_flags_t> flags) {
  IDLE_ASSERT(current);

  // 1. Go one child in the tree down (DFS)
  if (Service* child = next_child_of_parts(current->parts(), flags)) {
    return child;
  }

  return next_parent_or_neighbour(current, flags);
}

void transitive_service_iterator::increment() noexcept {
  current_ = next_child_or_up(current_, BitSet<iteration_flags_t>::all());
}

Range<transitive_service_iterator>
transitive_services(Service& current) noexcept {
  return make_range(transitive_service_iterator(current), {});
}

static_assert(
    std::is_same<
        std::iterator_traits<transitive_postorder_service_iterator>::value_type,
        Service&>::value,
    "");

void transitive_postorder_service_iterator::increment() noexcept {
  IDLE_ASSERT(current_);

  Service* current = next_neighbour_or_child(current_,
                                             BitSet<iteration_flags_t>::all());

  if (current) {
    Service* previous;

    do {
      IDLE_ASSERT(current);

      previous = current;
      current = next_child_of_parts(current->parts(),
                                    BitSet<iteration_flags_t>::all());
    } while (current);

    current_ = previous;
  } else if (current_->isRoot()) {
    current_ = nullptr;
  } else {
    current_ = &(current_->parent().owner());
  }
}

Range<transitive_postorder_service_iterator>
transitive_postorder_services(Service& current) noexcept {

  Service* previous;
  Service* actual = &current;

  do {
    previous = actual;
    actual = next_child_of_parts(actual->parts(),
                                 BitSet<iteration_flags_t>::all());
  } while (actual);

  IDLE_ASSERT(previous);
  return make_range(transitive_postorder_service_iterator(*previous), {});
}

void cluster_service_iterator::increment() noexcept {
  current_ = next_child_or_up(current_, {iteration_flags_t::iterate_imports});
}

Range<cluster_service_iterator> cluster_members(Service& current) noexcept {
  Service& head = get_cluster_head_of(current);
  Service* const end = next_parent_or_neighbour(
      &head, {iteration_flags_t::iterate_imports});
  return make_range(cluster_service_iterator(current),
                    end ? cluster_service_iterator(*end)
                        : cluster_service_iterator{});
}

Range<cluster_service_iterator> all_cluster_members(Service& current) noexcept {
  return cluster_members(get_cluster_head_of(current));
}

void cluster_iterator::increment() noexcept {
  // TODO This can be improved such that we do not perform an intermediate loop
  do {
    current_ = next_child_or_up(current_, {iteration_flags_t::iterate_exports,
                                           iteration_flags_t::iterate_imports});
  } while (current_ && !is_cluster_head(*current_));
}

Range<cluster_iterator> clusters(Service& current) noexcept {
  Service* const end = next_parent_or_neighbour(
      &current,
      {iteration_flags_t::iterate_exports, iteration_flags_t::iterate_imports});

  return make_range(cluster_iterator(current),
                    end ? cluster_iterator(*end) : cluster_iterator{});
}

void parent_service_iterator::increment() noexcept {
  IDLE_ASSERT(current_);

  Service& parent = current_->parent().owner();
  if (parent == *current_) {
    current_ = nullptr;
  } else {
    current_ = std::addressof(parent);
  }
}

Range<parent_service_iterator> parent_services(Service& current) noexcept {
  return make_range(parent_service_iterator(current), {});
}

void cluster_parent_service_iterator::increment() noexcept {
  IDLE_ASSERT(current_);

  Part& parent = current_->parent();
  if (isa<Import>(parent)) {
    current_ = std::addressof(parent.owner());
  } else {
    current_ = nullptr;
  }
}

Range<cluster_parent_service_iterator>
cluster_parent_services(Service& current) noexcept {
  return make_range(cluster_parent_service_iterator(current), {});
}
} // namespace idle
