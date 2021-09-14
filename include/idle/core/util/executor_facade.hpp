
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

#ifndef IDLE_CORE_UTIL_EXECUTOR_FACADE_HPP_INCLUDED
#define IDLE_CORE_UTIL_EXECUTOR_FACADE_HPP_INCLUDED

#include <type_traits>
#include <utility>
#include <idle/core/async.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/use.hpp>

namespace idle {
namespace detail {
template <typename Callable>
struct work_wrap_t {
  void operator()() && noexcept {
    std::move(callable_)();
  }

  void operator()(exception_arg_t, exception_t exception) && noexcept {
    if (exception) {
      IDLE_DETAIL_UNREACHABLE();
    }
  }

  Callable callable_;
};

template <typename T>
auto make_wrap(T&& callable) {
  return work_wrap_t<std::decay_t<T>>{std::forward<T>(callable)};
}
} // namespace detail

/// Represents a boost::asio compatible executor_type without
/// publicly exposing boost internals, that passes work over to the origin.
///
/// You can specify your own executor as following by providing
/// the following methods:
/// ```
/// class my_executor : public idle::executor_facade<Parent> {
///   bool can_dispatch_inplace() const noexcept;
///
///   template<typename W>
///   void queue(W&& work) noexcept;
///
///   weak_ref_counter_t weakRefCounter() const noexcept;
/// };
/// ```
template <typename Parent>
class ExecutorFacade {
  using allocator_t = std::allocator<char>;

public:
  ExecutorFacade() noexcept {}

  template <typename Callable, typename Allocator = allocator_t>
  void post(Callable&& work, Allocator const& = {}) {
    Parent* const parent = static_cast<Parent*>(this);
    parent->queue(detail::make_wrap(std::forward<Callable>(work)));
  }

  template <typename Callable, typename Allocator = allocator_t>
  bool dispatch(Callable&& work, Allocator const& = {}) {
    Parent* const parent = static_cast<Parent*>(this);

    if (parent->can_dispatch_inplace()) {
      // If we are on the event loop dispatch the callable directly,
      // so we are not required to type erase the callable type.
      std::forward<Callable>(work)();
      return true;
    } else {
      parent->queue(detail::make_wrap(std::forward<Callable>(work)));
      return false;
    }
  }

  template <typename Callable, typename Allocator = allocator_t>
  void defer(Callable&& callable, Allocator const& = {}) {
    post(std::forward<Callable>(callable));
  }

  auto through_dispatch() noexcept {
    return [origin = weakOf(*static_cast<Parent*>(this))](auto&& work) {
      if (auto me = origin.lock()) {
        if (me->can_dispatch_inplace()) {
          std::forward<decltype(work)>(work)();
        } else {
          me->queue(detail::make_wrap(std::forward<decltype(work)>(work)));
        }
      } else {
        std::forward<decltype(work)>(work).set_canceled();
      }
    };
  }

  auto through_post() noexcept {
    return [parent = weakOf(*static_cast<Parent*>(this))](auto&& work) {
      if (auto me = parent.lock()) {
        me->queue(detail::make_wrap(std::forward<decltype(work)>(work)));
      } else {
        std::forward<decltype(work)>(work).set_canceled();
      }
    };
  }

  auto through_defer() noexcept {
    return through_post();
  }

  template <typename Callable>
  auto async_dispatch(Callable&& callable) {
    return async_on(std::forward<Callable>(callable), through_dispatch());
  }

  template <typename Callable>
  auto async_post(Callable&& callable) {
    return async_on(std::forward<Callable>(callable), through_post());
  }

  template <typename Callable>
  auto async_defer(Callable&& callable) {
    return async_on(std::forward<Callable>(callable), through_defer());
  }

  template <typename Actor, typename Callable>
  auto async_dispatch(Actor&& actor, Callable&& callable) {
    return async_on(wrap(std::forward<Actor>(actor),
                         std::forward<Callable>(callable)),
                    through_dispatch());
  }

  template <typename Actor, typename Callable>
  auto async_post(Actor&& actor, Callable&& callable) {
    return async_on(wrap(std::forward<Actor>(actor),
                         std::forward<Callable>(callable)),
                    through_post());
  }

  template <typename Actor, typename Callable>
  auto async_defer(Actor&& actor, Callable&& callable) {
    return async_on(wrap(std::forward<Actor>(actor),
                         std::forward<Callable>(callable)),
                    through_defer());
  }

  void on_work_started() const noexcept {
    // We never count the work in order to avoid cache-line ping-pong
  }
  void on_work_finished() const noexcept {
    // We never count the work in order to avoid cache-line ping-pong
  }

  friend bool operator==(ExecutorFacade const& left,
                         ExecutorFacade const& right) noexcept {
    return &left == &right;
  }

  friend bool operator!=(ExecutorFacade const& left,
                         ExecutorFacade const& right) noexcept {
    return !(left == right);
  }
};

} // namespace idle

#endif // IDLE_CORE_UTIL_EXECUTOR_FACADE_HPP_INCLUDED
