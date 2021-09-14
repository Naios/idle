
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

#ifndef IDLE_CORE_DETAIL_GRAPH_DFS_HPP_INCLUDED
#define IDLE_CORE_DETAIL_GRAPH_DFS_HPP_INCLUDED

#include <stack>
#include <type_traits>
#include <utility>
#include <idle/core/external/boost/graph.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/flat_set.hpp>
#include <idle/core/util/range.hpp>
#include <idle/core/views/filter.hpp>

namespace idle {
namespace detail {
template <typename T>
class wrap : public T {
public:
  explicit wrap(T visitor)
    : T(std::move(visitor)) {}

  using T::operator();

  template <typename... Args>
  bool operator()(Args&&...) const noexcept {
    return true;
  }
};

struct none1 {
  template <typename A1>
  bool operator()(A1&&) const noexcept {
    return true;
  }
};
struct none2 {
  template <typename A1, typename A2>
  bool operator()(A1&&, A2&&) const noexcept {
    return true;
  }
};
} // namespace detail

enum class dfs_event_t { visit, leaf };

template <dfs_event_t Value>
struct dfs_event_arg_t : std::integral_constant<dfs_event_t, Value> {};

using dfs_event_visit = dfs_event_arg_t<dfs_event_t::visit>;
using dfs_event_leaf = dfs_event_arg_t<dfs_event_t::leaf>;

namespace detail {
template <typename>
struct vertex_type_tag_t {};

inline Service* get_vertex_from_ref(vertex_type_tag_t<Service*>,
                                    Service& s) noexcept {
  return std::addressof(s);
}
inline Node get_vertex_from_ref(vertex_type_tag_t<Node>, Service& s) noexcept {
  return Node(s);
}
} // namespace detail

/// Represents the data gathered during the dfs
struct DFSData {
  bool acyclic{true};

  FlatSet<Service*> visited;
  FlatSet<Service*> stack;
  std::vector<Service*> next;

  bool empty() const noexcept {
    return acyclic && visited.empty() && stack.empty() && next.empty();
  }

  void clear() {
    acyclic = true;
    visited.clear();
    stack.clear();
    next.clear();
  }

  void ancestors(std::initializer_list<Service*> ancestors) {
    visited.insert(ancestors.begin(), ancestors.end());
    stack.insert(ancestors.begin(), ancestors.end());
  }

  /// Returns true if the dfs visited detected no cycles
  explicit operator bool() const noexcept {
    return acyclic;
  }
};

/// Implements a simple RAII guard for clearing dfs_data after usage
class DFSScope {
public:
  explicit DFSScope(DFSData& data)
    : data_(data) {
    IDLE_ASSERT(data.empty());
  }
  ~DFSScope() {
    data_.clear();
  }

  DFSScope(DFSScope const&) = delete;
  DFSScope& operator=(DFSScope const&) = delete;

private:
  DFSData& data_;
};

/// Represents flags to customize \see dfs
enum class DFSFlags : std::uint8_t {
  /// Traverse all paths instead of stopping at nodes seen already
  flag_traverse_all_paths,
  /// Don't targets of edges if the node was visited before already.
  /// This effectively cancels cycles in a post-order way.
  flag_post_cancel_cycles,
};

/// Performs a dfs visit over the graph from the given node.
///
/// This implementation makes use of the fact that after every few nodes
/// the visit passes through a service. Through that we can fast forward
/// on normal edges while checking only when encountering a service
/// for possible cycles and path repetition which is much more space efficient
/// than the default boost::depth_first_visit with a huge color map.
///
/// Beside that, this function allows simple filtering of edges.
template <typename Graph, typename Visitor = detail::none2,
          typename Filter = detail::none1>
void dfs(Graph const& graph, Service* start, DFSData& data,
         Visitor&& visitor = {}, Filter&& filter = {},
         BitSet<DFSFlags> flags = BitSet<DFSFlags>::none()) {

  using vertex_t = typename boost::graph_traits<Graph>::vertex_descriptor;
  using edge_t = typename boost::graph_traits<Graph>::edge_descriptor;

  auto& next = data.next;
  auto& visited = data.visited;
  auto& stack = data.stack;

  next.push_back(start);

  detail::wrap<std::decay_t<Visitor>> vis(std::forward<Visitor>(visitor));

  while (!next.empty()) {
    Service* const top = next.back();

    if (stack.erase(top)) {
      IDLE_ASSERT(visited.find(top) != visited.end());
      next.pop_back();
      continue;
    }

    visited.insert(top);

    IDLE_ASSERT(stack.find(top) == stack.end());
    stack.insert(top);

    if (!static_cast<bool>(vis(dfs_event_visit{}, top))) {
      continue;
    }

    auto const out = out_edges(top, graph);
    for (auto itr = out.first; itr != out.second;) {
      edge_t const edge = *itr;
      ++itr;

      if (!filter(edge)) {
        continue;
      }

      vertex_t const tar = target(edge, graph);
      if (visited.find(tar) == visited.end()) {
        next.push_back(tar);
      } else if (stack.find(tar) != stack.end()) {
        if (!flags.contains(DFSFlags::flag_post_cancel_cycles)) {
          data.acyclic = false;
          return;
        } else {
          vis(dfs_event_visit{}, tar);
        }
      } else if (flags.contains(DFSFlags::flag_traverse_all_paths)) {
        IDLE_ASSERT(stack.find(tar) == stack.end());
        next.push_back(tar);
      }
    }
  }
}
} // namespace idle

#endif // IDLE_CORE_DETAIL_GRAPH_DFS_HPP_INCLUDED
