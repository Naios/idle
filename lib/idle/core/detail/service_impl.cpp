
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

#include <exception>
#include <limits>
#include <boost/graph/reverse_graph.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/context/context_impl.hpp>
#include <idle/core/detail/context/garbage_collector.hpp>
#include <idle/core/detail/context/registry_impl.hpp>
#include <idle/core/detail/context/scheduler.hpp>
#include <idle/core/detail/graph/dfs.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/external/boost/graph.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/iterators.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/bitset.hpp>

/* # Algorithm explanation
 *
 * Outer dependencies represent missing dependencies outside of the cluster
 * with a direct connected edge and one implicit edge to the root if there
 * is no direct edge connection.
 *
 * Inner dependencies represent the cluster children that are not running yet.
 */

namespace idle {
static bool has_implicit_root_edge(Service& current) noexcept {
  return !current.isRoot() && !current.parent().owner().isRoot();
}

static bool are_imports_satisfied(Service const& me) noexcept {
  for (Import const& e : me.imports()) {
    if (!e.isSatisfied()) {
      return false;
    }
  }
  return true;
}

template <typename Iterator>
static decltype(auto) last_of(Iterator begin, Iterator end) {
  IDLE_ASSERT(begin != end);

  for (;;) {
    auto const prev = begin;
    ++begin;

    if (begin == end) {
      return *prev;
    }
  }
}

bool is_cluster_head(Service const& member) noexcept {
  return isa<Export>(member.parent());
}

Service& get_cluster_head_of(Service& member) noexcept {
  auto const parents = cluster_parent_services(member);
  return last_of(parents.begin(), parents.end());
}

template <typename T>
void chain_promise(promise<>& promise, T&& add) {
  if (promise) {
    promise = split(std::move(promise), std::forward<T>(add));
  } else {
    promise = std::forward<T>(add);
  }
}

bool ServiceImpl::is_in_same_cluster(Service& first, Service& second) noexcept {
  return first.cluster_ == second.cluster_;
}

void ServiceImpl::interface_init(Interface& self, Context& root) noexcept {
  self.registry_entry_ = root.find(self.type());
}

void ServiceImpl::interface_publish(Interface& self) noexcept {
  if (!self.owner().state().isHidden()) {
    IDLE_DETAIL_LOG_DEBUG("Adding interface {} of service {} to its registry",
                          self, self.owner());

    RegistryImpl::from(*self.registry_entry_).onInterfaceCreate(self);
  } else {
    IDLE_DETAIL_LOG_DEBUG(
        "Hidden interface {} of service {} is not added to a registry!", self,
        self.owner());
  }
}

void ServiceImpl::interface_revoke(Interface& self) noexcept {
  if (!self.owner().state().isHidden()) {
    IDLE_DETAIL_LOG_DEBUG(
        "Removing interface {} of service {} from its registry", self,
        self.owner());

    RegistryImpl::from(*self.registry_entry_).onInterfaceDestroy(self);
  }
}

void ServiceImpl::interface_destroy(Interface& self) noexcept {
  self.registry_entry_.reset();
}

void ServiceImpl::used_destroy(Usage& u, Export& exp) noexcept {
  u.onUsedDestroy(exp);
}

void ServiceImpl::used_started(Usage& u, Export& exp) noexcept {
  u.onUsedStarted(exp);
}

bool ServiceImpl::verifyCluster(std::ostream& os, Service& head) noexcept {
  IDLE_ASSERT(is_cluster_head(head));

  ClusterDependencyGraph const graph(head.root(), graph_view);

  {
    std::size_t influencing = 0;
    if (head.state().isManual()) {
      ++influencing;
    }

    for (Edge const& e : out_edges(&head, graph)) {
      Service* const out = target(e, graph);

      if (is_cluster_pushed(*out)) {
        ++influencing;
      }
    }

    if (head.cluster_->pushes_ != influencing) {
      fmt::print(os,
                 FMT_STRING("Cluster '{}' (started: {}) has {} pushes but "
                            "expected {}!\n"),
                 head, head.state().isManual(), head.cluster_->pushes_,
                 influencing);

      for (Edge const& e : out_edges(&head, graph)) {
        Service* const out = target(e, graph);

        fmt::print(os, FMT_STRING("> {}: {}\n"), *out, out->cluster_->pushes_);
      }

      return false;
    }
  }

  return true;
}

bool ServiceImpl::try_use(Service const& me) noexcept {
  auto& uses = me.uses_;

  auto const start = uses.load(std::memory_order_relaxed);
  if (auto strong = start) {
    while (!uses.compare_exchange_weak(strong, strong + 1,
                                       std::memory_order_release,
                                       std::memory_order_relaxed)) {
      if (strong == 0) {
        return false;
      }
    }

    IDLE_ASSERT(me.state().isUsable());
    return true;
  } else {
    return false;
  }
}

void ServiceImpl::inc_use(Service const& me) noexcept {
  IDLE_ASSERT(me.state().isUsable());

  auto const previous = me.uses_.fetch_add(1U, std::memory_order_relaxed);
  (void)previous;
  IDLE_ASSERT(previous > 0);
}

void ServiceImpl::dec_use(Service const& me) noexcept {
  IDLE_ASSERT(me.state().isUsable());

  auto const previous = me.uses_.fetch_sub(1U, std::memory_order_relaxed);
  (void)previous;
  IDLE_ASSERT(previous > 1);

  Service& mut = const_cast<Service&>(me);
  if (has_no_external_uses(mut, previous - 1)) {
    if (me.root().is_on_event_loop()) {
      on_outgoing_reached_no_external_use(mut, previous - 1);
    } else {
      mut.root().event_loop().post([previous, weak = weakOf(mut)]() mutable {
        if (auto me = weak.lock()) {
          if (me->state().isRunning()) {
            on_outgoing_reached_no_external_use(*me, previous - 1);
          }
        }
      });
    }
  }
}

continuable<> ServiceImpl::request_start(Service& me, Reason reason) {
  Service& head = get_cluster_head_of(me);
  return head.root().event_loop().async_post(
      weakly(weakOf(head), [reason](auto&& me) mutable {
        return make_continuable<void>([me = std::forward<decltype(me)>(me),
                                       reason](auto&& promise) mutable {
          IDLE_ASSERT(me->root().is_on_event_loop());

          if (!me->state().isInitializedUnsafe()) {
            promise.set_canceled();
            return;
          }

          if (reason == Reason::Manual) {
            set_override(*me, override_t::start);
          }

          if (is_cluster_running(get_cluster_head_of(*me))) {
            std::forward<decltype(promise)>(promise)();
          } else {
            ContextImpl::from(me->root())
                .associated_scheduler()
                .on_service_start_request(*me);

            transition_target_set(*me, target_t::start);

            chain_promise(me->cluster_->promise_,
                          std::forward<decltype(promise)>(promise));
          }
        });
      }));
}

continuable<> ServiceImpl::request_stop(Service& me, Reason reason) {
  Service& head = get_cluster_head_of(me);
  return head.root().event_loop().async_post(
      weakly(weakOf(head), [reason](auto&& me) mutable {
        return make_continuable<void>([me = std::forward<decltype(me)>(me),
                                       reason](auto&& promise) mutable {
          IDLE_ASSERT(me->root().is_on_event_loop());

          if (!me->state().isInitializedUnsafe()) {
            promise.set_canceled();
            return;
          }

          set_override(*me, override_t::none);

          if (is_cluster_stopped(*me)) {
            std::forward<decltype(promise)>(promise)();
          } else {
            ContextImpl::from(me->root())
                .associated_scheduler()
                .on_service_stop_request(*me);

            transition_target_set(*me, target_t::stop);

            chain_promise(me->cluster_->promise_,
                          std::forward<decltype(promise)>(promise));
          }
        });
      }));
}

continuable<> ServiceImpl::request_observe(Service& me) {
  Service& head = get_cluster_head_of(me);
  return head.root().event_loop().async_post(
      weakly(weakOf(head), [](auto&& me) mutable {
        return make_continuable<void>(
            [me = std::forward<decltype(me)>(me)](auto&& promise) mutable {
              IDLE_ASSERT(me->root().is_on_event_loop());

              if (!me->state().isInitializedUnsafe()) {
                promise.set_canceled();
                return;
              }

              chain_promise(me->cluster_->promise_,
                            std::forward<decltype(promise)>(promise));
            });
      }));
}

continuable<> ServiceImpl::do_start_eager(Service& me) {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isStopped() || me.state().isLocked());
  IDLE_ASSERT(me.isRoot() || me.root().state().isRunning());
  IDLE_ASSERT(me.isRoot() || !isa<Export>(me.parent()) ||
              me.parent().owner().state().isRunning());
  IDLE_ASSERT(me.state().isStartable());
  IDLE_ASSERT(are_imports_satisfied(me));

