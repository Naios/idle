
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

#include <queue>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <idle/core/context.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/service/detail/timer/timer_impl.hpp>
#include <idle/service/timer.hpp>

namespace idle {
template <typename Promise>
class timed_work_task_impl : public timed_work_task {
  Promise promise_;

public:
  explicit timed_work_task_impl(Promise&& promise)
    : promise_(std::move(promise)) {}

  void resolve() noexcept override {
    std::move(promise_)();
  }
};

continuable<> timer_impl::onStart() {
  return async([this] {
    boost::asio::io_context& context = io_context_->get();
    timer_.emplace(context);
    strand_.emplace(context);
  });
}

continuable<> timer_impl::onStop() {
  return async([this] {
    timer_->cancel();
    timer_.reset();
    strand_.reset();
  });
}

continuable<> timer_impl::wait_for_impl(Duration exact) {
  return [weak = weakOf(this), exact](auto&& promise) {
    // The timepoint when the continuable shall be resolved
    auto const deadline = clock_type::now() + exact;

    if (auto me = weak.lock()) {
      boost::asio::post(*me->strand_, [weak = std::move(weak), deadline,
                                       promise = std::forward<
                                           decltype(promise)>(
                                           promise)]() mutable {
        if (auto me = weak.lock()) {
          using work_t = timed_work_task_impl<std::decay_t<decltype(promise)>>;
          auto task = std::make_unique<work_t>(std::move(promise));
          me->deposit(timed_work{deadline, std::move(task)});
        }
      });
    }
  };
}

continuable<> timer_impl::wait_for_impl(Duration min, Duration max) {
  return [weak = weakOf(this), min, max](auto&& promise) mutable {
    if (auto me = weak.lock()) {
      auto now = clock_type::now();

      boost::asio::post(*me->strand_, [weak = std::move(weak), now, min, max,
                                       promise = std::forward<
                                           decltype(promise)>(
                                           promise)]() mutable {
        if (auto me = weak.lock()) {
          using work_t = timed_work_task_impl<std::decay_t<decltype(promise)>>;
          auto task = std::make_unique<work_t>(std::move(promise));

          std::uniform_int_distribution<std::size_t> dist(min.count(),
                                                          max.count());
          Duration const actual(dist(me->random_engine_));

          me->deposit(timed_work{now + actual, std::move(task)});
        }
      });
    }
  };
}

continuable<> timer_impl::wait_until_impl(TimePoint deadline) {
  return [weak = weakOf(this), deadline](auto&& promise) mutable {
    if (auto me = weak.lock()) {
      boost::asio::post(*me->strand_, [weak = std::move(weak), deadline,
                                       promise = std::forward<
                                           decltype(promise)>(
                                           promise)]() mutable {
        if (auto me = weak.lock()) {
          using work_t = timed_work_task_impl<std::decay_t<decltype(promise)>>;
          auto task = std::make_unique<work_t>(std::move(promise));
          me->deposit(timed_work{deadline, std::move(task)});
        }
      });
    }
  };
}

continuable<> timer_impl::wait_until_impl(TimePoint min, TimePoint max) {
  return [weak = weakOf(this), min, max](auto&& promise) mutable {
    if (auto me = weak.lock()) {
      boost::asio::post(*me->strand_, [weak = std::move(weak), min, max,
                                       promise = std::forward<
                                           decltype(promise)>(
                                           promise)]() mutable {
        if (auto me = weak.lock()) {
          using work_t = timed_work_task_impl<std::decay_t<decltype(promise)>>;
          auto task = std::make_unique<work_t>(std::move(promise));

          Duration const diff = max - min;

          std::uniform_int_distribution<std::size_t> dist(0, diff.count());
          Duration const actual(dist(me->random_engine_));

          me->deposit(timed_work{min + actual, std::move(task)});
        }
      });
    }
  };
}

void timer_impl::ready(boost::system::error_code const& error) {
  if (error == boost::asio::error::operation_aborted) {
    return;
  }

  boost::asio::post(*strand_, [weak = weakOf(this)]() mutable {
    if (auto me = weak.lock()) {
      IDLE_ASSERT(!me->queue_.empty());

      // Resolve all pending timed events
      auto const now = clock_type::now();
      while (!me->queue_.empty() && (me->queue_.top().deadline_ <= now)) {
        auto const& top = me->queue_.top();

        top.task_->resolve();
        me->queue_.pop();
      }

      // There are outstanding events which we have to reschedule
      if (!me->queue_.empty()) {
        auto const deadline = me->queue_.top().deadline_;
        me->schedule(deadline);
      }
    }
  });
}

void timer_impl::schedule(TimePoint deadline) {
  timer_->expires_at(deadline);

  timer_->async_wait([this](boost::system::error_code const& error) {
    // Set the promise ready
    ready(error);
  });
}

void timer_impl::deposit(timed_work&& work) {
  auto const deadline = work.deadline_;
  if (queue_.empty()) {
    queue_.push(std::move(work));
    schedule(deadline);
  } else if (queue_.top().deadline_ <= deadline) {
    // The deadline of the given task is later than the current one,
    // so just queue it.
    queue_.push(std::move(work));
  } else {
    IDLE_ASSERT(queue_.top().deadline_ > deadline);
    queue_.push(std::move(work));
    timer_->cancel();
    schedule(deadline);
  }
}
} // namespace idle
