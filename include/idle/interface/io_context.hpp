
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

#ifndef IDLE_INTERFACE_IO_CONTEXT_HPP_INCLUDED
#define IDLE_INTERFACE_IO_CONTEXT_HPP_INCLUDED

#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/executor_facade.hpp>

namespace boost {
namespace asio {
class io_context;
}
} // namespace boost

namespace idle {
class IOContext;

class IDLE_API(idle) IOContext : public Interface,
                                 public ExecutorFacade<IOContext> {

  friend class ExecutorFacade<IOContext>;

public:
  using Interface::Interface;

  boost::asio::io_context& get() noexcept {
    IDLE_ASSERT(this->owner().state().isRunning());
    return *io_context_;
  }

  static Ref<IOContext> create(Inheritance parent);

protected:
  boost::asio::io_context* io_context_{nullptr};

  bool can_dispatch_inplace() const noexcept;
  void queue(work work) noexcept;

  IDLE_INTERFACE
};

} // namespace idle

#endif // IDLE_INTERFACE_IO_CONTEXT_HPP_INCLUDED
