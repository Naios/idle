
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

#include <ostream>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/context/event_loop_executor_impl.hpp>
#include <idle/core/detail/for_each.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/iterators.hpp>
#include <idle/core/util/thread_name.hpp>

namespace idle {
EventLoopExecutorImpl::~EventLoopExecutorImpl() {
  // IDLE_ASSERT(queue_.size_approx() == 0U);
}

void EventLoopExecutorImpl::prepare_ev() {
  thread_id_ = std::this_thread::get_id();

  set(state_t::running);
}

void EventLoopExecutorImpl::run_ev() {
  IDLE_DETAIL_LOG_DEBUG("event loop started, ~{} pending handlers in queue",
                        queue_.size_approx());

  std::unique_lock<std::mutex> lock(mutex_);
  for (;;) {
    dispatch_all();

    if (is(state_t::stopping)) {
      break;
    }

    condition_.wait(lock);
  }

  Service& context = owner();

  // Destroy the root owner and stop accepting more work
  transitive_postorder_service_iterator const end(context);
  safe_for_each(transitive_postorder_services(context).begin(), end,
                [&](Service& current) {
                  if (current.state().isInitializedUnsafe()) {
                    IDLE_ASSERT(current.state().isStopped());
                    current.destroy();
                  }
                });

  IDLE_DETAIL_LOG_DEBUG("event loop stopping destroying root {}", context);

  // Finally destroy the service
  context.destroy();

  set(state_t::destroyed);

  IDLE_DETAIL_LOG_DEBUG("event loop stopping, dispatching ~{} pending handlers "
                        "after root destruction",
                        queue_.size_approx());

  // Dispatch all pending work
  dispatch_all();

  set(state_t::stopped);

  unset_running_from_this_thread();

  IDLE_ASSERT(queue_.size_approx() == 0U);
}

void EventLoopExecutorImpl::queue(work&& work) {
  if (is_running()) {
    queue_.enqueue(std::move(work));
    condition_.notify_one();
  } else {
    std::move(work).set_canceled();
  }
}

/*
void event_loop_executor_impl::printPartDetails(std::ostream& os) const {
  print(os, FMT_STRING("~{} handlers in queue"), queue_.size_approx());
}
*/

void EventLoopExecutorImpl::dispatch_all() {
  work w;

  while (queue_.try_dequeue(consumer_token_, w)) {
    std::move(w).set_value();
  }
}
} // namespace idle
