
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

#ifndef IDLE_CORE_DETAIL_CONTEXT_EVENT_LOOP_EXECUTOR_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_CONTEXT_EVENT_LOOP_EXECUTOR_IMPL_HPP_INCLUDED

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <ostream>
#include <thread>
#include <concurrentqueue/concurrentqueue.h>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/service.hpp>

#ifdef _MSC_VER
#  pragma warning(disable : 4245)
#  pragma warning(disable : 4127)
#endif // _MSC_VER

namespace idle {
class EventLoopExecutorImpl : public Import {
  friend class event_loop_executor_type;

  enum class state_t : std::uint8_t {
    ready, //
    running,
    stopping,
    destroyed,
    stopped
  };

public:
  using executor_type = Context::executor_type;

  explicit EventLoopExecutorImpl(Service& owner)
    : Import(owner)
    , consumer_token_(queue_) {}
  ~EventLoopExecutorImpl();

  void prepare_ev();
  void run_ev();

  void onImportLock() noexcept override {}

  void onImportUnlock() noexcept override {
    IDLE_ASSERT(is(state_t::running));

    owner().root().event_loop().post([this] {
      set(state_t::stopping);
    });
  }

  bool is_on_event_loop() const noexcept {
    switch (state_.load(std::memory_order_acquire)) {
      case state_t::running:
      case state_t::stopping:
      case state_t::destroyed: {
        return thread_id_ == std::this_thread::get_id();
      }
      default: {
        return false;
      }
    }
  }

  bool is_running() const noexcept {
    switch (state_.load(std::memory_order_acquire)) {
      case state_t::ready:
      case state_t::running:
      case state_t::stopping: {
        return true;
      }
      default: {
        return false;
      }
    }
  }

  bool is_stopped() const noexcept {
    return !is_running();
  }

  void queue(work&& work);

  void set_running_from_this_thread() noexcept {
    IDLE_ASSERT(is(state_t::ready));
    thread_id_ = std::this_thread::get_id();
  }
  void unset_running_from_this_thread() noexcept {
    IDLE_ASSERT(is(state_t::stopped));
    thread_id_ = {};
  }

protected:
  void partName(std::ostream& os) const override {
    os << "idle::event_loop";
  }

private:
  void dispatch_all();

  bool is(state_t state) const noexcept {
    return state == state_.load(std::memory_order_acquire);
  }
  void set(state_t state) noexcept {
    state_.store(state, std::memory_order_release);
  }

  std::mutex mutex_;
  std::condition_variable condition_;
  moodycamel::ConcurrentQueue<work> queue_;
  moodycamel::ConsumerToken consumer_token_;
  std::thread::id thread_id_{};
  std::atomic<state_t> state_{state_t::ready};
};
} // namespace idle

#endif // IDLE_CORE_DETAIL_CONTEXT_EVENT_LOOP_EXECUTOR_IMPL_HPP_INCLUDED
