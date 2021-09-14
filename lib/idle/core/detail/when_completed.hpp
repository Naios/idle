
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

#ifndef IDLE_CORE_DETAIL_WHEN_COMPLETED_HPP_INCLUDED
#define IDLE_CORE_DETAIL_WHEN_COMPLETED_HPP_INCLUDED

#include <memory>
#include <utility>
#include <idle/core/dep/continuable.hpp>

namespace idle {
namespace detail {
template <typename Promise>
class when_completed_frame
  : public std::enable_shared_from_this<when_completed_frame<Promise>> {

public:
  explicit when_completed_frame(Promise promise)
    : promise_(std::move(promise)) {}

  auto attach_one() {
    remaining_.fetch_add(1U, std::memory_order_acq_rel);

    return [me = this->shared_from_this()](auto&&...) mutable {
      me->complete();
    };
  }

  void complete() {
    auto const previous = remaining_.fetch_sub(1U, std::memory_order_acq_rel);
    IDLE_ASSERT(previous > 0);

    if (previous == 1U) {
      promise_.set_value();
    }
  }

private:
  Promise promise_;
  std::atomic<std::size_t> remaining_{1};
};

template <typename Range>
auto when_completed(Range&& range) {
  return make_continuable<void>(
      [range = std::forward<Range>(range)](auto&& promise) {
        using frame_t = when_completed_frame<
            std::remove_reference_t<decltype(promise)>>;

        auto frame = std::make_shared<frame_t>(
            std::forward<decltype(promise)>(promise));

        for (auto&& one : range) {
          std::move(one).next(frame->attach_one());
        }

        frame->complete();
      });
}
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_WHEN_COMPLETED_HPP_INCLUDED
