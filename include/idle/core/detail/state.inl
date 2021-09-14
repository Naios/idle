
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

#ifndef IDLE_DETAIL_FOR_EACH_PHASE
#  define IDLE_DETAIL_FOR_EACH_PHASE(NAME)
#endif // IDLE_DETAIL_FOR_EACH_PHASE

IDLE_DETAIL_FOR_EACH_PHASE(uninitialized) /// The service is not initialized yet
IDLE_DETAIL_FOR_EACH_PHASE(on_init)       /// The on_init_hook was called
IDLE_DETAIL_FOR_EACH_PHASE(stopped)       /// The service is stopped
IDLE_DETAIL_FOR_EACH_PHASE(locked)        /// Outer cluster deps are locked
IDLE_DETAIL_FOR_EACH_PHASE(starting)      /// Wait for on_start
IDLE_DETAIL_FOR_EACH_PHASE(running) /// The service is running and might be used
IDLE_DETAIL_FOR_EACH_PHASE(stopping) /// Wait for on_stop
IDLE_DETAIL_FOR_EACH_PHASE(pending)  /// Stopped but waiting for stop of cluster
IDLE_DETAIL_FOR_EACH_PHASE(on_destroy) /// The on_destroy hook was called
IDLE_DETAIL_FOR_EACH_PHASE(destroyed)  /// The service is destroyed

#undef IDLE_DETAIL_FOR_EACH_PHASE

#ifndef IDLE_DETAIL_FOR_EACH_OVERRIDE
#  define IDLE_DETAIL_FOR_EACH_OVERRIDE(NAME)
#endif // IDLE_DETAIL_FOR_EACH_OVERRIDE

IDLE_DETAIL_FOR_EACH_OVERRIDE(none)
IDLE_DETAIL_FOR_EACH_OVERRIDE(start)

#undef IDLE_DETAIL_FOR_EACH_OVERRIDE

#ifndef IDLE_DETAIL_FOR_EACH_TARGET
#  define IDLE_DETAIL_FOR_EACH_TARGET(NAME)
#endif // IDLE_DETAIL_FOR_EACH_TARGET

IDLE_DETAIL_FOR_EACH_TARGET(none)
IDLE_DETAIL_FOR_EACH_TARGET(start)
IDLE_DETAIL_FOR_EACH_TARGET(stop)

#undef IDLE_DETAIL_FOR_EACH_TARGET