  if (!me.state().isLocked()) {
    do_lock_cluster(get_cluster_head_of(me));
  }

  do_transition_phase(me, phase_t::locked, phase_t::starting);

  IDLE_CHECK(me.uses_.fetch_add(1U, std::memory_order_relaxed) == 0);

  call_on_inner_outgoing_increment(me);

  auto on_start_hook = me.onStart();

  // Post start actions called after on_start
  auto post_start_actions = [&]() mutable {
    IDLE_ASSERT(me.root().is_on_event_loop());

    do_transition_phase(me, phase_t::starting, phase_t::running);

    call_on_outer_delayed_outgoing_increment(me);
    call_on_required_decrement(me);

    if (!shall_cluster_stop(me)) {
      if (is_cluster_head(me)) {
        // If the cluster head has been started this means that the
        // whole cluster is ready now and can be finalized.
        do_finalize_cluster(me);
      }
    } else {
      IDLE_ASSERT(me.state().isStoppable());

      ContextImpl::from(me.root()).associated_scheduler().on_service_stoppable(
          me);
    }
  };

  if (on_start_hook.is_ready()) {
    result<> res = std::move(on_start_hook).unpack();
    if (res.is_exception()) {
      return make_exceptional_continuable<void>(res.get_exception());
    }
    IDLE_ASSERT(res.is_value());

    post_start_actions();

    return make_ready_continuable();
  } else {
    return std::move(on_start_hook)
        .then(std::move(post_start_actions),
              me.root().event_loop().through_dispatch());
  }
}

continuable<> ServiceImpl::do_stop_eager(Service& me) {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isStopping());
  IDLE_ASSERT(!me.uses_.load(std::memory_order_relaxed));

  call_on_required_increment(me);

  auto on_stop_hook = me.onStop();

  // Post stop actions called after on_stop
  auto post_stop_actions = [me = &me] {
    IDLE_ASSERT(me);
    IDLE_ASSERT(me->root().is_on_event_loop());

    call_on_inner_outgoing_decrement(*me);

    do_transition_phase(*me, phase_t::stopping, phase_t::pending);

    call_on_cluster_running_decrement(*me);
  };

  if (on_stop_hook.is_ready()) {
    result<> res = std::move(on_stop_hook).unpack();
    if (res.is_exception()) {
      return make_exceptional_continuable<void>(res.get_exception());
    }
    IDLE_ASSERT(res.is_value());

    post_stop_actions();

    return make_ready_continuable();
  } else {
    return std::move(on_stop_hook)
        .then(std::move(post_stop_actions),
              me.root().event_loop().through_dispatch());
  }
}

