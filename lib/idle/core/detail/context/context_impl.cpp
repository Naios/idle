
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

#include <idle/core/detail/context/context_impl.hpp>
#include <idle/core/detail/declare_impl.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/parts/listener.hpp>
#include <idle/core/util/thread_name.hpp>

namespace idle {
void DeclaredServicesContainer::onPartInit() noexcept {
  Container::onPartInit();

  for (detail::ServiceDeclaration const& declared :
       detail::declared_services()) {

    Ref<Service> instantiated = declared(*this);
    instantiated->init();

    IDLE_DETAIL_LOG_DEBUG("Instantiated a declared service from executable: {}",
                          *instantiated);
  }
}

int ContextImpl::run_impl() {
  set_this_thread_name("idle::event_loop");

  IDLE_DETAIL_LOG_DEBUG("context::run() invoked");

  event_loop().post([this] {
    if (!state().isRunning()) {
      ServiceImpl::start_root(*this);
    }
  });

  prepare_ev();
  run_ev();
  int const ec = exit_code_.load(std::memory_order_acquire);

  IDLE_DETAIL_LOG_DEBUG("context::run() has finished with exit code {}", ec);
  return ec;
}

bool ContextImpl::is_on_event_loop_impl() const noexcept {
  return EventLoopExecutorImpl::is_on_event_loop();
}

continuable<> ContextImpl::stop_impl(int exit_code) {
  return event_loop().async_dispatch([=] {
    exit_code_ = exit_code;
    return static_cast<Service*>(this)->stop();
  });
}

Ref<Service> ContextImpl::lookupImpl(Guid guid) const noexcept {
  IDLE_ASSERT(Context::is_on_event_loop());

  if (guid.isPart()) {
    return {};
  }

  auto const itr = services_.find(guid.low());
  if (itr == services_.end()) {
    return {};
  }

  if (guid.high() == Guid::min_high()) {
    IDLE_ASSERT(itr->second->guid() == guid);
    return refOf(itr->second);
  }

  for (Service& child : cluster_members(*itr->second).next()) {
    if (child.guid() == guid) {
      return refOf(child);
    }
  }

  return {};
}

void ContextImpl::onChildInit(Service& child) {
  if (child == *this) {
    return;
  }

  if (child.parent() == *static_cast<RegistryManager*>(this)) {
    RegistryManager::onChildInit(child);
  } else if (child.parent() == *static_cast<DeclaredServicesContainer*>(this)) {
    DeclaredServicesContainer::onChildInit(child);
  } else {
    IDLE_ASSERT(child.parent() == *static_cast<Context*>(this));
    Context::onChildInit(child);
  }
}

void ContextImpl::onChildDestroy(Service& child) {
  if (child == *this) {
    return;
  }

  if (child.parent() == *static_cast<RegistryManager*>(this)) {
    RegistryManager::onChildDestroy(child);
  } else if (child.parent() == *static_cast<DeclaredServicesContainer*>(this)) {
    DeclaredServicesContainer::onChildDestroy(child);
  } else {
    IDLE_ASSERT(child.parent() == *static_cast<Context*>(this));
    Context::onChildDestroy(child);
  }
}

Guid::Low ContextImpl::id_recycler::allocate() {
  if (recycled_.empty()) {
    IDLE_ASSERT((current_ != Guid::max_low()) && "GUID overflow!");
    return current_++;
  } else {
    auto const itr = std::prev(recycled_.end());
    auto const id = *itr;
    recycled_.erase(itr);
    return id;
  }
}

void ContextImpl::id_recycler::recycle(Guid::Low recycled) {
  recycled_.insert(recycled);

  while (!recycled_.empty()) {
    auto const itr = std::prev(recycled_.end());
    if (*itr != current_ - 1) {
      break;
    }
    recycled_.erase(itr);
    --current_;
  }
}

Guid::Low ContextImpl::allocate_guid(Service& current) noexcept {
  Guid::Low low = recycler_.allocate();
  IDLE_ASSERT(services_.find(low) == services_.end());
  services_.insert(std::make_pair(low, std::addressof(current)));
  return low;
}

void ContextImpl::recycle_guid(Service& current, Guid::Low low) noexcept {
  IDLE_ASSERT(services_.find(low) != services_.end());
  IDLE_ASSERT(*(services_.find(low)->second) == current);

  services_.erase(low);
  recycler_.recycle(low);
}

void ContextImpl::call_on_service_init(Service& current) const noexcept {
  IDLE_ASSERT(listener_);

#ifndef IDLE_HAS_NO_SERVICE_LISTENER
  for (Interface& inter : listener_->interfaces()) {
    Listener& li = cast<Listener>(inter);
    if (li.filter_.contains(Listener::Event::OnServiceInit) &&
        inter.owner().state().isRunning()) {
      li.onServiceInit(current);
    }
  }
#endif
}

void ContextImpl::call_on_service_destroy(Service& current) const noexcept {
  IDLE_ASSERT(listener_);

#ifndef IDLE_HAS_NO_SERVICE_LISTENER
  for (Interface& inter : listener_->interfaces()) {
    Listener& li = cast<Listener>(inter);
    if (li.filter_.contains(Listener::Event::OnServiceDestroy) &&
        inter.owner().state().isRunning()) {
      li.onServiceDestroy(current);
    }
  }
#endif
}

void ContextImpl::call_on_service_changed(Service& current, Phase from,
                                          Phase to) const noexcept {
  IDLE_ASSERT(listener_);

#ifndef IDLE_HAS_NO_SERVICE_LISTENER
  for (Interface& inter : listener_->interfaces()) {
    Listener& li = cast<Listener>(inter);
    if (li.filter_.contains(Listener::Event::OnServiceChanged) &&
        inter.owner().state().isRunning()) {
      li.onServiceChanged(current, from, to);
    }
  }
#endif
}

void ContextImpl::call_on_usage_connect(Usage& current) const noexcept {
  IDLE_ASSERT(listener_);

#ifndef IDLE_HAS_NO_SERVICE_LISTENER
  for (Interface& inter : listener_->interfaces()) {
    Listener& li = cast<Listener>(inter);
    if (li.filter_.contains(Listener::Event::OnUsageConnected) &&
        inter.owner().state().isRunning()) {
      li.onUsageConnected(current);
    }
  }
#endif
}

void ContextImpl::call_on_usage_disconnect(Usage& current) const noexcept {
  IDLE_ASSERT(listener_);

#ifndef IDLE_HAS_NO_SERVICE_LISTENER
  for (Interface& inter : listener_->interfaces()) {
    Listener& li = cast<Listener>(inter);
    if (li.filter_.contains(Listener::Event::OnUsageDisconnected) &&
        inter.owner().state().isRunning()) {
      li.onUsageDisconnect(current);
    }
  }
#endif
}

void ContextImpl::onInit() {
  listener_ = this->find<Listener>();
}

void ContextImpl::onDestroy() {
  Context::onPartDestroy();
  DeclaredServicesContainer::onPartDestroy();
  RegistryManager::onPartDestroy();

  listener_ = {};
}
} // namespace idle
