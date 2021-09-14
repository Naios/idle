
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

#ifndef IDLE_CORE_PARTS_STORAGE_VOID_STORAGE_HPP_INCLUDED
#define IDLE_CORE_PARTS_STORAGE_VOID_STORAGE_HPP_INCLUDED

#include <idle/core/parts/dependency_types.hpp>

namespace idle {
class Import;

/// A dependency storage which doesn't store any dependencies in user space
///
/// This is useful for implementing your own container used by dependency_list,
/// or just have a dependency_list that references other interfaces per usage.
///
/// The storage is always ensured to be called in a thread safe way which means
/// from the storage implementation side there is no extra mutual exclusion
/// required.
template <typename Dependency>
struct VoidStorage {
protected:
  /// Is called when a dependency is inserted from the user container
  void onInsert(Dependency& dep, StorageUpdate type) noexcept {
    // Insert the dependency into your container here
    (void)dep;
    (void)type;
  }
  /// Is called when a dependency is erased from the user container
  void onErase(Dependency& dep, StorageUpdate type) noexcept {
    // Erase the dependency from your container here
    (void)dep;
    (void)type;
  }

  /// Is called when the importer is locked
  void onLock(Import& self) noexcept {
    // Here you could possibly sort your container or access
    // the underlying dependencies class.
    (void)self;
  }
  /// Is called when the importer is unlocked
  void onUnlock(Import& self) noexcept {
    // Here you could possibly sort your container or access
    // the underlying dependencies class.
    (void)self;
  }
};
} // namespace idle

#endif // IDLE_CORE_PARTS_STORAGE_VOID_STORAGE_HPP_INCLUDED
