
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

#ifndef IDLE_CORE_DETAIL_SERVICE_DELETER_HPP_INCLUDED
#define IDLE_CORE_DETAIL_SERVICE_DELETER_HPP_INCLUDED

#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/locality.hpp>
#include <idle/core/ref.hpp>

namespace idle {
class Service;

namespace detail {
class IDLE_API(idle) service_deleter {
public:
  explicit service_deleter(RefCounter locality = {}) noexcept
    : locality_(std::move(locality)) {}

  void on_unique(Service* current) noexcept;
  void on_destruct(Service* current) noexcept;

  template <typename Service, typename Data>
  void on_unique(joined_allocation_block<Service, Data>* block) noexcept {
    this->on_unique(&block->obj);
  }
  template <typename Service, typename Data>
  void on_destruct(joined_allocation_block<Service, Data>* block) noexcept {
    this->on_destruct(&block->obj);
  }

private:
  /// Embed the current locality into the control block
  RefCounter locality_;
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_SERVICE_DELETER_HPP_INCLUDED
