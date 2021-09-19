
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

#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/tiernan_all_cycles.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/context/scheduler.hpp>
#include <idle/core/detail/for_each.hpp>
#include <idle/core/detail/graph/dfs.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/detail/streams.hpp>
#include <idle/core/detail/when_completed.hpp>
#include <idle/core/external/boost/graph.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/iterators.hpp>
#include <idle/core/util/panic.hpp>
#include <idle/core/util/printable.hpp>
#include <idle/core/views/filter.hpp>
#include <idle/core/views/start.hpp>

#ifdef _MSC_VER
#  pragma warning(disable : 4267)
#endif // _MSC_VER

#ifdef IDLE_DETAIL_HAS_CORE_TRACE
#  define IDLE_DETAIL_TRACE_SCHEDULER(...)                                     \
    do {                                                                       \
      IDLE_DETAIL_LOG_TRACE(__VA_ARGS__);                                      \
      detail::indentation_stream ident__(std::cerr);                           \
      dependency_graph graph__(root_, graph_view);                             \
      graph__.print_list(ident__);                                             \
      std::cerr.flush();                                                       \
    } while (false)
#else
#  define IDLE_DETAIL_TRACE_SCHEDULER(...)
#endif

namespace idle {
using override_t = detail::override_t;

struct cycle_collector_visitor {
  std::vector<std::vector<Node>>& paths;

