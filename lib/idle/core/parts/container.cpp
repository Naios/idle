
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
#include <idle/core/parts/container.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
Container::~Container() {
  IDLE_ASSERT(children_.empty()); // Test

  while (!children_.empty()) {
    // Acquire a strong reference here such that the service is not
    // deallocated in the middle of the destruction.
    auto counter = children_.begin()->refCounter();
    counter.decrement();
    Ref<Service> child(&(*children_.begin()), std::move(counter));
    children_.erase(*child);
  }
}

void Container::partName(std::ostream& os) const {
  os << "idle::Container";
}

void Container::onPartInit() noexcept {
  IDLE_ASSERT(children().empty());
}

void Container::onPartDestroy() noexcept {
  while (!children_.empty()) {
    // Acquire a strong reference here such that the service is not
    // deallocated in the middle of the destruction.
    Ref<Service> child = refOf(*children_.begin());
    child->destroy();
    IDLE_ASSERT(!children_.contains_unsafe(*child));
  }
}

void Container::init_service(Ref<Service> svc) {
  owner().root().event_loop().dispatch([svc] {
    svc->init();
  });
}

void Container::onChildInit(Service& child) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(!child.isRoot());
  IDLE_ASSERT(!children_.contains_unsafe(child));
  child.refCounter().increment();
  children_.push_back(child);
}

void Container::onChildDestroy(Service& child) {
  IDLE_ASSERT(owner().root().is_on_event_loop());
  IDLE_ASSERT(children_.contains_unsafe(child));
  children_.erase(child);
  child.refCounter().decrement();
}
} // namespace idle
