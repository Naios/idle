
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

#ifndef IDLE_CORE_PARTS_POLLABLE_EXECUTOR_HPP_INCLUDED
#define IDLE_CORE_PARTS_POLLABLE_EXECUTOR_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/fwd.hpp>
#include <idle/core/import.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/executor_facade.hpp>

namespace idle {
/// Represents a pollable executor that can receive work from
/// any thread and dispatches the work when PollableExecutor::pool is called.
class IDLE_API(idle) PollableExecutor final
  : public Import,
    public ExecutorFacade<PollableExecutor> {

  class Impl;
  friend class ExecutorFacade<PollableExecutor>;

public:
  explicit PollableExecutor(Service& owner);
  ~PollableExecutor() override;

  /// Sets the current thread as the active one, where work is dispatched.
  ///
  /// \attention Setting different threads than the active thread
  ///            during the same lifecycle of this object is not supported.
  void setRunningFromThisThread() noexcept;

  /// Dispatches all outstanding work on the current thread.
  ///
  /// \attention In case an active thread is set, this method must be invoked
  ///            from the active thead.
  void poll();

  /// Returns true if the current thread is the active thread,
  /// where the executor is dispatched on.
  bool isThisThread() const noexcept;

  Service& owner() noexcept override {
    return owner_;
  }
  Service const& owner() const noexcept override {
    return owner_;
  }

  bool can_dispatch_inplace() const noexcept;
  void queue(work&& work) noexcept;

protected:
  void onImportUnlock() noexcept override;

  Service& owner_;
  Impl* impl_;

  IDLE_PART
};
} // namespace idle

#endif // IDLE_CORE_PARTS_POLLABLE_EXECUTOR_HPP_INCLUDED
