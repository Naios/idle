
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

#include <idle/core/context.hpp>
#include <idle/core/detail/context/context_impl.hpp>
#include <idle/core/detail/context/garbage_collector.hpp>
#include <idle/core/detail/service_deleter.hpp>

namespace idle {
namespace detail {
void service_deleter::on_unique(Service* current) noexcept {
  garbage_collector::on_service_unique(*current);
}

class async_deleter {
public:
  async_deleter(Service* current, WeakRefCounter counter)
    : current_(current)
    , counter_(std::move(counter)) {}

  async_deleter(async_deleter&&) = default;
  async_deleter(async_deleter const&) = delete;
  async_deleter& operator=(async_deleter&&) = default;
  async_deleter& operator=(async_deleter const&) = delete;

  ~async_deleter() noexcept {
    IDLE_ASSERT(!counter_);
  }

  void operator()() noexcept {
    if (!current_->state().isDestroyedUnsafe()) {
      current_->destroy();
    }

    current_->~Service();
    counter_.reset();
  }

  Service* current_;
  WeakRefCounter counter_;
};

void service_deleter::on_destruct(Service* current) noexcept {
  IDLE_ASSERT(current);

  if (current->state().isDestroyedUnsafe()) {
    // If the object was destroyed already we can destruct it in-place
    // this can happen if the parent referenced the child strongly and
    // was deallocated before the child.
    current->~Service();
    return;
  }

  ContextImpl& root = ContextImpl::from(current->root());
  IDLE_ASSERT(root.is_running_impl());

  // We can't guarantee here that the onDestroy() hook happens on the
  // event loop thread automatically. Thus we require that services destroy
  // their active children when being destroyed itself.
  //
  // The destruction must always happen asynchronously in respect to the
  // current callstack since service::destroy could be called underneath
  root.event_loop().async_post(async_deleter{current, //
                                             current->weakRefCounter()});
}
} // namespace detail
} // namespace idle
