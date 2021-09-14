
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

#ifndef IDLE_SERVICE_STRAND_HPP_INCLUDED
#define IDLE_SERVICE_STRAND_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/executor_facade.hpp>

namespace idle {
class Strand;

class IDLE_API(idle) Strand : public Service, public ExecutorFacade<Strand> {
  friend class ExecutorFacade<Strand>;

protected:
  using Service::Service;

public:
  static Ref<Strand> create(Inheritance parent);

private:
  bool can_dispatch_inplace() const noexcept;
  void queue(work&& work) noexcept;

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_SERVICE_STRAND_HPP_INCLUDED