void ServiceImpl::do_handle_exception(Service& me, Service& dependency,
                                      std::exception_ptr const& e) {
  (void)me;
  (void)dependency;
  try {
    std::rethrow_exception(e);
  } catch (std::exception const& ex) {
    (void)ex;
    IDLE_DETAIL_UNREACHABLE();
  } catch (...) {
    IDLE_DETAIL_UNREACHABLE();
  }
}

bool ServiceImpl::try_set_service_stopping(Service& current) {
  auto& uses = current.uses_;

  auto strong = uses.load(std::memory_order_relaxed);
  if (strong == 1) {
    while (!uses.compare_exchange_weak(strong, 0, std::memory_order_release,
                                       std::memory_order_relaxed)) {

      IDLE_ASSERT(strong != 0); // This should be impossible

      if (strong > 1) {
        return false;
      }
    }

    do_transition_phase(current, phase_t::running, phase_t::stopping);
    return true;
  } else {
    return false;
  }
}

void ServiceImpl::start_root(Context& root) noexcept {
  IDLE_DETAIL_LOG_DEBUG("Starting the root service!");

  set_override(root, override_t::start);

  continuable<> hook = do_start_eager(root);
  IDLE_ASSERT(hook.is_ready());

  result<> result = std::move(hook).unpack();
  IDLE_ASSERT(result.is_value());
}

bool ServiceImpl::do_release_usage(Usage& u) noexcept {
  IDLE_ASSERT(u.used().owner().root().is_on_event_loop());

  return u.requestRelease();
}

void ServiceImpl::on_import_satisfied(Service& me, Import& imp) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.parts_.contains(imp));

  on_outer_required_decrement(me);
}

void ServiceImpl::on_import_unsatisfied(Service& me, Import& imp) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.parts_.contains(imp));

  on_outer_required_increment(me);
}

bool ServiceImpl::is_cluster_startable(Service const& head) noexcept {
  IDLE_ASSERT(is_cluster_head(head));
  IDLE_ASSERT(head.root().is_on_event_loop());

  return head.cluster_->outer_deps_missing_ == 0U &&
         head.cluster_->running_ == 0U && !head.state().isLocked();
}

bool ServiceImpl::is_cluster_stoppable(Service const& head) noexcept {
  IDLE_ASSERT(is_cluster_head(head));
  return head.state().isStoppable();
}

bool ServiceImpl::is_cluster_pushed(Service const& me) noexcept {
  return me.cluster_->pushes_ != 0U;
}

bool ServiceImpl::is_cluster_pulled(Service const& me) noexcept {
  return me.cluster_->pulls_ != 0U;
}

bool ServiceImpl::is_cluster_running(Service const& head) noexcept {
  IDLE_ASSERT(is_cluster_head(head));
  return head.state().isRunning();
}

bool ServiceImpl::is_cluster_stopped(Service const& member) noexcept {
  return member.cluster_->running_ == 0U;
}

void ServiceImpl::do_lock_cluster(Service& head) noexcept {
  IDLE_ASSERT(head.root().is_on_event_loop());
  IDLE_ASSERT(is_cluster_head(head));

  IDLE_DETAIL_LOG_DEBUG("Locking cluster of {}", head);

  for (Service& current : cluster_members(head)) {
    detail::garbage_collector::on_service_start(current);
    do_transition_phase(current, phase_t::stopped, phase_t::locked);

    call_on_outer_outgoing_increment(current);

    for (Import& imp : current.imports()) {
      imp.onImportLock();
    }
  }

  for (Interface& inter : head.interfaces()) {
    RegistryImpl::from(*inter.registry_entry_).onInterfaceLock(inter);
  }

  IDLE_DETAIL_LOG_DEBUG("Configuring cluster of {}", head);

  // Start the configuration backwards from the cluster head
  for (Service& current : cluster_members(head)) {
    IDLE_DETAIL_LOG_DEBUG("Calling service::on_setup() of {}", current);
    IDLE_ASSERT(current.state().isLocked());

    try {
      // TODO Exception handling is not correctly implemented as of now!
      current.onSetup();
    } catch (std::exception const& e) {
      IDLE_DETAIL_LOG_ERROR("Service {} threw an exception during setup:\n{}",
                            current, e.what());

      char const* msg = e.what();
      (void)msg;
      IDLE_DETAIL_UNREACHABLE();
    } catch (...) {
      IDLE_DETAIL_LOG_ERROR(
          "Service {} threw an unknown exception during setup!", current);

      IDLE_DETAIL_UNREACHABLE();
    }
  }

  for (Service& current : cluster_members(head)) {
    call_on_cluster_running_increment(current);
  }
}

void ServiceImpl::do_finalize_cluster(Service& head) {
  IDLE_ASSERT(!shall_cluster_stop(head));

  do_mark_cluster_for_start(head, false);

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_DEBUG
  if (head.interfaces()) {
    IDLE_DETAIL_LOG_DEBUG("Publishing interfaces of cluster {}", head);
  }
#endif

  // Only the interfaces of the cluster head are published
  for (Interface& inter : head.interfaces()) {
    IDLE_DETAIL_LOG_TRACE("Publishing interface {} of cluster {}", inter, head);

    RegistryImpl::from(*inter.registry_entry_).onInterfaceStart(inter);
  }

  transition_target_complete_if(head, target_t::start);
}

