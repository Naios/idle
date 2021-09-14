
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

#ifndef IDLE_SERVICE_DETAIL_TIMER_TIMER_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_TIMER_TIMER_IMPL_HPP_INCLUDED

#include <queue>
#include <random>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/io_context.hpp>
#include <idle/service/timer.hpp>

namespace idle {
struct timed_work_task {
  virtual ~timed_work_task() = default;

  virtual void resolve() noexcept = 0;
};

class timer_impl : public Timer, public Upcastable<timer_impl> {
  struct timed_work {
    TimePoint deadline_;
    std::unique_ptr<timed_work_task> task_;

    bool operator<(timed_work const& other) const noexcept {
      return deadline_ > other.deadline_;
    }
  };

public:
  explicit timer_impl(Inheritance parent)
    : Timer(std::move(parent)) {}

  continuable<> onStart() override;
  continuable<> onStop() override;

  continuable<> wait_for_impl(Duration exact);
  continuable<> wait_for_impl(Duration min, Duration max);

  continuable<> wait_until_impl(TimePoint deadline);
  continuable<> wait_until_impl(TimePoint min, TimePoint max);

private:
  void ready(boost::system::error_code const& error);
  void schedule(TimePoint deadline);
  void deposit(timed_work&& work);

  Dependency<IOContext> io_context_{*this};
  optional<boost::asio::basic_waitable_timer<clock_type>> timer_;
  optional<boost::asio::io_context::strand> strand_;
  std::default_random_engine random_engine_;
  std::priority_queue<timed_work> queue_;
};
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_TIMER_TIMER_IMPL_HPP_INCLUDED
