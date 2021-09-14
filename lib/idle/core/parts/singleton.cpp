
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

#include <idle/core/parts/singleton.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
static bool isTopPriorityInterface(Interface const& excluded) noexcept {
  auto const neighbors = excluded.publishedNeighbors();
  auto const previous = --neighbors.begin();

  IDLE_ASSERT(previous != neighbors.begin());
  return previous == neighbors.end();
}

static bool isAnySiblingLocked(Registry const& entry) noexcept {
  for (Interface const& inter : entry.interfaces()) {
    if (inter.owner().state().isLocked()) {
      return true;
    }
  }
  return false;
}

Singleton::Singleton(Interface& excluded) noexcept
  : Import(excluded.owner())
  , Subscriber({Event::OnCreated, Event::OnDestroy, //
                Event::OnLock, Event::OnUnlocked})
  , excluded_(excluded) {}

Service& Singleton::owner() noexcept {
  return excluded_.owner();
}

Service const& Singleton::owner() const noexcept {
  return excluded_.owner();
}

void Singleton::onPartInit() noexcept {
  excluded_.registry().subscriberAdd(*this);
}

void Singleton::onPartDestroy() noexcept {
  excluded_.registry().subscriberDel(*this);
}

void Singleton::onSubscribedCreated(Interface& subscribed) {
  if (isSatisfied()) {
    if (subscribed == excluded_) {
      bool const is_any_locked = isAnySiblingLocked(excluded_.registry());
      if (is_any_locked || !isTopPriorityInterface(excluded_)) {
        setSatisfied(false);
      }

      IDLE_ASSERT(!isSatisfied() || isTopPriorityInterface(excluded_));
      IDLE_ASSERT(!isSatisfied() || !is_any_locked);
    } else if (!isTopPriorityInterface(excluded_)) {
      IDLE_ASSERT(isTopPriorityInterface(subscribed));

      setSatisfied(false);
    }
  }
}

void Singleton::onSubscribedDestroy(Interface& subscribed) {
  (void)subscribed;

  if (!isSatisfied()) {
    if (isTopPriorityInterface(excluded_)) {
      setSatisfied(true);
    }
  }
}

void Singleton::onSubscribedLock(Interface& subscribed) noexcept {
  (void)subscribed;
  IDLE_ASSERT((subscribed != excluded_) || isTopPriorityInterface(excluded_));
}

void Singleton::onSubscribedUnlocked(Interface& subscribed) noexcept {
  if (subscribed != excluded_) {
    if (!isSatisfied()) {
      if (isTopPriorityInterface(excluded_)) {
        setSatisfied(true);
      }
    }
  } else {
    if (!isTopPriorityInterface(excluded_)) {
      setSatisfied(false);
    }
  }
}
} // namespace idle