void ServiceImpl::do_unlock_cluster(Service& head) {
  IDLE_ASSERT(head.root().is_on_event_loop());
  IDLE_ASSERT(is_cluster_head(head));
  IDLE_ASSERT(is_in_phase(head, phase_t::pending));
  IDLE_ASSERT(head.isRoot() || head.root().state().isRunning());
  IDLE_ASSERT(head.cluster_->running_ == 0U);

  IDLE_DETAIL_LOG_DEBUG("Unlocking cluster of {}", head);

  for (Service& current : cluster_members(head)) {
    IDLE_ASSERT(is_in_same_cluster(current, head));

    for (Import& imp : current.imports()) {
      imp.onImportUnlock();
    }

    call_on_outer_outgoing_decrement(current);

    do_transition_phase(current, phase_t::pending, phase_t::stopped);
  }

  for (Interface& inter : head.interfaces()) {
    RegistryImpl::from(*inter.registry_entry_).onInterfaceUnlock(inter);
  }

  transition_target_complete_if(head, target_t::stop);

  do_mark_cluster_for_stop(head, false);

  IDLE_DETAIL_LOG_TRACE("Checking cyclic restartable of {}: startable: {}, "
                        "shall start: {}",
                        head, is_cluster_startable(head),
                        shall_cluster_start(head));

  // Schedule a self restart if we are finished with a restart restart,
  // and this service marks a leaf of the corresponding chain.
  if (is_cluster_startable(head) && shall_cluster_start(head)) {
    if (!head.isRoot()) {
      IDLE_DETAIL_LOG_TRACE(">> Cluster will be restarted: {}", head);

      head.root().event_loop().post([weak = weakOf(head)] {
        // Dispatch the operation asynchronous so the service is allowed
        // to get garbage collected.
        if (auto locked = weak.lock()) {
          check_any_in_cluster_startable(*locked);
        }
      });
    }
  }

  for (Service& current : cluster_members(head)) {
    detail::garbage_collector::on_service_stop(current);
  }
}

void ServiceImpl::do_increment_epoch(Service const& me) noexcept {
  me.cluster_->epoch_.fetch_add(1, std::memory_order_release);
}

void ServiceImpl::do_transition_phase(Service& me, phase_t from,
                                      phase_t to) noexcept {

  ContextImpl::from(me.root()).call_on_service_changed(me, from, to);
  do_transition_phase_silent(me, from, to);
}

void ServiceImpl::do_transition_phase_silent(Service& me, phase_t from,
                                             phase_t to) noexcept {
  IDLE_ASSERT(me.phase_.load(std::memory_order_acquire) == from);
  (void)from;

  me.phase_.store(to, std::memory_order_release);
}

void ServiceImpl::on_usage_connected(Usage& use) {
  Service& source = use.used().owner();
  Service& target = use.user().owner();

  IDLE_ASSERT(!is_in_same_cluster(source, target));

  if (source.state().isRunning() && target.state().isLocked()) {
    IDLE_DETAIL_LOG_TRACE("  +out (outer) {} ({})", source,
                          target.stats().usage());

    on_outgoing_increment(source);
  }

  if (!source.state().isRunning() && !use.isWeak()) {
    IDLE_DETAIL_LOG_TRACE("  +in (outer) {} ({})", target,
                          target.stats().usage());

    on_outer_required_increment(target);
  }

  if (is_cluster_pushed(target)) {
    call_on_cluster_pushes_increment(source, target);
  }
  if (is_cluster_marked_for_stop(source)) {
    on_cluster_pulls_increment(get_cluster_head_of(target));
  }
}

void ServiceImpl::on_usage_disconnect(Usage& use) {
  Service& source = use.used().owner();
  Service& target = use.user().owner();

  IDLE_DETAIL_LOG_DEBUG("Disconnected usage {} ({}) from {} ({})", source,
                        source.stats().state(), target, target.stats().state());

  if (is_cluster_marked_for_stop(source)) {
    on_cluster_pulls_decrement(get_cluster_head_of(target));
  }
  if (is_cluster_pushed(target)) {
    call_on_cluster_pushes_decrement(source, target);
  }

  if (!source.state().isRunning() && !use.isWeak()) {
    IDLE_DETAIL_LOG_TRACE("  -in (outer) {} ({})", target,
                          target.stats().usage());

    on_outer_required_decrement(target);
  }

  if (source.state().isRunning() && target.state().isLocked()) {
    IDLE_DETAIL_LOG_TRACE("  -out (outer) {} ({})", source,
                          target.stats().usage());

    on_outgoing_decrement(source);
  }
}

