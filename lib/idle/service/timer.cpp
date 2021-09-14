
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

#include <idle/core/context.hpp>
#include <idle/core/ref.hpp>
#include <idle/service/detail/timer/timer_impl.hpp>
#include <idle/service/timer.hpp>

namespace idle {
Ref<Timer> Timer::create(Inheritance parent) {
  return spawn<timer_impl>(std::move(parent));
}

continuable<> Timer::waitFor(Duration exact) {
  return timer_impl::from(this)->wait_for_impl(exact);
}

continuable<> Timer::waitFor(Duration min, Duration max) {
  return timer_impl::from(this)->wait_for_impl(min, max);
}

continuable<> Timer::waitUntil(TimePoint exact) {
  return timer_impl::from(this)->wait_until_impl(exact);
}

continuable<> Timer::waitUntil(TimePoint min, TimePoint max) {
  return timer_impl::from(this)->wait_until_impl(min, max);
}
} // namespace idle
