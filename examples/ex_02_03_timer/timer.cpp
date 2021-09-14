
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
#include <idle/idle.hpp>

using namespace idle;
using namespace std::chrono_literals;

namespace examples {
/// Implements an eager service that greets asynchronously after 10s
class TimerGreetingService final : public Implements<Autostarted> {
public:
  using Super::Super;

  continuable<> onStart() override {
    return async([this] {
      // Spawns the timed action detached from the control flow.
      timer_->waitFor(10s).then(wrap(*this, [](Use<TimerGreetingService> me) {
        IDLE_LOG_INFO(me->log_, "Hello from TimerGreetingService!");
      }));

      // If you return the following expression, then the TimerGreetingService
      // will start after 10 seconds:
      //
      // return timer_->waitFor(10s).then(
      //     wrap(*this, [](Use<TimerGreetingService> me) {
      //       IDLE_LOG_INFO(me->log_, "TimerGreetingService started after
      //       10s!");
      //     }));
    });
  }

private:
  Component<Timer> timer_{*this};
  Dependency<Log> log_{*this};

  IDLE_SERVICE
};

IDLE_DECLARE(TimerGreetingService)
} // namespace examples