void ServiceImpl::on_service_add(Service& me) {
  IDLE_DETAIL_LOG_TRACE("Adding {} -> {} ({})", me.parent().owner(), me,
                        me.stats().usage());

  IDLE_ASSERT(me.state().isStopped());
  IDLE_ASSERT(is_cluster_head(me) || me.parent().owner().state().isStopped());
  IDLE_ASSERT(me.uses_.load(std::memory_order_relaxed) == 0);

  call_on_required_increment(me);

  // Count missing in dependencies
  Context& root = me.root();
  {
    ServiceDependencyGraph const graph(root, graph_view,
                                       GraphFlags::strong_usage_graph());

    for (Edge const& e : in_edges(&me, graph)) {
      IDLE_ASSERT(*target(e, graph) == me);

      Service& from = *source(e, graph);
      if (!from.state().isRunning() && from.state().isInitializedUnsafe()) {
        IDLE_DETAIL_LOG_TRACE("  +in (outer) {} -> {} ({})", from, me,
                              me.stats().usage());

        IDLE_ASSERT(*source(e, graph) != me);
        IDLE_ASSERT(!is_in_same_cluster(from, me));
        on_outer_required_increment(me);
      }
    }
  }

  // Add the missing requirement on the root
  if (has_implicit_root_edge(me)) {
    if (!root.state().isRunning() && root.state().isInitializedUnsafe()) {
      IDLE_DETAIL_LOG_TRACE("  +in (outer) {} -> {} ({})", root, me,
                            me.stats().usage());

      on_outer_required_increment(me);
    }
  }

  IDLE_ASSERT(!is_cluster_pushed(me));
  IDLE_ASSERT(!is_cluster_pulled(me));
  IDLE_ASSERT(!is_cluster_marked_for_stop(me));

  /*if (is_cluster_influencing(me)) {
    IDLE_ASSERT(!is_cluster_head(me));

    ServiceDependencyGraph const graph(root, graph_view,
                                       GraphFlags::external_graph());

    for (Edge e : in_edges(&me, graph)) {
      Service* src = source(e, graph);
      IDLE_ASSERT(src);
      call_on_cluster_influence_increment(*src);
    }
  }*/
}

void ServiceImpl::on_service_del(Service& me) {
  IDLE_DETAIL_LOG_TRACE("Removing {} -> {} ({})", me.parent().owner(), me,
                        me.stats().usage());

  IDLE_ASSERT(me.state().isStopped());
  IDLE_ASSERT(is_cluster_head(me) || me.parent().owner().state().isStopped());
  IDLE_ASSERT(me.uses_.load(std::memory_order_relaxed) == 0);

  if (is_cluster_head(me)) {
    set_override(me, override_t::none);

    do_mark_cluster_for_stop(me, false);
  } else {
    if (is_cluster_pushed(me)) {
      ServiceDependencyGraph const graph(me.root(), graph_view,
                                         GraphFlags::external_graph());

      for (Edge e : in_edges(&me, graph)) {
        Service* src = source(e, graph);
        IDLE_ASSERT(src);
        call_on_cluster_pushes_decrement(*src, me);
      }
    }

    if (is_cluster_marked_for_stop(me)) {
      ServiceDependencyGraph const graph(me.root(), graph_view,
                                         GraphFlags::external_graph());

      for (Edge e : out_edges(&me, graph)) {
        Service* tar = target(e, graph);
        IDLE_ASSERT(tar);
        on_cluster_pulls_decrement(*tar);
      }
    }
  }

  call_on_required_decrement(me);
}

void ServiceImpl::set_override(Service& me, override_t mode) noexcept {
  IDLE_ASSERT(is_cluster_head(me));

  if (me.cluster_->override_ != mode) {
    me.cluster_->override_ = mode;

    if (mode == override_t::start) {
      call_on_cluster_pushes_increment(me);
    } else {
      call_on_cluster_pushes_decrement(me);
    }
  }
}

void ServiceImpl::transition_target_set(Service& me, target_t target) {
  IDLE_ASSERT(me.cluster_);

  target_t const current = me.cluster_->target_;

  if (current != target) {
    me.cluster_->target_ = target;

    if ((current != target_t::none) && me.cluster_->promise_) {
      IDLE_DETAIL_LOG_DEBUG("Canceling promise of {}", me);

      promise<> cache = std::move(me.cluster_->promise_);
      cache.set_canceled();
    }
  }
}

void ServiceImpl::transition_target_complete_if(Service& me, target_t target) {
  if (me.cluster_->target_ == target) {
    me.cluster_->target_ = target_t::none;

    if (me.cluster_->promise_) {
      IDLE_DETAIL_LOG_DEBUG("Resolving promise of {}", me);

      // We are copying the promise to the stack because
      // the action triggered on promise resolving could
      // possibly alter the service again.
      promise<> cache = std::move(me.cluster_->promise_);
      cache.set_value();
    }
  }
}

void ServiceImpl::dependent_add(Export& me, Usage& dependent) {
  IDLE_ASSERT(me.owner().root().is_on_event_loop());
  IDLE_ASSERT(!me.dependent_users_.contains_unsafe(dependent));

  me.dependent_users_.push_back(dependent);
}

void ServiceImpl::dependent_remove(Export& me, Usage& dependent) {
  IDLE_ASSERT(me.owner().root().is_on_event_loop());
  IDLE_ASSERT(me.dependent_users_.contains_unsafe(dependent));

  me.dependent_users_.erase(dependent);
}

void ServiceImpl::call_on_required_decrement(Service& me) noexcept {
  Context& root = me.root();
  if (me != root) {
    {
      ServiceDependencyGraph const graph(me.root(), graph_view,
                                         GraphFlags::cluster_graph());
      for (Edge const& e : out_edges(&me, graph)) {
        Service& to = *target(e, graph);
        IDLE_ASSERT(*source(e, graph) == me);
        IDLE_ASSERT(is_in_same_cluster(me, to));

        IDLE_DETAIL_LOG_TRACE("  -in (inner) {} -> {} ({})", me, to,
                              to.stats().usage());
        on_inner_required_decrement(to);
      }
    }

    {
      ServiceDependencyGraph const graph(me.root(), graph_view,
                                         GraphFlags::external_graph() |
                                             GraphFlags::strong_usage_graph());

      for (Edge const& e : out_edges(&me, graph)) {
        Service& to = *target(e, graph);
        IDLE_ASSERT(*source(e, graph) == me);
        IDLE_ASSERT(!is_in_same_cluster(me, to));

        IDLE_DETAIL_LOG_TRACE("  -in (outer) {} -> {} ({})", me, to,
                              to.stats().usage());
        on_outer_required_decrement(to);
      }
    }
  } else {
    for (Service& current : transitive_services(root).next()) {
      IDLE_DETAIL_LOG_TRACE("  -in (outer) {} -> {} ({})", root, current,
                            current.stats().usage());

      on_outer_required_decrement(current);
    }
  }
}

