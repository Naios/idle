
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

#ifndef IDLE_CORE_PARTS_DEPENDENCY_TYPES_HPP_INCLUDED
#define IDLE_CORE_PARTS_DEPENDENCY_TYPES_HPP_INCLUDED

#include <cstdint>
#include <iosfwd>
#include <idle/core/api.hpp>

namespace idle {
/// Specifies possible replies of the dependencies onUseOffer callable
enum class UseReply : std::uint8_t {
  /// No usage is created to the given dependency within this epoch
  ///
  /// A usage is automatically acquired for the next epoch (the next run after
  /// the owning service has been shut down).
  UseNever,

  /// An usage is created to the given dependency and it is inserted into
  /// the user dependency storage (on the event loop).
  /// The service is also inserted into the storage if it is not running yet.
  ///
  /// \attention Make sure that it is thread-safe to access the storage
  ///            from the event loop while the part is locked.
  ///            In most cases this means that you are only allowed to access
  ///            the storage from the event loop directly.
  UseNowAndInsert,

  /// An usage is created to the given dependency.
  UseLater,

  /// An usage is created to the given dependency and onUseOffer
  /// is called again as soon as the service is running.
  /// If the service is running already the hook won't be called again.
  UseLaterAndNotify,

  /// An usage is created to the given dependency and it is inserted into
  /// the user dependency storage as soon as the service is running.
  /// This can happen immediately if the service is running already.
  ///
  /// \attention Make sure that it is thread-safe to access the storage later
  ///            from the event loop while the part is locked.
  ///            In most cases this means that you are only allowed to access
  ///            the storage from the event loop directly.
  UseLaterAndInsert
};

IDLE_API(idle) std::ostream& operator<<(std::ostream& os, UseReply value);

/// Specifies the reason why the storage was updated and can be a hint
/// for efficiently implementing storage update policies.
enum class StorageUpdate : std::uint8_t {
  /// The container is changed due to pre-population, this is always ensured to
  /// be thread safe since the action is always issued through the event loop.
  Implicit,

  /// The container is changed automatically while running due to a returned
  /// use_reply_t::use_now_and_insert. This means that the storage is only
  ///  accessed from the event loop so use it with caution.
  Auto,

  /// The container is changed manually through dependencies::insert
  /// or dependencies::erase and runs on the thread the user is using.
  Manual,
};

IDLE_API(idle)
std::ostream& operator<<(std::ostream& os, StorageUpdate value);
} // namespace idle

#endif // IDLE_CORE_PARTS_DEPENDENCY_TYPES_HPP_INCLUDED
