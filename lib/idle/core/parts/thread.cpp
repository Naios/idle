
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

#include <string>
#include <idle/core/context.hpp>
#include <idle/core/parts/thread.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/thread_name.hpp>

namespace idle {
class ThreadImpl {
  using thread_state_t = Thread::ThreadState;

public:
  static void set_stopped(Thread& me) noexcept {
    IDLE_ASSERT(me.isRunning());

    me.state_.store(thread_state_t::stopped, std::memory_order_release);
    me.handle_.store({}, std::memory_order_release);
  }

  static void join_and_reset(Thread& me) noexcept {
    IDLE_ASSERT(me.owner().root().is_on_event_loop());
    IDLE_ASSERT(me.thread_->joinable());
    IDLE_ASSERT(!me.isRunning());

    me.thread_->join();
    me.thread_.reset();
  }
};

Thread::~Thread() {
  IDLE_ASSERT(!isRunning());
  IDLE_ASSERT(!thread_);
  IDLE_ASSERT(threadId() == std::thread::id{});
}

continuable<> Thread::launch(std::string thread_name) {
  return make_continuable<void>([this, thread_name = std::move(thread_name)](
                                    promise<>&& promise) mutable {
           IDLE_ASSERT(!isRunning());
           IDLE_ASSERT(!promise_);
           IDLE_ASSERT(!thread_);

           promise_ = std::forward<decltype(promise)>(promise);

           thread_.emplace([this, thread_name = std::move(thread_name)] {
             handle_.store(std::this_thread::get_id(),
                           std::memory_order_release);

             state_.store(ThreadState::launching, std::memory_order_release);

             set_this_thread_name(thread_name);

             try {
               onLaunch();
             } catch (...) {
               std::lock_guard<std::mutex> lock(mutex_);
               IDLE_ASSERT(promise_);
               ThreadImpl::set_stopped(*this);
               promise_.set_exception(std::current_exception());
               IDLE_ASSERT(!promise_);
               return;
             }

             {
               IDLE_ASSERT(promise_);
               std::lock_guard<std::mutex> lock(mutex_);
               IDLE_ASSERT(current_state() == ThreadState::launching);
               promise_.set_value();
               IDLE_ASSERT(!promise_);
             }

             state_.store(ThreadState::running, std::memory_order_release);

             try {
               onRun();
               set_this_thread_name();

             } catch (...) {
               std::lock_guard<std::mutex> lock(mutex_);
               if (isStopRequested()) {
                 ThreadImpl::set_stopped(*this);

                 IDLE_ASSERT(promise_);
                 promise_.set_exception(std::current_exception());
                 IDLE_ASSERT(!promise_);
               } else {
                 state_.store(ThreadState::finished, std::memory_order_release);

                 IDLE_ASSERT(!promise_);
                 exception_ = std::current_exception();
               }
               return;
             }

             std::lock_guard<std::mutex> lock(mutex_);

             if (isStopRequested()) {
               ThreadImpl::set_stopped(*this);

               IDLE_ASSERT(promise_);
               promise_.set_value();
               IDLE_ASSERT(!promise_);
             } else {
               state_.store(ThreadState::finished, std::memory_order_release);
             }
           });
         })
      .fail(
          [this](std::exception_ptr e) mutable {
            ThreadImpl::join_and_reset(*this);
            return rethrow(std::move(e));
          },
          owner().root().event_loop().through_post());
}

continuable<> Thread::requestStop() {
  return make_continuable<void>([this](auto&& promise) {
           IDLE_ASSERT(isRunning());
           IDLE_ASSERT(!isStopRequested());

           auto const state = current_state();

           {
             std::lock_guard<std::mutex> lock(mutex_);
             IDLE_ASSERT(state == ThreadState::running ||
                         state == ThreadState::finished);

             if (state == ThreadState::finished) {
               ThreadImpl::set_stopped(*this);

               if (exception_) {
                 std::forward<decltype(promise)>(promise).set_exception(
                     std::move(exception_));
               } else {
                 std::forward<decltype(promise)>(promise).set_value();
               }
               return;
             } else {
               IDLE_ASSERT(!promise_);
               promise_ = std::forward<decltype(promise)>(promise);
               state_.store(ThreadState::stop_requested,
                            std::memory_order_release);
               IDLE_ASSERT(promise_);
             }
           }

           onStopRequested();
         })
      .next(
          [this](auto&&... args) {
            ThreadImpl::join_and_reset(*this);
            return result<>::from(std::forward<decltype(args)>(args)...);
          },
          owner().root().event_loop().through_post());
}

void Thread::onLaunch() {
  IDLE_ASSERT(current_state() == ThreadState::launching);
}

void Thread::onRun() {
  IDLE_ASSERT(isRunning());
}

void Thread::onStopRequested() noexcept {
  IDLE_ASSERT(isStopRequested());
}
} // namespace idle
