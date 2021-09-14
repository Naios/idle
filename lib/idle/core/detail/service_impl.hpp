
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

#ifndef IDLE_CORE_DETAIL_SERVICE_IMPL_HPP_INCLUDED
#define IDLE_CORE_DETAIL_SERVICE_IMPL_HPP_INCLUDED

#include <exception>
#include <idle/core/context.hpp>
#include <idle/core/detail/state.hpp>
#include <idle/core/service.hpp>

namespace idle {
bool is_cluster_head(Service const& member) noexcept;
Service& get_cluster_head_of(Service& member) noexcept;

class ServiceImpl {
  friend Service;
  friend Usage;

  using phase_t = Service::Phase;
  using override_t = detail::override_t;
  using target_t = detail::target_t;

public:
  /// Returns true when both services are inside the same cluster
  static bool is_in_same_cluster(Service& first, Service& second) noexcept;

  static void interface_init(Interface& self, Context& root) noexcept;
  static void interface_publish(Interface& self) noexcept;
  static void interface_revoke(Interface& self) noexcept;
  static void interface_destroy(Interface& self) noexcept;

  static void used_destroy(Usage& u, Export& exp) noexcept;
  static void used_started(Usage& u, Export& exp) noexcept;

  static continuable<> request_start(Service& me, Reason reason);
  static continuable<> request_stop(Service& me, Reason reason);
  static continuable<> request_observe(Service& me);

  static continuable<> do_start_eager(Service& me);
  static continuable<> do_stop_eager(Service& me);
  static void do_handle_exception(Service& me, Service& dependency,
                                  std::exception_ptr const& e);

  static bool try_set_service_stopping(Service& current);

  static void start_root(Context& root) noexcept;

  /// Returns true if the usage could be released or will be release in
  /// the near future.
  static bool do_release_usage(Usage& u) noexcept;

  static void on_import_satisfied(Service& me, Import& imp) noexcept;
  static void on_import_unsatisfied(Service& me, Import& imp) noexcept;

  static phase_t get_phase(Service const& me) noexcept {
    return me.phase_.load(std::memory_order_acquire);
  }
  static bool is_in_phase(Service const& current, phase_t phase) noexcept {
    return get_phase(current) == phase;
  }

  static bool is_cluster_startable(Service const& head) noexcept;
  static bool is_cluster_stoppable(Service const& head) noexcept;
  static bool is_cluster_pushed(Service const& me) noexcept;
  static bool is_cluster_pulled(Service const& me) noexcept;
  static bool is_cluster_running(Service const& head) noexcept;
  static bool is_cluster_stopped(Service const& member) noexcept;

  static bool is_cluster_marked_for_start(Service const& me) noexcept {
    return me.cluster_->is_marked_for_start_;
  }
  static bool is_cluster_marked_for_stop(Service const& me) noexcept {
    return me.cluster_->is_marked_for_stop_;
  }

  static void do_mark_cluster_for_start(Service& head, bool set) noexcept {
    head.cluster_->is_marked_for_start_ = set;
  }

  static void do_mark_cluster_for_stop(Service& head, bool set) noexcept;

  static bool shall_cluster_start(Service& me) noexcept;
  static bool shall_cluster_stop(Service& me) noexcept;

  static void set_override(Service& me, override_t mode) noexcept;
  static bool is_cluster_overriden_for_start(Service const& me) noexcept {
    return me.cluster_->override_ == override_t::start;
  }

  static void transition_target_set(Service& me, target_t target);
  static void transition_target_complete_if(Service& me, target_t target);

  static void dependent_add(Export& me, Usage& dependent);
  static void dependent_remove(Export& me, Usage& dependent);

  static bool verifyCluster(std::ostream& os, Service& head) noexcept;

  static bool try_use(Service const& me) noexcept;
  static void inc_use(Service const& me) noexcept;
  static void dec_use(Service const& current) noexcept;

private:
  static void do_lock_cluster(Service& head) noexcept;
  static void do_finalize_cluster(Service& head);
  static void do_unlock_cluster(Service& head);

  static void do_increment_epoch(Service const& me) noexcept;

  static void do_transition_phase(Service& me, phase_t from,
                                  phase_t to) noexcept;
  static void do_transition_phase_silent(Service& me, phase_t from,
                                         phase_t to) noexcept;

  static void on_usage_connected(Usage& use);
  static void on_usage_disconnect(Usage& use);
  static void on_service_add(Service& me);
  static void on_service_del(Service& me);

  static void call_on_required_decrement(Service& me) noexcept;
  static void call_on_required_increment(Service& me) noexcept;
  static void call_on_outer_outgoing_increment(Service& me) noexcept;
  static void call_on_outer_delayed_outgoing_increment(Service& me) noexcept;
  static void call_on_outer_outgoing_decrement(Service& me) noexcept;
  static void call_on_inner_outgoing_increment(Service& me) noexcept;
  static void call_on_inner_outgoing_decrement(Service& me) noexcept;
  static void call_on_cluster_running_increment(Service& me) noexcept;
  static void call_on_cluster_running_decrement(Service& me) noexcept;
  static void
  call_on_cluster_pushes_increment(Service& me,
                                   Nullable<Service> ancestor = {}) noexcept;
  static void
  call_on_cluster_pushes_decrement(Service& me,
                                   Nullable<Service> ancestor = {}) noexcept;

  static void call_on_cluster_pulls_increment(Service& me) noexcept;
  static void call_on_cluster_pulls_decrement(Service& me) noexcept;

  static void on_cluster_pulls_increment(Service& head) noexcept;
  static void on_cluster_pulls_decrement(Service& head) noexcept;

  static void on_outgoing_increment(Service& me) noexcept;
  static void on_outgoing_decrement(Service& me) noexcept;
  static bool has_no_external_uses(Service const& me,
                                   std::uint32_t current) noexcept;

  static void
  on_outgoing_reached_no_external_use(Service& me,
                                      std::uint32_t current) noexcept;

  static void on_outer_required_increment(Service& me) noexcept;
  static void on_outer_required_decrement(Service& me) noexcept;

  static void on_inner_required_increment(Service& me) noexcept;
  static void on_inner_required_decrement(Service& me) noexcept;

  static void check_any_in_cluster_startable(Service& head) noexcept;
  static void check_inner_startable(Service& me) noexcept;
  static void check_stoppable(Service& me) noexcept;
};
} // namespace idle

#endif // IDLE_CORE_DETAIL_SERVICE_IMPL_HPP_INCLUDED
