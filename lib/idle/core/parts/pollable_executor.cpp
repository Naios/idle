
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

#include <atomic>
#include <thread>
#include <concurrentqueue/concurrentqueue.h>
#include <idle/core/parts/pollable_executor.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
class PollableExecutor::Impl final {
public:
  Impl()
    : consumer_token_(queue_) {}
  ~Impl() {
    IDLE_ASSERT(queue_.size_approx() == 0U);
  }

  std::atomic<std::thread::id> this_thread_;
  moodycamel::ConcurrentQueue<work> queue_;
  moodycamel::ConcurrentQueue<work>::consumer_token_t consumer_token_;
};

PollableExecutor::PollableExecutor(Service& owner)
  : Import(owner)
  , owner_(owner)
  , impl_(new Impl()) {
  IDLE_ASSERT(impl_);
}

PollableExecutor::~PollableExecutor() {
  IDLE_ASSERT(impl_);

  delete impl_;
  impl_ = nullptr;
}

void PollableExecutor::setRunningFromThisThread() noexcept {
  IDLE_ASSERT(impl_);

  IDLE_ASSERT(impl_->this_thread_.load(std::memory_order_acquire) ==
              std::thread::id{});

  impl_->this_thread_.store(std::this_thread::get_id(),
                            std::memory_order_release);
}

void PollableExecutor::poll() {
  IDLE_ASSERT(((impl_->this_thread_.load(std::memory_order_acquire) ==
                std::thread::id{}) ||
               isThisThread()) &&
              "Used pool from a different thread than the active one!");

  work w;

  while (impl_->queue_.try_dequeue(impl_->consumer_token_, w)) {
    std::move(w).set_value();
  }
}

bool PollableExecutor::isThisThread() const noexcept {
  IDLE_ASSERT(impl_);

  return impl_->this_thread_.load(std::memory_order_relaxed) ==
         std::this_thread::get_id();
}

void PollableExecutor::onImportUnlock() noexcept {
  IDLE_ASSERT(impl_);

  impl_->this_thread_.store(std::thread::id{}, std::memory_order_release);

  IDLE_ASSERT(impl_->queue_.size_approx() == 0U);
}

bool PollableExecutor::can_dispatch_inplace() const noexcept {
  IDLE_ASSERT(impl_);

  return owner().state().isRunning() && isThisThread();
}

void PollableExecutor::queue(work&& work) noexcept {
  IDLE_ASSERT(impl_);

  if (owner().state().isRunning()) {
    impl_->queue_.enqueue(std::move(work));
  } else {
    work.set_canceled();
  }
}
} // namespace idle
