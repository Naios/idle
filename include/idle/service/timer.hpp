
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

#ifndef IDLE_SERVICE_TIMER_HPP_INCLUDED
#define IDLE_SERVICE_TIMER_HPP_INCLUDED

#include <chrono>
#include <idle/core/api.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/service.hpp>

namespace idle {
class IDLE_API(idle) Timer : public Service {
  friend class timer_impl;
  using Service::Service;

  using clock_type = std::chrono::steady_clock;

public:
  using Duration = clock_type::duration;
  using TimePoint = clock_type::time_point;

  static Ref<Timer> create(Inheritance parent);

  /// Wait for the given duration asynchronously
  continuable<> waitFor(Duration exact);
  /// Wait for a duration between the given min and max duration asynchronously
  continuable<> waitFor(Duration min, Duration max);

  /// Wait until the given time point asynchronously
  continuable<> waitUntil(TimePoint exact);
  /// Wait until a time point between the given
  /// min and max time point asynchronously
  continuable<> waitUntil(TimePoint min, TimePoint max);

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_SERVICE_TIMER_HPP_INCLUDED
