
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

#include <ostream>
#include <idle/core/context.hpp>
#include <idle/core/parts/collection.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
Collection::~Collection() {
  IDLE_ASSERT(children_.empty());
}

void Collection::onPartInit() noexcept {
  IDLE_ASSERT(children_.empty());
}

void Collection::onPartDestroy() noexcept {
  clear();
}

void Collection::clear() {
  IDLE_ASSERT(owner().root().is_on_event_loop());

  while (!children_.empty()) {
    children_.front().destroy();
  }
}

void Collection::onChildInit(Service& child) {
  IDLE_ASSERT(!child.isRoot());
  IDLE_ASSERT(!children_.contains_unsafe(child));
  children_.push_back(child);
}

void Collection::onChildDestroy(Service& child) {
  IDLE_ASSERT(children_.contains_unsafe(child));
  children_.erase(child);
}

void Collection::partName(std::ostream& os) const {
  os << "idle::Collection";
}
} // namespace idle
