
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

#ifndef IDLE_CORE_DETAIL_ASYNC_FINALIZER_HPP_INCLUDED
#define IDLE_CORE_DETAIL_ASYNC_FINALIZER_HPP_INCLUDED

#include <continuable/continuable-primitives.hpp>
#include <idle/core/api.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/exception.hpp>
#include <idle/core/locality.hpp>

namespace idle {
namespace detail {
template <typename... Args>
class async_finalizer {
public:
  async_finalizer()
#ifdef IDLE_HAS_DYNAMIC_LINKING
    : locality_(this_locality())
#endif //  IDLE_HAS_DYNAMIC_LINKING
  {
  }

  async_finalizer(async_finalizer&&) = default;
  async_finalizer(async_finalizer const&) = delete;
  async_finalizer& operator=(async_finalizer&&) = default;
  async_finalizer& operator=(async_finalizer const&) = delete;
  ~async_finalizer() = default;

  void operator()(Args... /*args*/) && {}

  void operator()(cti::exception_arg_t, cti::exception_t exception) && {
    if (bool(exception)) {
      try {
        std::rethrow_exception(exception);
      } catch (AsyncException const& e) {
        // TODO Report the unhandled exception to the logger (ignore it for now)
        (void)e;
      } catch (std::exception const& e) {
        char const* reason = e.what();
        (void)reason;
        IDLE_DETAIL_TRAP();
      } catch (...) {
        IDLE_DETAIL_TRAP();
      }
    }
  }

  void set_value(Args... args) {
    std::move (*this)(std::forward<Args>(args)...);
  }

  void set_exception(cti::exception_t exception) {
    // NOLINTNEXTLINE(hicpp-move-const-arg, performance-move-const-arg)
    std::move (*this)(cti::exception_arg_t{}, std::move(exception));
  }

  void set_canceled() {
    std::move (*this)(cti::exception_arg_t{}, cti::exception_t{});
  }

private:
#ifdef IDLE_HAS_DYNAMIC_LINKING
  Ref<Locality> locality_;
#endif // IDLE_HAS_DYNAMIC_LINKING
};
} // namespace detail
} // namespace idle

#endif // IDLE_CORE_DETAIL_ASYNC_FINALIZER_HPP_INCLUDED
