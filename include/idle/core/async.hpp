
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

#ifndef IDLE_CORE_ASYNC_HPP_INCLUDED
#define IDLE_CORE_ASYNC_HPP_INCLUDED

#include <type_traits>
#include <utility>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/detail/traits.hpp>
#include <idle/core/util/meta.hpp>

namespace idle {
namespace detail {
using cti::detail::base::decoration::invoke_callback;

template <typename Result>
struct weakly_trait {
  template <typename T, typename Callable, typename... Args>
  static result<Result> invoke(T&& weak, Callable&& callable, Args&&... args) {
    if (auto locked = weak.lock()) {
      return make_result(invoke_callback(std::forward<Callable>(callable),
                                         std::forward<Args>(args)...,
                                         std::move(locked)));
    } else {
      return cancel();
    }
  }
};

template <>
struct weakly_trait<void> {
  template <typename T, typename Callable, typename... Args>
  static result<> invoke(T&& weak, Callable&& callable, Args&&... args) {
    if (auto locked = weak.lock()) {
      invoke_callback(std::forward<Callable>(callable),
                      std::forward<Args>(args)..., std::move(locked));
      return make_result();
    } else {
      return cancel();
    }
  }
};

template <typename... A>
struct weakly_trait<result<A...>> {
  template <typename T, typename Callable, typename... Args>
  static result<A...> invoke(T&& weak, Callable&& callable, Args&&... args) {
    if (auto locked = weak.lock()) {
      return invoke_callback(std::forward<Callable>(callable),
                             std::forward<Args>(args)..., std::move(locked));
    } else {
      return cancel();
    }
  }
};

template <typename... A>
struct weakly_continuable_trait {
  template <typename T, typename Callable, typename... Args>
  static auto invoke(T&& weak, Callable&& callable, Args&&... args) {
    return make_continuable<A...>([weak = std::forward<T>(weak),
                                   callable = std::forward<Callable>(callable),
                                   args = std::make_tuple(std::forward<Args>(
                                       args)...)](auto&& promise) mutable {
      if (auto locked = weak.lock()) {
        using promise_t = decltype(promise);

        unpack(
            [&](auto&&... args) {
              invoke_callback(std::move(callable),
                              std::forward<decltype(args)>(args)...,
                              std::move(locked))
                  .next(std::forward<promise_t>(promise));
            },
            std::move(args));
      } else {
        promise.set_canceled();
      }
    });
  }
};

template <typename Data>
struct weakly_trait<continuable_base<Data, cti::signature_arg_t<>>>
  : weakly_continuable_trait<void> {};

template <typename Data, typename... Args>
struct weakly_trait<continuable_base<Data, cti::signature_arg_t<Args...>>>
  : weakly_continuable_trait<Args...> {};

template <typename T, typename = void>
struct finalized_result : std::common_type<T> {};
template <typename T>
struct finalized_result<
    T, std::enable_if_t<is_continuable<std::decay_t<T>>::value>>
  : std::common_type<decltype(std::declval<std::decay_t<T>>().finish())> {};

template <typename Weak, typename Callback>
class weakly_callable {
public:
  explicit weakly_callable(Weak weak, Callback callback)
    : weak_(std::move(weak))
    , callback_(std::move(callback)) {}

  template <typename... Args>
  auto operator()(Args&&... args) & {
    using result_t = decltype(invoke_callback(std::declval<Callback>(),
                                              std::declval<Args>()...,
                                              std::declval<Weak&>().lock()));

    using finalized_t = typename finalized_result<result_t>::type;
    using weakly_trait_t = weakly_trait<finalized_t>;

    return weakly_trait_t::invoke(weak_, callback_,
                                  std::forward<Args>(args)...);
  }

  template <typename... Args>
  auto operator()(Args&&... args) && {
    using result_t = decltype(invoke_callback(std::declval<Callback>(),
                                              std::declval<Args>()...,
                                              std::declval<Weak&&>().lock()));

    using finalized_t = typename finalized_result<result_t>::type;
    using weakly_trait_t = weakly_trait<finalized_t>;

    return weakly_trait_t::invoke(std::move(weak_), std::move(callback_),
                                  std::forward<Args>(args)...);
  }

  Weak weak_;
  Callback callback_;
};
} // namespace detail

template <typename Ref, typename Callable>
auto weakly(Ref&& ref, Callable&& callable) {
  using type_t = detail::weakly_callable<std::decay_t<Ref>,
                                         std::decay_t<Callable>>;

  return type_t(std::forward<Ref>(ref), std::forward<Callable>(callable));
}
} // namespace idle

// spawn
// through_this

// launch

#endif // IDLE_CORE_ASYNC_HPP_INCLUDED
