
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

#ifndef IDLE_CORE_DETAIL_CONTEXT_CONTEXT_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_CONTEXT_CONTEXT_IMPL_HPP_INCLUDED

#include <utility>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/detail/context/event_loop_executor_impl.hpp>
#include <idle/core/detail/context/registry_impl.hpp>
#include <idle/core/detail/context/scheduler.hpp>
#include <idle/core/detail/unordered_map.hpp>
#include <idle/core/parts/container.hpp>
#include <idle/core/parts/listener.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/flat_set.hpp>
#include <idle/core/util/upcastable.hpp>

namespace idle {
class DeclaredServicesContainer : public Container {
public:
  using Container::Container;

  void onPartInit() noexcept override;
};

class ContextImpl final : public Context,
                          protected RegistryManager,
                          protected DeclaredServicesContainer,
                          protected Scheduler,
                          protected EventLoopExecutorImpl,
                          protected detail::Cluster,
                          public Upcastable<ContextImpl> {

  friend Context;

public:
  ContextImpl()
    : RegistryManager(*static_cast<Context*>(this))
    , DeclaredServicesContainer(*static_cast<Context*>(this))
    , Scheduler(*static_cast<Context*>(this))
    , EventLoopExecutorImpl(*static_cast<Context*>(this)) {
    detail::setCluster(*this, *static_cast<Cluster*>(this));
  }

  using Context::Context;
  using Context::refCounter;
  using Context::weakRefCounter;

  ContextImpl& owner() noexcept override {
    return *this;
  }
  ContextImpl const& owner() const noexcept override {
    return *this;
  }

  int run_impl();

  bool is_on_event_loop_impl() const noexcept;

  continuable<> stop_impl(int exit_code);

  RegistryManager& anchor() noexcept override {
    return *this;
  }
  RegistryManager const& anchor() const noexcept override {
    return *this;
  }

  Scheduler& associated_scheduler() noexcept {
    return *this;
  }

  Ref<Service> lookupImpl(Guid guid) const noexcept;

  bool is_running_impl() const noexcept {
    return EventLoopExecutorImpl::is_running();
  }

  void queue_impl(work&& work) {
    EventLoopExecutorImpl::queue(std::move(work));
  }

  class id_recycler {
  public:
    id_recycler() = default;

    Guid::Low allocate();
    void recycle(Guid::Low guid);

  private:
    Guid::Low current_{Guid::min_low()};
    FlatSet<Guid::Low> recycled_;
  };

  Guid::Low allocate_guid(Service& current) noexcept;
  void recycle_guid(Service& current, Guid::Low low) noexcept;

  void call_on_service_init(Service& current) const noexcept;
  void call_on_service_destroy(Service& current) const noexcept;
  void call_on_service_changed(Service& current, Phase from,
                               Phase to) const noexcept;
  void call_on_usage_connect(Usage& current) const noexcept;
  void call_on_usage_disconnect(Usage& current) const noexcept;

protected:
  void onInit() override;
  void onDestroy() override;

  void onChildInit(Service& child) override;
  void onChildDestroy(Service& child) override;

  Ref<Registry> listener_;
  std::atomic<int> exit_code_{EXIT_SUCCESS};
  id_recycler recycler_;
  detail::unordered_map<Guid::Low, Service*> services_;
};
} // namespace idle

#endif // IDLE_CORE_DETAIL_CONTEXT_CONTEXT_IMPL_HPP_INCLUDED
