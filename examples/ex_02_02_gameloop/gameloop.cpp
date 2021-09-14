
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

#include <chrono>
#include <thread>
#include <idle/idle.hpp>

using namespace idle;
using namespace std::chrono_literals;

namespace examples {
/// Implements an eager service that spawns a new ticking thread on start.
///
/// The \see idle::Thread is stopped asynchronously when the service is stopped.
class GameLoopService final : public Implements<Autostarted, Thread> {
public:
  using Super::Super;

  continuable<> onStart() override {
    return Thread::launch("GameLoop Thread");
  }

  continuable<> onStop() override {
    return Thread::requestStop();
  }

  void onRun() override {
    unsigned int step = 0;

    while (!isStopRequested()) {
      IDLE_LOG_INFO(log_, "Game loop tick: {}", step++);
      std::this_thread::sleep_for(1s);
    }
  }

private:
  Dependency<Log> log_{*this};

  IDLE_SERVICE
};

IDLE_DECLARE(GameLoopService)
} // namespace examples