void ServiceImpl::call_on_required_increment(Service& me) noexcept {
  Context& root = me.root();
  if (me != root) {
    {
      ServiceDependencyGraph const graph(me.root(), graph_view,
                                         GraphFlags::cluster_graph());
      for (Edge const& e : out_edges(&me, graph)) {
        Service& to = *target(e, graph);
        IDLE_ASSERT(*source(e, graph) == me);

        IDLE_DETAIL_LOG_TRACE("  +in (inner) {} -> {}", me, to);
        on_inner_required_increment(to);
      }
    }

    {
      ServiceDependencyGraph const graph(me.root(), graph_view,
                                         GraphFlags::external_graph() |
                                             GraphFlags::strong_usage_graph());

      for (Edge const& e : out_edges(&me, graph)) {
        Service& to = *target(e, graph);
        IDLE_ASSERT(*source(e, graph) == me);

        IDLE_DETAIL_LOG_TRACE("  +in (outer) {} -> {}", me, to);
        on_outer_required_increment(to);
      }
    }
  } else {
    for (Service& current : transitive_services(root).next()) {
      IDLE_DETAIL_LOG_TRACE("  +in (outer) {} -> {}", root, current);

      on_outer_required_increment(current);
    }
  }
}

void ServiceImpl::call_on_outer_outgoing_increment(Service& me) noexcept {
  ServiceDependencyGraph const graph(me.root(), graph_view,
                                     GraphFlags::external_graph());

  IDLE_ASSERT(me.isRoot() || in_edges(&me, graph) ||
              has_implicit_root_edge(me));

  for (Edge const& e : in_edges(&me, graph)) {
    Service& from = *source(e, graph);
    IDLE_ASSERT(*target(e, graph) == me);
    IDLE_ASSERT(me != from);
    IDLE_ASSERT(!is_in_same_cluster(from, me));
    IDLE_ASSERT(e.isWeak() || from.state().isRunning());

    if (e.isWeak() && !from.state().isRunning()) {
      continue;
    }

    on_outgoing_increment(from);
  }

  // For every service which is not a direct parent of root
  // add an implicit use.
  if (has_implicit_root_edge(me)) {
    Context& root = me.root();
    on_outgoing_increment(root);
  }
}

void ServiceImpl::call_on_outer_delayed_outgoing_increment(
    Service& me) noexcept {
  IDLE_ASSERT(me.state().isRunning());

  ServiceDependencyGraph const graph(me.root(), graph_view,
                                     GraphFlags::external_graph());

  for (Edge const& e : out_edges(&me, graph)) {
    Service& to = *target(e, graph);
    IDLE_ASSERT(*source(e, graph) == me);
    IDLE_ASSERT(me != to);
    IDLE_ASSERT(!is_in_same_cluster(to, me));

    if (to.state().isLocked()) {
      on_outgoing_increment(me);
    }
  }
}

void ServiceImpl::call_on_outer_outgoing_decrement(Service& me) noexcept {
  ServiceDependencyGraph const graph(me.root(), graph_view,
                                     GraphFlags::external_graph());

  IDLE_ASSERT(me.isRoot() || in_edges(&me, graph) ||
              has_implicit_root_edge(me));

  for (Edge const& e : in_edges(&me, graph)) {
    Service& from = *source(e, graph);
    IDLE_ASSERT(me != from);
    IDLE_ASSERT(!is_in_same_cluster(from, me));

    if (from.state().isRunning()) {
      on_outgoing_decrement(from);
    }
  }

  // For every service which is not a direct parent of root
  // remove an implicit use.
  if (has_implicit_root_edge(me)) {
    on_outgoing_decrement(me.root());
  }
}

void ServiceImpl::call_on_inner_outgoing_increment(Service& me) noexcept {
  ServiceDependencyGraph const graph(me.root(), graph_view,
                                     GraphFlags::cluster_graph());
  for (Edge const& e : in_edges(&me, graph)) {
    Service& from = *source(e, graph);
    IDLE_ASSERT(me != from);
    IDLE_ASSERT(is_in_same_cluster(from, me));
    on_outgoing_increment(from);
  }
}

void ServiceImpl::call_on_inner_outgoing_decrement(Service& me) noexcept {
  ServiceDependencyGraph const graph(me.root(), graph_view,
                                     GraphFlags::cluster_graph());
  for (Edge const& e : in_edges(&me, graph)) {
    Service& from = *source(e, graph);
    IDLE_ASSERT(me != from);
    IDLE_ASSERT(is_in_same_cluster(from, me));
    on_outgoing_decrement(from);
  }
}

void ServiceImpl::call_on_cluster_running_increment(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(
      me.cluster_->running_ <
      std::numeric_limits<
          std::remove_reference_t<decltype(me.cluster_->running_)>>::max());

  ++me.cluster_->running_;
}

