
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

#include <idle/idle.hpp>

using namespace idle;

namespace examples {
/// Implements a Service that shows all possible states
/// of the Service lifecycle.
///
/// The Service lifecycle states can occur as following:
///  +---------+
///  |         V
///  |  - 1) onInit
///  |  - 2) onSetup <-+
///  |  - 3) onStart   |
///  |  - 4) onStop -->+
///  |  - 5) onDestroy
///  |         V
///  +---------+
class LifeCycleService final : public Implements<Autostarted> {
public:
  using Super::Super;

  void onInit() override {
    IDLE_ASSERT(root().is_on_event_loop());

    // 1)
    // The service is initialized on the event loop thread,
    // dependencies are not resolved here yet
  }

  void onSetup() override {
    IDLE_ASSERT(root().is_on_event_loop());

    // 2)
    // The service (and its strong compound cluster) is prepared for start on
    // the event loop thread, dependencies are resolved here and usable.
    // onSetup is called for the whole cluster, beginning at the cluster head
    // down to its children.
    // head ---> child 1 --> child 1.1
    //       *-> child 2 --> child 2.1
    //                   *-> child 2.2
  }

  continuable<> onStart() override {
    return async([this] {
      IDLE_ASSERT(root().is_on_event_loop());

      // 3)
      // The service is starting, perform all logic here to get the Service up.
    });
  }

  continuable<> onStop() override {
    return async([this] {
      IDLE_ASSERT(root().is_on_event_loop());

      // 4)
      // The service is starting, perform all logic here to
      // get the Service down.
    });
  }

  void onDestroy() override {
    IDLE_ASSERT(root().is_on_event_loop());

    // 5)
    // The service is destroyed on the event loop thread,
    // dependencies are not resolved here anymore
  }

  IDLE_SERVICE
};

IDLE_DECLARE(LifeCycleService)
} // namespace examples