  template <typename Graph>
  void cycle(std::vector<Node> const& path, Graph const&) {
    paths.push_back(path);
  }
};

static cyclic_dependency_exception get_cyclic_dependencies(Context& root) {
  IndexedDependencyGraph const indexed(root);
  auto const reverse = boost::make_reverse_graph(indexed);

  std::vector<std::vector<Node>> paths;
  boost::tiernan_all_cycles(reverse, cycle_collector_visitor{paths});

  // TODO Add the paths to the exception
  (void)paths;

  IDLE_DETAIL_TRAP();
  return cyclic_dependency_exception{};
}

Nullable<Service> SchedulingQueue::pop() {
  if (!down_.empty()) {
    return pop_one(down_);
  }
  if (!up_.empty()) {
    return pop_one(up_);
  }
  return {};
}

static auto details_of(Service& current) {
  return printable([current = &current](std::ostream& os) {
    print(os, "{} - {} - {}", current->stats().state(),
          current->stats().usage(), current->stats().cluster());
  });
}

void SchedulingQueue::push_down(Service& value) {
  IDLE_DETAIL_LOG_DEBUG("Pushed '{}' ({} - {} - {}) to iteration queue (down)",
                        value, value.stats().state(), value.stats().usage(),
                        value.stats().cluster());

  IDLE_ASSERT(up_.find(&value) == up_.end());
  IDLE_ASSERT(value.state().isStoppable());

  auto const result = down_.insert(&value);

  // IDLE_ASSERT(result.second); // The element might be in the queue currently
  IDLE_ASSERT(result.first != down_.end());
  IDLE_ASSERT(result.first == down_.find(&value));
}

void SchedulingQueue::push_up(Service& value) {
  IDLE_DETAIL_LOG_DEBUG("Pushed '{}' ({}) to iteration queue (up)", value,
                        details_of(value));

  IDLE_ASSERT(down_.find(&value) == down_.end());
  IDLE_ASSERT(value.state().isStartable());

  auto const result = up_.insert(&value);

  // IDLE_ASSERT(result.second); // The element might be in the queue currently
  IDLE_ASSERT(result.first != up_.end());
  IDLE_ASSERT(result.first == up_.find(&value));
}

static void erase_one(SchedulingQueue::set_t& queue, Service& value) {
#ifndef NDEBUG
  auto const itr = queue.find(&value);
  bool const contains = itr != queue.end();
#endif

  IDLE_ASSERT(!contains || *itr == &value);

  std::size_t count = queue.erase(&value);

  (void)count;
  IDLE_ASSERT((count == 1U) == contains);
}

void SchedulingQueue::erase(Service& value) {
  IDLE_DETAIL_LOG_TRACE("Erasing {} from scheduling queue!", value);

  erase_one(down_, value);
  erase_one(up_, value);
}

void SchedulingQueue::clear() {
  up_.clear();
  down_.clear();
}

bool SchedulingQueue::empty() const noexcept {
  return up_.empty() && down_.empty();
}

FlatSet<Service*, std::greater<>>::size_type
SchedulingQueue::size() const noexcept {
  return up_.size() + down_.size();
}

Service& SchedulingQueue::pop_one(set_t& current) {
  // We always pop the last element from the set since this is the
  // cheapest direction to take elements from.
  auto const itr = std::prev(current.end());
  Service* first = *itr;
  auto const next = current.erase(itr);
  IDLE_ASSERT(next == current.end());
  return *first;
}

static bool start_traversal_progresses_further(Service const& head) noexcept {
  if (head.state().isRunning() &&
      !ServiceImpl::is_cluster_marked_for_stop(head)) {
    return false;
  }

  return !ServiceImpl::is_cluster_marked_for_start(head);
}

void Scheduler::insert_into_queue_if_startable(Service& current) {
  if (!ServiceImpl::shall_cluster_start(current)) {
    return;
  }

  for (Service& member : cluster_members(current)) {
    if (member.state().isStartable()) {
      IDLE_DETAIL_LOG_TRACE("start leaf: {} [{}]", member, details_of(member));

      queue_.push_up(member);
    }
  }
}

void Scheduler::on_service_start_request(Service& current) {
  IDLE_ASSERT(root_.is_on_event_loop());
  IDLE_ASSERT(current.cluster_);
  IDLE_ASSERT(current.state().isInitializedUnsafe());
  IDLE_ASSERT(is_cluster_head(current));

  IDLE_DETAIL_LOG_DEBUG("Requesting start of: {} [{}]", current,
                        details_of(current));

  if (!start_traversal_progresses_further(current)) {
    IDLE_DETAIL_LOG_TRACE("{} is marked for start already!", current);
  }

  ClusterDependencyGraph const graph(root_, graph_view);
  auto const rev = boost::make_reverse_graph(graph);

  DFSScope const scope(dfs_data_);
  (void)scope;

  dfs(
      rev, &current, dfs_data_,
      [&](dfs_event_visit, Service* head) {
        IDLE_ASSERT(is_cluster_head(*head));

        IDLE_DETAIL_LOG_TRACE("start visit: {} [{}]", *head, details_of(*head));

        ServiceImpl::do_mark_cluster_for_start(*head, true);

        insert_into_queue_if_startable(*head);
        return true;
      },
      [&](auto const& e) {
        Service* tar = target(e, rev);
        IDLE_ASSERT(tar);

        if (!start_traversal_progresses_further(*tar)) {
          // Set services with a potential back edge as leaf,
          // otherwise they might not be started.
          insert_into_queue_if_startable(*tar);
          return false;
        } else {
          return true;
        }
      });

  // The above dfs algorithm never encounters a cycle
  IDLE_ASSERT(dfs_data_.acyclic);
  iterate();
}

static bool stop_traversal_progresses_further(Service const& head) noexcept {
  IDLE_ASSERT(is_cluster_head(head));

  if (ServiceImpl::is_cluster_stopped(head)) {
    return false;
  }

  return !ServiceImpl::is_cluster_marked_for_stop(head);
}

void Scheduler::insert_into_queue_if_stoppable(Service& current) {
  IDLE_DETAIL_LOG_TRACE("checking service for stop: {} [{}]", current,
                        details_of(current));

  if (current.state().isStoppable()) {
    IDLE_DETAIL_LOG_TRACE(">> stop service: {} [{}]", current,
                          details_of(current));

    queue_.push_down(current);
  }
}

void Scheduler::on_service_stop_request(Service& current) {
  IDLE_ASSERT(root_.is_on_event_loop());
  IDLE_ASSERT(current.cluster_);
  IDLE_ASSERT(current.state().isInitializedUnsafe());
  IDLE_ASSERT(is_cluster_head(current));

  IDLE_DETAIL_LOG_DEBUG("Requesting stop of: {} [{}]", current,
                        details_of(current));

  if (!stop_traversal_progresses_further(current)) {
    IDLE_DETAIL_LOG_TRACE("{} is marked for stop already!", current);
  }

  if (current == root_) {
    stop_root();
    return;
  }

  DFSScope const scope(dfs_data_);
  (void)scope;

  ClusterDependencyGraph const graph(root_, graph_view);
  dfs(
      graph, &current, dfs_data_,
      [&](dfs_event_visit, Service* head) {
        IDLE_ASSERT(is_cluster_head(*head));

        IDLE_DETAIL_LOG_TRACE("V: stop visit: {} [{}]", *head,
                              details_of(*head));

        ServiceImpl::do_mark_cluster_for_stop(*head, true);

        insert_into_queue_if_stoppable(*head);
        return true;
      },
      [&](Edge const& e) {
        Service* dest = target(e, graph);
        IDLE_ASSERT(dest);

        bool const progress_further = stop_traversal_progresses_further(*dest);

        // The plain edge target is the usage whereas the target of the graph
        // is the service the usage points to.
        if (e.target().is_usage()) {
          Usage& use = e.target().get<Usage>();

          // Attempt to break weak cycles
          if (progress_further || use.isWeak()) {
            if (ServiceImpl::do_release_usage(use)) {
              return false;
            }
          }
        }
        return progress_further;
      });

  // The dfs algorithm above never encounters a cycle
  IDLE_ASSERT(dfs_data_.acyclic);

  IDLE_DETAIL_LOG_TRACE("V: stop raversal of '{}' finished.", current);

  iterate();
}

void Scheduler::on_service_startable(Service& current) {
  IDLE_ASSERT(root_.is_on_event_loop());

  IDLE_DETAIL_LOG_DEBUG("Service '{}' shall be started! [{}]", current,
                        details_of(current));

  IDLE_ASSERT(current.state().isStartable());
  IDLE_ASSERT(current.isRoot() || !root_.state().isStopped());
  IDLE_ASSERT(current.isRoot() ||
              (isa<Export>(current.parent()) ==
               current.parent().owner().state().isRunning()));

  if (!current.isRoot() && !ServiceImpl::is_cluster_running(root_)) {
    ServiceImpl::start_root(root_);
  }

  queue_.push_up(current);
  iterate();
}

void Scheduler::on_service_stoppable(Service& current) {
  IDLE_ASSERT(root_.is_on_event_loop());
  IDLE_ASSERT(current.state().isStoppable());

  IDLE_DETAIL_LOG_DEBUG("Service '{}' shall be stopped! [{}]", current,
                        details_of(current));

  queue_.push_down(current);
  iterate();
}

static bool is_unhealthy(Service& head) {
  IDLE_ASSERT(is_cluster_head(head));

  return ServiceImpl::is_cluster_pushed(head) &&
         (!ServiceImpl::is_cluster_running(head) ||
          ServiceImpl::is_cluster_marked_for_stop(head));
}

continuable<> Scheduler::do_update_system() {
  return root_.event_loop().async_post(
      wrap(*this, [](auto&& me) mutable -> continuable<> {
        IDLE_ASSERT(me->owner().state().isRunning());

        auto const unhealthy = clusters(me->root_) |
                               views::filter([](Service& head) {
                                 return is_unhealthy(head);
                               }) |
                               views::start(Reason::Implicit);

        if (!unhealthy) {
          return make_ready_continuable();
        }

        IDLE_DETAIL_LOG_DEBUG("Updating {} unhealthy services...",
                              unhealthy.size());

        // Perform a fixpoint iteration until the system is stable
        return detail::when_completed(unhealthy).then(
            wrap(*me, [](auto&& me) mutable {
              return me->do_update_system();
            }));
      }));
}

void Scheduler::partName(std::ostream& os) const {
  os << "idle::Scheduler";
}

bool Scheduler::try_start_service_inplace(Service& current) {
  continuable<> hook = ServiceImpl::do_start_eager(current);
  if (hook.is_ready()) {
    result<> result = std::move(hook).unpack();
    if (result.is_exception()) {
      handle_start_async_exception(current, result.get_exception());
      return false;
    } else {
      IDLE_DETAIL_LOG_DEBUG("Called service::on_start of '{}' synchronously",
                            current);
      return true;
    }
  } else {
    IDLE_DETAIL_LOG_DEBUG("Calling service::on_start of '{}' asynchronously",
                          current);

    std::move(hook)
        .next(
            [this, current = &current](auto&&... args) mutable {
              IDLE_ASSERT(current->root().is_on_event_loop());
              on_service_start_cb(*current,
                                  std::forward<decltype(args)>(args)...);
            },
            root_.event_loop().through_dispatch())
        .done();
    return false;
  }
}

void Scheduler::handle_start_async_exception(Service& thrower,
                                             std::exception_ptr const& e) {

  IDLE_DETAIL_LOG_TRACE("Received an exception from {} ", thrower);

  if (e) {
    try {
      std::rethrow_exception(e);
    } catch (std::exception const& rethrown) {
      char const* msg = rethrown.what();
      (void)msg;

      IDLE_PANIC(
          false,
          fmt::format(
              FMT_STRING(
                  "Service {} threw an exception during start or stop:\n{}"),
              thrower, msg));
    } catch (...) {
      IDLE_PANIC(false,
                 fmt::format(FMT_STRING("Service {} threw an unknown exception "
                                        "during start or stop!"),
                             thrower));
    }
  } else {
    IDLE_PANIC(false, fmt::format(FMT_STRING("Service {} cancelled its start!"),
                                  thrower));
  }

  IDLE_DETAIL_UNREACHABLE();

  // Pass the exception to every service which was started explicitly
  // and that has the thrower as dependency.
  // ServiceDependencyGraph const graph(root_, graph_view);

  /*
   dfs_result const result = dfs(
      graph, &thrower, [&](dfs_event_visit, service* current) {
        if (service_impl::is_in_transition(*current,
                                           detail::transition_t::start)) {
          service_impl::do_handle_exception(*current, thrower, e);
          return true;
        } else {
          return false;
        }
      });*/

  // It should never happen that the visited graph is cyclic since this
  // was checked on starting the dependent services and services added
  // meanwhile are not tagged as starting.
  // IDLE_ASSERT(result.acyclic);
}

void Scheduler::on_service_start_cb(Service& current) {
  IDLE_DETAIL_TRACE_SCHEDULER("Started '{}' asynchronously", current);
  (void)current;
}

void Scheduler::on_service_start_cb(Service& current, exception_arg_t,
                                    std::exception_ptr e) {

  handle_start_async_exception(current, e);
}

bool Scheduler::try_stop_service_inplace(Service& current) {
  continuable<> hook = ServiceImpl::do_stop_eager(current);
  if (hook.is_ready()) {
    result<> result = std::move(hook).unpack();
    if (result.is_exception()) {
      handle_stop_async_exception(current, result.get_exception());
      return false;
    } else {
      IDLE_DETAIL_LOG_DEBUG("Called service::on_stop of '{}' synchronously",
                            current);
      return true;
    }
  } else {
    IDLE_DETAIL_LOG_DEBUG("Calling service::on_stop of '{}' asynchronously",
                          current);

    std::move(hook)
        .next(
            [this, current = &current](auto&&... args) mutable {
              IDLE_ASSERT(current->root().is_on_event_loop());
              on_service_stop_cb(*current,
                                 std::forward<decltype(args)>(args)...);
            },
            root_.event_loop().through_dispatch())
        .done();
    return false;
  }
}

void Scheduler::handle_stop_async_exception(Service& thrower,
                                            std::exception_ptr const& e) {

  IDLE_DETAIL_TRAP();
  (void)thrower;
  (void)e;

  /*IDLE_DETAIL_LOG("Received an exception from {} ", thrower);

  // Pass the exception to every service which was started explicitly
  // and that has the thrower as dependency.
  service_dependency_graph const graph(root_, graph_view);

  dfs_result const result =
      dfs(graph, &thrower, [&](dfs_event_visit, service& current) {
        if (is_in_transition(current, detail::transition_t::start)) {
          handle_dependent_exception(current, thrower, e);
          return true;
        } else {
          return false;
        }
      });

  // It should never happen that the visited graph is cyclic since this
  // was checked on starting the dependent services and services added
  // meanwhile are not tagged as starting.
  IDLE_ASSERT(result.acyclic);*/
}

void Scheduler::on_service_stop_cb(Service& current) {
  IDLE_DETAIL_LOG_DEBUG("Stopped '{}' asynchronously", current);
  (void)current;
}

void Scheduler::on_service_stop_cb(Service& current, exception_arg_t,
                                   std::exception_ptr e) {

  handle_start_async_exception(current, e);
}

void Scheduler::stop_root() {
  if (ServiceImpl::is_cluster_stopped(root_)) {
    return;
  }

  if (ServiceImpl::is_cluster_marked_for_stop(root_)) {
    return;
  }

  IDLE_DETAIL_LOG_DEBUG("Stopping all services!");

  // Clear all pending actions, service waiting for stop will
  // be re-queued through the iteration below.
  queue_.clear();

  // Since the root is the only service on which all services
  // implicitly depend (even without a direct dependency),
  // we iterate through the system and stop all services.
  for (Service& head : clusters(root_)) {
    if (head.state().isLocked()) {
      // Dissolve weak usages
      for (Export& exp : head.exports()) {
        for (Usage& use : exp.exports()) {
          if (use.isWeak()) {
            bool const result = ServiceImpl::do_release_usage(use);
            (void)result;
            IDLE_ASSERT(result);
          }
        }
      }
    }

    ServiceImpl::set_override(head, override_t::none);
    ServiceImpl::transition_target_set(head, detail::target_t::none);
    ServiceImpl::do_mark_cluster_for_start(head, false);

    if (!ServiceImpl::is_cluster_stopped(head)) {
      ServiceImpl::do_mark_cluster_for_stop(head, true);

      if (ServiceImpl::is_cluster_stoppable(head)) {
        queue_.push_down(head);
      }
    }
  }

  iterate();
}

void Scheduler::iterate() noexcept {
  IDLE_ASSERT(root_.is_on_event_loop());

  // In case we are not in an iteration start a new one
  if (!processing_ && !processing_dispatched_) {
    processing_dispatched_ = true;
    IDLE_DETAIL_LOG_TRACE("Queueing the next scheduling iteration");

    root_.event_loop().post([this] {
      IDLE_ASSERT(!processing_);
      IDLE_ASSERT(processing_dispatched_);
      processing_dispatched_ = false;

      // This keeps this->processing_ true as long as this scope is valid
      std::unique_ptr<bool, void (*)(bool*)> lock{&processing_,
                                                  [](bool* processing) mutable {
                                                    IDLE_ASSERT(*processing);
                                                    *processing = false;
                                                  }};
      processing_ = true;

      process();
    });
  }
}

void Scheduler::process() noexcept {
  IDLE_ASSERT(root_.is_on_event_loop());
  IDLE_ASSERT(processing_);

  IDLE_DETAIL_LOG_TRACE("Processing {} services from queue...", queue_.size());

  while (Nullable<Service> changing = queue_.pop()) {
    IDLE_DETAIL_LOG_DEBUG("Popped '{}' ({} - {} - {}) from iteration queue",
                          *changing, changing->stats().state(),
                          changing->stats().usage(),
                          changing->stats().cluster());

    if (ServiceImpl::shall_cluster_stop(*changing)) {
      if (changing->state().isStoppable()) {
        if (ServiceImpl::try_set_service_stopping(*changing)) {
          IDLE_DETAIL_LOG_DEBUG("Stopping service '{}' ", *changing);
          (void)try_stop_service_inplace(*changing);
        }
      } else {
        IDLE_DETAIL_LOG_ERROR("Service '{}' is not stoppable yet", *changing);
      }
    } else if (ServiceImpl::shall_cluster_start(*changing)) {
      if (changing->state().isStartable()) {
        IDLE_DETAIL_LOG_DEBUG("Starting service '{}' ", *changing);
        (void)try_start_service_inplace(*changing);
      } else {
        IDLE_DETAIL_LOG_ERROR("Service '{}' is not startable yet", *changing);
      }
    }
  }

  IDLE_ASSERT(queue_.empty());
}

void Scheduler::on_service_init(Service&) {
  IDLE_ASSERT(root_.is_on_event_loop());
}

void Scheduler::on_service_destroy(Service& current) {
  IDLE_ASSERT(root_.is_on_event_loop());
  IDLE_ASSERT(current.state().isInitializedUnsafe());

  queue_.erase(current);
}
} // namespace idle