void ServiceImpl::call_on_cluster_running_decrement(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(me.cluster_->running_ > 0);

  --me.cluster_->running_;

  if (me.cluster_->running_ == 0U) {
    Service& head = get_cluster_head_of(me);

    do_increment_epoch(head);

    do_unlock_cluster(head);
  }
}

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_TRACE
inline auto indentation(std::size_t depth) {
  return printable([=](std::ostream& os) {
    for (std::size_t i = 0; i < depth; ++i) {
      os.write("  ", 2);
    }
  });
}
#endif

void ServiceImpl::call_on_cluster_pushes_increment(
    Service& me, Nullable<Service> ancestor) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(is_cluster_head(me));

  if (me.cluster_->pushes_ == 0U) {
    ClusterDependencyGraph const graph(me.root(), graph_view);
    auto const rev = boost::make_reverse_graph(graph);

    IDLE_DETAIL_LOG_TRACE("pushes increment start: {}", me);

    DFSData data;
    if (ancestor) {
      Service& ancestor_head = get_cluster_head_of(*ancestor);
      data.ancestors({&ancestor_head});
    }

    dfs(rev, &me, data,
        [&](dfs_event_visit, Service* head) {
          IDLE_ASSERT(head);
          IDLE_ASSERT(is_cluster_head(*head));

          IDLE_DETAIL_LOG_TRACE("{}{} +1 push: {}",
                                indentation(data.stack.size()),
                                head->cluster_->pushes_, *head);

          head->cluster_->pushes_ += 1;

          if (head->cluster_->pushes_ == 1U) {
            return true;
          } else {
            return false;
          }
        },
        detail::none1{},
        {DFSFlags::flag_traverse_all_paths, DFSFlags::flag_post_cancel_cycles});

  } else {
    IDLE_DETAIL_LOG_TRACE("{} +1 push: {}", me.cluster_->pushes_, me);

    ++me.cluster_->pushes_;
  }
}

void ServiceImpl::call_on_cluster_pushes_decrement(
    Service& me, Nullable<Service> ancestor) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(is_cluster_head(me));

  IDLE_DETAIL_LOG_TRACE("pushes decrement start: {} (current: {})", me,
                        me.cluster_->pushes_);
  IDLE_ASSERT(me.cluster_->pushes_ != 0);

  if (me.cluster_->pushes_ == 1U) {
    ClusterDependencyGraph const graph(me.root(), graph_view);
    auto const rev = boost::make_reverse_graph(graph);

    DFSData data;
    if (ancestor) {
      Service& ancestor_head = get_cluster_head_of(*ancestor);
      data.ancestors({&ancestor_head});
    }

    dfs(rev, &me, data,
        [&](dfs_event_visit, Service* head) {
          IDLE_ASSERT(head);
          IDLE_ASSERT(is_cluster_head(*head));

          IDLE_DETAIL_LOG_TRACE("{}{} -1 push: {}",
                                indentation(data.stack.size()),
                                head->cluster_->pushes_, *head);

          IDLE_ASSERT(head->cluster_->pushes_ != 0);

          head->cluster_->pushes_ -= 1;

          if (head->cluster_->pushes_ == 0U) {
            IDLE_ASSERT(!ServiceImpl::is_cluster_overriden_for_start(*head));
            check_stoppable(*head);
            return true;
          } else {
            return false;
          }
        },
        detail::none1{},
        {DFSFlags::flag_traverse_all_paths, DFSFlags::flag_post_cancel_cycles});

    IDLE_ASSERT(me.cluster_->pushes_ == 0U);
  } else {
    IDLE_DETAIL_LOG_TRACE("{} -1 push: {}", me.cluster_->pushes_, me);

    --me.cluster_->pushes_;
  }
}

void ServiceImpl::call_on_cluster_pulls_increment(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(is_cluster_head(me));

  ClusterDependencyGraph const graph(me.root(), graph_view);
  for (auto&& e : out_edges(&me, graph)) {
    Service* tar = target(e, graph);
    IDLE_ASSERT(tar);

    on_cluster_pulls_increment(*tar);
  }
}

void ServiceImpl::call_on_cluster_pulls_decrement(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(is_cluster_head(me));

  ClusterDependencyGraph const graph(me.root(), graph_view);
  for (auto&& e : out_edges(&me, graph)) {
    Service* tar = target(e, graph);
    IDLE_ASSERT(tar);

    on_cluster_pulls_decrement(*tar);
  }
}

void ServiceImpl::on_cluster_pulls_increment(Service& head) noexcept {
  IDLE_ASSERT(head.root().is_on_event_loop());
  IDLE_ASSERT(head.state().isInitializedUnsafe());
  IDLE_ASSERT(is_cluster_head(head));

  IDLE_DETAIL_LOG_TRACE("pulls increment: {} (current: {})", head,
                        head.cluster_->pulls_);

  head.cluster_->pulls_ += 1;
}

void ServiceImpl::on_cluster_pulls_decrement(Service& head) noexcept {
  IDLE_ASSERT(head.root().is_on_event_loop());
  IDLE_ASSERT(head.state().isInitializedUnsafe());
  IDLE_ASSERT(is_cluster_head(head));

  IDLE_DETAIL_LOG_TRACE("pulls decrement: {} (current: {})", head,
                        head.cluster_->pulls_);
  IDLE_ASSERT(head.cluster_->pulls_ != 0);

  head.cluster_->pulls_ -= 1;

  if (head.cluster_->pulls_ == 0U) {
    check_any_in_cluster_startable(head);
  }
}

