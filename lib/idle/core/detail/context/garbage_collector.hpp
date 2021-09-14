
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

#ifndef IDLE_CORE_DETAIL_CONTEXT_GARBAGE_COLLECTOR_HPP_INCLUDED
#define IDLE_CORE_DETAIL_CONTEXT_GARBAGE_COLLECTOR_HPP_INCLUDED

namespace idle {
class Service;
class Context;

namespace detail {
struct garbage_collector {
  static void on_service_init(Service& current) noexcept;
  static void on_service_start(Service& current) noexcept;
  static void on_service_stop(Service& current) noexcept;
  static void on_service_destroy(Service& current) noexcept;

  static void on_service_unique(Service& current) noexcept;
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_CONTEXT_GARBAGE_COLLECTOR_HPP_INCLUDED
