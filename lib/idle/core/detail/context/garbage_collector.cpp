
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
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>

namespace idle {
namespace detail {
static virtual_cb* control_block_of(Service& current) noexcept {
  return reference_counted_trait::get(&current);
}

void garbage_collector::on_service_init(Service& current) noexcept {
  IDLE_ASSERT(current.root().is_on_event_loop());
}

void garbage_collector::on_service_start(Service& current) noexcept {
  IDLE_ASSERT(current.root().is_on_event_loop());

  if (auto* cb = control_block_of(current)) {
    cb->increase(strong_arg_t{});
  }
}

void garbage_collector::on_service_stop(Service& current) noexcept {
  IDLE_ASSERT(current.root().is_on_event_loop());

  if (auto* cb = control_block_of(current)) {
    cb->decrease(strong_arg_t{});
  }

  /*if (auto* cb = control_block_of(current)) {
    if (cb->count_unsafe(strong_arg_t{}) == 1U) {
      current.root().event_loop().post([weak = weak_of(current)] {
        // Destroy the service asynchronously
        if (auto locked = weak.lock()) {
          if (control_block_of(*locked)->count_unsafe(strong_arg_t{}) == 1U) {
            IDLE_DETAIL_LOG_DEBUG("Destroying unreferenced service {}",
                                   *locked);

            locked->destroy();
          }
        }
      });
    }
  }*/
}

void garbage_collector::on_service_destroy(Service& current) noexcept {
  IDLE_ASSERT(current.root().is_on_event_loop());
}

static void immediate_stop(Context& root, Service& current) noexcept {
  IDLE_ASSERT(root.is_on_event_loop());

  if (current.state().isStarting() || current.state().isRunning()) {
    IDLE_DETAIL_LOG_DEBUG("Stopping unreferenced service {} ({}, {})", current,
                          current.stats().state(), current.stats().usage());

    ServiceImpl::set_override(current, override_t::none);

    Service& head = get_cluster_head_of(current);
    ContextImpl::from(root) //
        .associated_scheduler()
        .on_service_stop_request(head);
  }
}

void garbage_collector::on_service_unique(Service& current) noexcept {
  if (!current.state().isInitializedUnsafe()) {
    // Never do anything if the service is not initialized
    return;
  }
  if (!control_block_of(current)) {
    // In case the service has a static lifetime do nothing
    return;
  }

  Context& root = current.root();

  if (root.is_on_event_loop()) {
    immediate_stop(root, current);
  } else {
    root.event_loop().post([weak = weakOf(current)] {
      if (auto current = weak.lock()) {

        auto* const cb = control_block_of(*current);
        IDLE_ASSERT(cb);
        if (cb->load(strong_arg_t{}) == 2U) {
          immediate_stop(current->root(), *current);
        }
      }
    });
  }
}
} // namespace detail
} // namespace idle
