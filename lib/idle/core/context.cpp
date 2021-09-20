
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

#include <algorithm>
#include <idle/core/context.hpp>
#include <idle/core/detail/context/context_impl.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/rtti.hpp>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
static void initialize_statics() {
  detail::initialize_log_level();
}

int Context::run() {
  return ContextImpl::from(this)->run_impl();
}

Ref<Service> Context::lookup(Guid guid) const noexcept {
  return ContextImpl::from(this)->lookupImpl(guid);
}

continuable<> Context::update() {
  return ContextImpl::from(this)->do_update_system();
}

bool Context::is_on_event_loop() const noexcept {
  return ContextImpl::from(this)->is_on_event_loop_impl();
}

continuable<> Context::stop(int exit_code) {
  return ContextImpl::from(this)->stop_impl(exit_code);
}

Ref<Registry> Context::find(Interface::Id const& id) {
  return ContextImpl::from(this)->findImpl(id);
}

Ref<Context> Context::create() {
  initialize_statics();
  return make_ref<ContextImpl>();
}

bool Context::verify(std::ostream& os) noexcept {
  bool successful = true;
  for (Service& cluster : clusters(*this)) {
    if (!ServiceImpl::verifyCluster(os, cluster)) {
      successful = false;
    }
  }
  return successful;
}

bool Context::can_dispatch_inplace() const noexcept {
  ContextImpl const* const impl = ContextImpl::from(this);
  return impl->is_running_impl() && impl->is_on_event_loop_impl();
}

void Context::queue(work work) noexcept {
  ContextImpl::from(this)->queue_impl(std::move(work));
}

namespace internal {
void setInternalLog(InternalLogLevel level) noexcept {
#if IDLE_DETAIL_HAS_LOG_LEVEL < IDLE_DETAIL_LOG_LEVEL_DISABLED
  detail::set_internal_loglevel(static_cast<detail::loglevel>(level));
#else
  (void)level;
#endif
}
} // namespace internal
} // namespace idle