void ServiceImpl::on_outgoing_increment(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isRunning());

  auto const previous = me.uses_.fetch_add(1U, std::memory_order_relaxed);
  (void)previous;

  IDLE_ASSERT(previous > 0);

  IDLE_ASSERT(
      previous <
      std::numeric_limits<std::remove_reference_t<decltype(previous)>>::max());
}

void ServiceImpl::on_outgoing_decrement(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isRunning());

  auto const previous = me.uses_.fetch_sub(1U, std::memory_order_relaxed);
  IDLE_ASSERT(previous > 1U);

  if (has_no_external_uses(me, previous - 1)) {
    on_outgoing_reached_no_external_use(me, previous - 1);
  }
}

bool ServiceImpl::has_no_external_uses(Service const& me,
                                       std::uint32_t current) noexcept {
  return (current == 1U) || (current == 2U && isa<Import>(me.parent()));
}

void ServiceImpl::on_outgoing_reached_no_external_use(
    Service& me, std::uint32_t current) noexcept {

  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(has_no_external_uses(me, current));
  IDLE_ASSERT(current != 0U);

  if (current == 1U) {
    check_stoppable(me);
  } else {
    IDLE_ASSERT(current == 2U && isa<Import>(me.parent()));

    // Fixes notification when a cluster member which is not the head of the
    // said cluster exports anything and shall be stopped.
    check_stoppable(get_cluster_head_of(me));
  }
}

void ServiceImpl::on_outer_required_increment(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(me.cluster_->outer_deps_missing_ <
              std::numeric_limits<std::uint16_t>::max());

  ++me.cluster_->outer_deps_missing_;
}

void ServiceImpl::on_outer_required_decrement(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(me.cluster_->outer_deps_missing_ != 0U);

  --me.cluster_->outer_deps_missing_;

  if (me.cluster_->outer_deps_missing_ == 0) {
    Service& head = get_cluster_head_of(me);
    check_any_in_cluster_startable(head);
  }
}

void ServiceImpl::on_inner_required_increment(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(me.inner_deps_missing_ < (1 << 12));

  ++me.inner_deps_missing_;
}

void ServiceImpl::on_inner_required_decrement(Service& me) noexcept {
  IDLE_ASSERT(me.root().is_on_event_loop());
  IDLE_ASSERT(me.state().isInitializedUnsafe());
  IDLE_ASSERT(me.inner_deps_missing_ > 0U);

  --me.inner_deps_missing_;

  if (me.inner_deps_missing_ == 0) {
    check_inner_startable(me);
  }
}

void ServiceImpl::do_mark_cluster_for_stop(Service& head, bool set) noexcept {
  if (set != head.cluster_->is_marked_for_stop_) {
    head.cluster_->is_marked_for_stop_ = set;

    if (set) {
      call_on_cluster_pulls_increment(head);
    } else {
      call_on_cluster_pulls_decrement(head);
    }
  }
}

bool ServiceImpl::shall_cluster_start(Service& me) noexcept {
  // TODO Maybe we should start when the cluster is influencing or marked?
  return (is_cluster_pushed(me) && is_cluster_marked_for_start(me)) &&
         !is_cluster_marked_for_stop(me) && !is_cluster_pulled(me);
}

bool ServiceImpl::shall_cluster_stop(Service& me) noexcept {
  return !is_cluster_pushed(me) || is_cluster_marked_for_stop(me);
}

void ServiceImpl::check_any_in_cluster_startable(Service& head) noexcept {
  IDLE_ASSERT(is_cluster_head(head));

  IDLE_DETAIL_LOG_TRACE(
      "Checking if any in cluster {} is cluster startable: {}, shall start: {}",
      head, is_cluster_startable(head), shall_cluster_start(head));

  if (shall_cluster_start(head)) {
    ContextImpl& impl = ContextImpl::from(head.root());

    ServiceDependencyGraph const graph(impl, graph_view,
                                       GraphFlags::cluster_graph());

    for (Service& member : cluster_members(head)) {
      if (!in_edges(&member, graph)) {
        IDLE_DETAIL_LOG_TRACE(
            "Checking if inner service is startable: {} (startable: {})",
            member, member.state().isStartable());

        if (member.state().isStartable()) {
          IDLE_DETAIL_LOG_TRACE("> Ok, Service is startable: {}", member);
          impl.associated_scheduler().on_service_startable(member);
        }
      }
    }
  }
}

void ServiceImpl::check_inner_startable(Service& me) noexcept {
  IDLE_DETAIL_LOG_TRACE("Checking if service is startable: {} ({} - startable: "
                        "{}, shall cluster start: {})",
                        me, me.stats().usage(), me.state().isStartable(),
                        shall_cluster_start(me));

  if (me.state().isStartable() && shall_cluster_start(me)) {
    IDLE_DETAIL_LOG_TRACE("> Ok, Service is startable: {}", me);

    ContextImpl& impl = ContextImpl::from(me.root());
    impl.associated_scheduler().on_service_startable(me);
  }
}

void ServiceImpl::check_stoppable(Service& me) noexcept {
  IDLE_DETAIL_LOG_TRACE("Checking if service is stoppable: {} ({} - stoppable: "
                        "{}, shall cluster stop: {})",
                        me, me.stats().usage(), me.state().isStoppable(),
                        shall_cluster_stop(me));

  if (me.state().isStoppable() && shall_cluster_stop(me)) {
    IDLE_DETAIL_LOG_TRACE("> Ok, Service is stoppable: {}", me);

    ContextImpl& impl = ContextImpl::from(me.root());
    impl.associated_scheduler().on_service_stoppable(me);
  }
}
} // namespace idle
