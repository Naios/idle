
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

#ifndef IDLE_CORE_PARTS_STORAGE_VECTOR_STORAGE_HPP_INCLUDED
#define IDLE_CORE_PARTS_STORAGE_VECTOR_STORAGE_HPP_INCLUDED

#include <utility>
#include <idle/core/parts/dependency_types.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/flat_set.hpp>

namespace idle {
class Import;

/// Implements an ordered vector storage
template <typename Dependency, typename Predicate = Interface::GreaterPred>
class VectorStorage {
  using storage_t = FlatSet<std::reference_wrapper<Dependency>, Predicate>;

public:
  using const_iterator = typename storage_t::const_iterator;
  using const_pointer = typename storage_t::const_pointer;
  using const_reference = typename storage_t::const_reference;
  using const_reverse_iterator = typename storage_t::const_reverse_iterator;
  using difference_type = typename storage_t::difference_type;
  using iterator = typename storage_t::iterator;
  using pointer = typename storage_t::pointer;
  using reference = typename storage_t::reference;
  using reverse_iterator = typename storage_t::reverse_iterator;
  using size_type = typename storage_t::size_type;

  VectorStorage() = default;

  bool empty() const noexcept {
    return container_.empty();
  }
  size_type size() const noexcept {
    return container_.size();
  }
  const_iterator begin() const noexcept {
    return const_iterator{container_.begin()};
  }
  const_iterator cbegin() const noexcept {
    return const_iterator{container_.cbegin()};
  }
  const_iterator end() const noexcept {
    return const_iterator{container_.end()};
  }
  const_iterator cend() const noexcept {
    return const_iterator{container_.cend()};
  }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator{container_.rbegin()};
  }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator{container_.rend()};
  }

  Dependency& operator[](size_type pos) noexcept {
    IDLE_ASSERT(pos < size());
    return *(container_[pos]);
  }
  Dependency const& operator[](size_type pos) const noexcept {
    IDLE_ASSERT(pos < size());
    return *(container_[pos]);
  }

protected:
  void onInsert(Dependency& dep, StorageUpdate type) noexcept {
    (void)type;

    container_.insert(dep);
  }
  void onErase(Dependency& dep, StorageUpdate type) noexcept {
    (void)type;

    container_.erase(dep);
  }

  void onLock(Import& self) noexcept {
    (void)self;
  }

  void onUnlock(Import& self) noexcept {
    (void)self;
  }

private:
  /// References used interfaces, does not mutate while the service is running
  /// The interfaces are strongly referenced internally by an artificial_user.
  storage_t container_;
};
} // namespace idle

#endif // IDLE_CORE_PARTS_STORAGE_VECTOR_STORAGE_HPP_INCLUDED
