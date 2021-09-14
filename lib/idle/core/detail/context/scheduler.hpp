
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

#ifndef IDLE_CORE_DETAIL_CONTEXT_SCHEDULER_HPP_INCLUDED
#define IDLE_CORE_DETAIL_CONTEXT_SCHEDULER_HPP_INCLUDED

#include <array>
#include <cstdint>
#include <functional>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/detail/graph/dfs.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/flat_set.hpp>
#include <idle/core/util/nullable.hpp>

namespace idle {
class SchedulingQueue {
public:
  using set_t = FlatSet<Service*, std::greater<>>;

  SchedulingQueue() = default;

  Nullable<Service> pop();
  void push_up(Service& value);
  void push_down(Service& value);
  void erase(Service& value);
  void clear();

  bool empty() const noexcept;
  set_t::size_type size() const noexcept;

private:
  static Service& pop_one(set_t& current);

  set_t down_;
  set_t up_;
};

class Scheduler : public Export {
  template <typename>
  friend class dependency_visitor;

public:
  explicit Scheduler(Context& root)
    : Export(*static_cast<Service*>(&root))
    , root_(root) {}

  void on_service_init(Service& current);
  void on_service_destroy(Service& current);
  void on_service_start_request(Service& current);
  void on_service_stop_request(Service& current);

  /// A hook which gets called when the service 'can' be started
  void on_service_startable(Service& current);
  /// A hook which gets called when the service 'can' be stopped
  void on_service_stoppable(Service& current);

  continuable<> do_update_system();

protected:
  void partName(std::ostream& os) const override;

private:
  // Start
  bool try_start_service_inplace(Service& current);
  void handle_start_async_exception(Service& thrower,
                                    std::exception_ptr const& e);

  void on_service_start_cb(Service& current);
  void on_service_start_cb(Service& current, exception_arg_t,
                           std::exception_ptr e);

  // Stop
  bool try_stop_service_inplace(Service& current);
  void handle_stop_async_exception(Service& thrower,
                                   std::exception_ptr const& e);

  void on_service_stop_cb(Service& current);
  void on_service_stop_cb(Service& current, exception_arg_t,
                          std::exception_ptr e);

  void insert_into_queue_if_startable(Service& current);
  void insert_into_queue_if_stoppable(Service& current);

  void stop_root();

  void iterate() noexcept;
  void process() noexcept;

  Context& root_;
  bool processing_{false};
  bool processing_dispatched_{false};
  SchedulingQueue queue_;

  // The dfs_data object is cached for allowing allocated heap reuse
  DFSData dfs_data_;
};
} // namespace idle

#endif // IDLE_CORE_DETAIL_CONTEXT_SCHEDULER_HPP_INCLUDED
