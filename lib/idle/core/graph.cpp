
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

#include <cctype>
#include <cstddef>
#include <stack>
#include <type_traits>
#include <boost/function_output_iterator.hpp>
#include <boost/graph/reverse_graph.hpp>
// #include <boost/graph/topological_sort.hpp>
#include <idle/core/casting.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/algorithm.hpp>
#include <idle/core/detail/service_impl.hpp>
#include <idle/core/detail/unordered_set.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/external/boost/graph.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/iterators.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/iterator_facade.hpp>
#include <idle/core/views/filter.hpp>

#ifdef _MSC_VER
#  pragma warning(disable : 4458)
#  pragma warning(disable : 4459)
#endif // _MSC_VER

#ifndef NDEBUG
#  define IDLE_DETAIL_GRAPH_VERIFY
// #  define IDLE_DETAIL_GRAPH_VERIFY_DEGREE_EACH
#endif // NDEBUG

namespace idle {
/// Filters out weak usages
static auto filter_usages(GraphFlags flags) noexcept {
  IDLE_ASSERT(flags.has_any_usage_edge());
  IDLE_ASSERT(!flags.has_all_usage_edge());
  IDLE_ASSERT(!flags.has_usage_weak_edge() || !flags.has_usage_strong_edge());
  IDLE_ASSERT(flags.has_usage_weak_edge() || flags.has_usage_strong_edge());

  return views::filter([weak = flags.has_usage_weak_edge()](Usage const& u) {
    return weak == u.isWeak();
  });
}

std::ostream& operator<<(std::ostream& os, Node const& n) {
  visit_node(n, [&](auto const& current) {
    os << current;
  });
  return os;
}

bool Node::has_owner() const noexcept {
  switch (index()) {
    case index_of<Service>():
    case index_of<Import>():
    case index_of<Export>():
      return true;
    default:
      return false;
  }
}

Service& Node::owner() const noexcept {
  switch (index()) {
    case index_of<Service>(): {
      return get<Service>();
    }
    case index_of<Import>(): {
      return get<Import>().owner();
    }
    case index_of<Export>(): {
      return get<Export>().owner();
    }
    case index_of<Usage>():
      // no break
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

static Guid node_guid(Service const& n) {
  return Guid::of(n);
}
static Guid node_guid(Part const& n) {
  return Guid::of(n);
}
static Guid node_guid(Usage const& /*n*/) {
  IDLE_DETAIL_UNREACHABLE();
}

Guid Node::guid() const noexcept {
  return visit_node(*this, [](auto const& n) {
    return node_guid(n);
  });
}

static void print_node_name_impl(std::ostream& os, Service const& n) {
  os << n.name();
}
static void print_node_name_impl(std::ostream& os, Part const& n) {
  os << n.partName();
}
static void print_node_name_impl(std::ostream& os, Usage const& n) {
  os << n.name();
}

void Node::print_node_name(std::ostream& os) const {
  visit_node(*this, [&](auto const& current) {
    print_node_name_impl(os, current);
  });
}

template <typename NodeOrEdge>
void set_next_from_service(NodeOrEdge& ne, Service& current) noexcept {
  if (auto next = transitive_services(current).next()) {
    ne.set(next.front());
  } else {
    ne.reset();
  }
}

void node_iterator::increment() noexcept {
  IDLE_ASSERT(current_);

  // Iterate over:
  // - 1. > service
  //     - 2. > parts
  //     - 3. > usage on exports
  switch (current_.index()) {
    case Node::index_of<Service>(): {
      Service& me = current_.get<Service>();
      if (auto parts = me.parts()) {
        current_ = parts.front();
      } else {
        set_next_from_service(current_, me);
      }
      break;
    }
    case Node::index_of<Import>(): {
      Import& me = current_.get<Import>();
      if (auto next = me.neighbors().next()) {
        current_ = next.front();
      } else {
        set_next_from_service(current_, me.owner());
      }
      break;
    }
    case Node::index_of<Export>(): {
      Export& me = current_.get<Export>();
      if (auto exported = me.exports()) {
        current_ = exported.front();
      } else if (auto next = me.neighbors().next()) {
        current_ = next.front();
      } else {
        set_next_from_service(current_, me.owner());
      }
      break;
    }
    case Node::index_of<Usage>(): {
      Usage& me = current_.get<Usage>();
      if (auto next_used = me.used_neighbors().next()) {
        current_ = next_used.front();
      } else if (auto next = me.used().neighbors().next()) {
        current_ = next.front();
      } else {
        set_next_from_service(current_, me.used().owner());
      }
      break;
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

EdgeRelation edge_relation_between(Guid source, Guid target) noexcept {
  if (source.cluster() == target.cluster()) {
    return EdgeRelation::implicit;
  } else if (target.isCluster()) {
    return EdgeRelation::parent;
  } else {
    return EdgeRelation::dependency;
  }
}

EdgeRelation Edge::relation() const noexcept {
  switch (target().index()) {
    case Node::index_of<Service>(): {
      if (source().is_exporter()) {
        return EdgeRelation::parent;
      } else {
        return EdgeRelation::implicit;
      }
    }
    case Node::index_of<Import>(): {
      if (source().is<Usage>()) {
        return EdgeRelation::dependency;
      } else {
        return EdgeRelation::implicit;
      }
    }
    case Node::index_of<Export>(): {
      return EdgeRelation::implicit;
    }
    case Node::index_of<Usage>(): {
      return EdgeRelation::dependency;
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

static bool is_service_active(Service const& current) noexcept {
  return current.state().isRunning();
}

bool Edge::is_active() const noexcept {
  switch (source().index()) {
    case Node::index_of<Service>():
    case Node::index_of<Import>():
    case Node::index_of<Export>(): {
      return is_service_active(source().owner());
    }
    case Node::index_of<Usage>(): {
      return is_service_active(source().get<Usage>().used().owner());
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

bool Edge::isWeak() const noexcept {
  if (source().is_usage()) {
    return source().get<Usage>().isWeak();
  } else {
    return target().is_usage() && target().get<Usage>().isWeak();
  }
}

EdgeProperties Edge::properties() const noexcept {
  return EdgeProperties{relation(), is_active(), isWeak()};
}

std::ostream& operator<<(std::ostream& os, Edge const& current) {
  print(os, FMT_STRING("'{}' -> '{}'"), current.source(), current.target());
  return os;
}

struct dir_out_tag_t {};
struct dir_in_tag_t {};

using default_edge_traversal_dir = dir_out_tag_t;

static Node const& iteration_node_of(dir_out_tag_t, Edge const& e) noexcept {
  return e.source();
}
static Node const& iteration_node_of(dir_in_tag_t, Edge const& e) noexcept {
  return e.target();
}
static Node const& current_itr_of(dir_out_tag_t, Edge const& e) noexcept {
  return e.target();
}
static Node const& current_itr_of(dir_in_tag_t, Edge const& e) noexcept {
  return e.source();
}
static Edge make_edge(dir_out_tag_t, Node const& current,
                      Node const& itr) noexcept {
  return Edge{current, itr};
}
static Edge make_edge(dir_in_tag_t, Node const& current,
                      Node const& itr) noexcept {
  return Edge{itr, current};
}

template <typename>
struct edge_trait_t;

template <>
struct edge_trait_t<Service> {
  static bool has_parent_edge(dir_out_tag_t, Service const& current,
                              GraphFlags flags) noexcept {
    return isa<Import>(current.parent()) && flags.has_child_importer_edge() &&
           !current.isRoot();
  }
  static Node begin(dir_out_tag_t tag, Service& current,
                    GraphFlags flags) noexcept {
    if (has_parent_edge(tag, current, flags)) {
      return Node(cast<Import>(current.parent()));
    } else if (flags.has_owner_exporter_edge()) {
      if (auto exports = current.exports()) {
        return Node(exports.front());
      }
    }
    return {};
  }
  static Node advance(dir_out_tag_t tag, Service& current, Node const& itr,
                      GraphFlags flags) noexcept {

    if (flags.has_owner_exporter_edge()) {
      switch (itr.index()) {
        case Node::index_of<Import>(): {
          IDLE_ASSERT((current.parent() == itr.get<Import>()) &&
                      has_parent_edge(tag, current, flags));

          if (auto exports = current.exports()) {
            return Node(exports.front());
          }
          return {};
        }
        case Node::index_of<Export>(): {
          if (auto next = itr.get<Export>().exportNeighbors().next()) {
            return Node(next.front());
          } else {
            return {};
          }
        }
        default: {
          IDLE_DETAIL_UNREACHABLE();
        }
      }
    } else {
      return {};
    }
  }
  static std::size_t degree(dir_out_tag_t tag, Service const& current,
                            GraphFlags flags) noexcept {
    std::size_t count = 0;

    if (has_parent_edge(tag, current, flags)) {
      count += 1;
    }

    if (flags.has_owner_exporter_edge()) {
      count += current.exports().size();
    }

    return count;
  }

  static bool has_parent_edge(dir_in_tag_t, Service const& current,
                              GraphFlags flags) noexcept {
    return isa<Export>(current.parent()) && flags.has_exporter_child_edge() &&
           !current.isRoot();
  }
  static Node begin(dir_in_tag_t tag, Service& current,
                    GraphFlags flags) noexcept {
    if (has_parent_edge(tag, current, flags)) {
      return Node(cast<Export>(current.parent()));
    } else if (flags.has_importer_owner_edge()) {
      if (auto imports = current.imports()) {
        return Node(imports.front());
      }
    }
    return {};
  }
  static Node advance(dir_in_tag_t tag, Service& current, Node const& itr,
                      GraphFlags flags) noexcept {

    if (flags.has_importer_owner_edge()) {
      switch (itr.index()) {
        case Node::index_of<Export>(): {
          IDLE_ASSERT((current.parent() == itr.get<Export>()) &&
                      has_parent_edge(tag, current, flags));

          if (auto imports = current.imports()) {
            return Node(imports.front());
          }
          return {};
        }
        case Node::index_of<Import>(): {
          if (auto next = itr.get<Import>().importNeighbors().next()) {
            return Node(next.front());
          } else {
            return {};
          }
        }
        default: {
          IDLE_DETAIL_UNREACHABLE();
        }
      }
    } else {
      return {};
    }
  }
  static std::size_t degree(dir_in_tag_t tag, Service const& current,
                            GraphFlags flags) noexcept {
    std::size_t count = 0;

    if (has_parent_edge(tag, current, flags)) {
      count += 1;
    }

    if (flags.has_importer_owner_edge()) {
      count += current.imports().size();
    }

    return count;
  }
};

template <>
struct edge_trait_t<Import> {
  static Node begin(dir_out_tag_t, Import& current, GraphFlags flags) noexcept {
    if (flags.has_importer_owner_edge()) {
      return Node(current.owner());
    } else {
      return {};
    }
  }

  static Node advance(dir_out_tag_t, Import&, Node const&,
                      GraphFlags) noexcept {
    return {};
  }
  static std::size_t degree(dir_out_tag_t, Import const&,
                            GraphFlags flags) noexcept {
    if (flags.has_importer_owner_edge()) {
      return 1U;
    } else {
      return 0;
    }
  }

  static Node begin(dir_in_tag_t, Import& current, GraphFlags flags) noexcept {

    if (flags.has_child_importer_edge()) {
      if (auto children = current.children()) {
        IDLE_ASSERT(children.front().state().isInitializedUnsafe());
        return Node(children.front());
      }
    }

    if (flags.has_usage_importer_edge()) {
      if (auto users = current.imports()) {
        return Node(users.front());
      }
    }

    return {};
  }
  static Node advance(dir_in_tag_t, Import& current, Node const& itr,
                      GraphFlags flags) noexcept {
    switch (itr.index()) {
      case Node::index_of<Service>(): {
        IDLE_ASSERT(flags.has_child_importer_edge());

        if (auto next = itr.get<Service>().neighbors().next()) {
          IDLE_ASSERT(next.front().state().isInitializedUnsafe());
          return Node(next.front());
        }

        if (flags.has_usage_importer_edge()) {
          if (flags.has_all_usage_edge()) {
            if (auto users = current.imports()) {
              return Node(users.front());
            }
          } else {
            if (auto users = current.imports() | filter_usages(flags)) {
              return Node(users.front());
            }
          }
        }

        return {};
      }
      case Node::index_of<Usage>(): {
        IDLE_ASSERT(flags.has_usage_importer_edge());
        IDLE_ASSERT(flags.has_usage_weak_edge() || !itr.get<Usage>().isWeak());
        IDLE_ASSERT(flags.has_usage_strong_edge() || itr.get<Usage>().isWeak());

        Usage& u = itr.get<Usage>();
        if (flags.has_all_usage_edge()) {
          if (auto next = u.user_neighbors().next()) {
            return Node(next.front());
          }
        } else {
          if (auto next = u.user_neighbors().next() | filter_usages(flags)) {
            return Node(next.front());
          }
        }
        return {};
      }
      default: {
        IDLE_DETAIL_UNREACHABLE();
      }
    }
  }
  static std::size_t degree(dir_in_tag_t, Import const& current,
                            GraphFlags flags) noexcept {
    std::size_t count = 0;

    if (flags.has_child_importer_edge()) {
      count += current.children().size();
    }

    if (flags.has_usage_importer_edge()) {
      if (flags.has_all_usage_edge()) {
        count += current.imports().size();
      } else {
        count += (current.imports() | filter_usages(flags)).size();
      }
    }

    return count;
  }
};

template <>
struct edge_trait_t<Export> {
  static Node begin(dir_out_tag_t, Export& current, GraphFlags flags) noexcept {
    if (flags.has_exporter_child_edge()) {
      if (auto children = current.children()) {
        IDLE_ASSERT(children.front().state().isInitializedUnsafe());
        return Node(children.front());
      }
    }

    if (flags.has_exporter_usage_edge()) {
      if (flags.has_all_usage_edge()) {
        if (auto exported = current.exports()) {
          return Node(exported.front());
        }
      } else {
        if (auto exported = current.exports() | filter_usages(flags)) {
          return Node(exported.front());
        }
      }
    }

    return {};
  }
  static Node advance(dir_out_tag_t, Export& current, Node const& itr,
                      GraphFlags flags) noexcept {

    switch (itr.index()) {
      case Node::index_of<Service>(): {
        IDLE_ASSERT(flags.has_exporter_child_edge());
        if (auto next = itr.get<Service>().neighbors().next()) {
          IDLE_ASSERT(next.front().state().isInitializedUnsafe());
          return Node(next.front());
        }

        if (flags.has_exporter_usage_edge()) {
          if (auto exported = current.exports()) {
            return Node(exported.front());
          }
        }

        return {};
      }
      case Node::index_of<Usage>(): {
        IDLE_ASSERT(flags.has_exporter_usage_edge());
        IDLE_ASSERT(flags.has_usage_weak_edge() || !itr.get<Usage>().isWeak());
        IDLE_ASSERT(flags.has_usage_strong_edge() || itr.get<Usage>().isWeak());

        Usage& u = itr.get<Usage>();
        if (flags.has_all_usage_edge()) {
          if (auto next = u.used_neighbors().next()) {
            return Node(next.front());
          }
        } else {
          if (auto next = u.used_neighbors().next() | filter_usages(flags)) {
            return Node(next.front());
          }
        }

        return {};
      }
      default: {
        IDLE_DETAIL_UNREACHABLE();
      }
    }
  }
  static std::size_t degree(dir_out_tag_t, Export const& current,
                            GraphFlags flags) noexcept {
    std::size_t count = 0;

    if (flags.has_exporter_child_edge()) {
      count += current.children().size();
    }

    if (flags.has_exporter_usage_edge()) {
      if (flags.has_all_usage_edge()) {
        count += current.exports().size();
      } else {
        count += (current.exports() | filter_usages(flags)).size();
      }
    }

    return count;
  }

  static Node begin(dir_in_tag_t, Export& current, GraphFlags flags) noexcept {
    if (flags.has_owner_exporter_edge()) {
      return Node(current.owner());
    } else {
      return {};
    }
  }
  static Node advance(dir_in_tag_t, Export& /*current*/, Node const& /*itr*/,
                      GraphFlags flags) noexcept {
    IDLE_ASSERT(flags.has_owner_exporter_edge());
    return {};
  }
  static std::size_t degree(dir_in_tag_t, Export const& /*current*/,
                            GraphFlags flags) noexcept {
    if (flags.has_owner_exporter_edge()) {
      return 1U;
    } else {
      return 0U;
    }
  }
};

template <>
struct edge_trait_t<Usage> {
  static Node begin(dir_out_tag_t, Usage& current, GraphFlags flags) noexcept {
    if (flags.has_usage_importer_edge() &&
        (flags.has_usage_weak_edge() || !current.isWeak()) &&
        (flags.has_usage_strong_edge() || current.isWeak())) {
      IDLE_ASSERT(current.user().owner() != current.used().owner());
      return Node(current.user());
    } else {
      return {};
    }
  }
  static Node advance(dir_out_tag_t, Usage& current, Node const& /*itr*/,
                      GraphFlags flags) noexcept {
    (void)current;

    IDLE_ASSERT(flags.has_usage_importer_edge());
    IDLE_ASSERT(flags.has_usage_weak_edge() || !current.isWeak());
    IDLE_ASSERT(flags.has_usage_strong_edge() || current.isWeak());
    return {};
  }
  static std::size_t degree(dir_out_tag_t, Usage& current,
                            GraphFlags flags) noexcept {
    (void)current;

    if (flags.has_usage_importer_edge() &&
        (flags.has_usage_weak_edge() || !current.isWeak()) &&
        (flags.has_usage_strong_edge() || current.isWeak())) {
      return 1U;
    } else {
      return {};
    }
  }

  static Node begin(dir_in_tag_t, Usage& current, GraphFlags flags) noexcept {
    if (flags.has_exporter_usage_edge() &&
        (flags.has_usage_weak_edge() || !current.isWeak()) &&
        (flags.has_usage_strong_edge() || current.isWeak())) {
      IDLE_ASSERT(current.user().owner() != current.used().owner());
      return Node(current.used());
    } else {
      return {};
    }
  }
  static Node advance(dir_in_tag_t, Usage& current, Node const& /*itr*/,
                      GraphFlags flags) noexcept {
    IDLE_ASSERT(flags.has_exporter_usage_edge());
    IDLE_ASSERT(flags.has_usage_weak_edge() || !current.isWeak());
    IDLE_ASSERT(flags.has_usage_strong_edge() || current.isWeak());
    return {};
  }
  static std::size_t degree(dir_in_tag_t, Usage& current,
                            GraphFlags flags) noexcept {
    if (flags.has_exporter_usage_edge() &&
        (flags.has_usage_weak_edge() || !current.isWeak()) &&
        (flags.has_usage_strong_edge() || current.isWeak())) {
      return 1U;
    } else {
      return {};
    }
  }
};

template <typename Tag>
Edge advance_edge(Tag tag, Edge const& e, GraphFlags flags) {
  return visit_node(iteration_node_of(tag, e), [&](auto& current) mutable {
    using trait_t = edge_trait_t<std::decay_t<decltype(current)>>;
    if (Node next = trait_t::advance(tag, current, current_itr_of(tag, e),
                                     flags)) {
      return make_edge(tag, Node(current), next);
    } else {
      return Edge{};
    }
  });
}

template <typename Tag>
Edge first_edge_of(Tag tag, Node const& current, GraphFlags flags) noexcept {
  return visit_node(current, [tag, flags](auto& current) mutable {
    using trait_t = edge_trait_t<std::decay_t<decltype(current)>>;
    if (Node first = trait_t::begin(tag, current, flags)) {
      return make_edge(tag, Node(current), first);
    } else {
      return Edge{};
    }
  });
}

template <typename Tag>
Edge first_edge_in_range(Tag tag, node_iterator begin, node_iterator end,
                         GraphFlags flags) noexcept {
  for (; begin != end; ++begin) {
    if (Edge first = first_edge_of(tag, *begin, flags)) {
      return first;
    }
  }
  return Edge{};
}

template <typename Tag>
std::size_t node_degree(Tag tag, Node const& current,
                        GraphFlags flags) noexcept {
  return visit_node(current, [tag, flags](auto& current) mutable {
    using trait_t = edge_trait_t<std::decay_t<decltype(current)>>;

#if !defined(NDEBUG) && defined(IDLE_DETAIL_GRAPH_DEBUG)
    {
      std::size_t edge_count = 0;
      for (node itr = trait_t::begin(tag, current, flags); bool(itr);
           itr = trait_t::advance(tag, current, itr, flags)) {

        ++edge_count;
      }

      std::size_t degree_count = trait_t::degree(tag, current, flags);
      IDLE_ASSERT(edge_count == degree_count);
    }
#endif

    return trait_t::degree(tag, current, flags);
  });
}

void edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);

  constexpr default_edge_traversal_dir tag;

  if (Edge next = advance_edge(tag, current_, flags_)) {
    current_ = std::move(next);
    return;
  }

  node_iterator begin{iteration_node_of(tag, current_)};
  node_iterator const end;

  // Start searching at the next node
  ++begin;

  current_ = first_edge_in_range(tag, begin, end, flags_);
}

void out_edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);
  current_ = advance_edge(dir_out_tag_t{}, current_, flags_);
}

void in_edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);
  current_ = advance_edge(dir_in_tag_t{}, current_, flags_);
}

template <typename Graph, typename Node>
std::size_t edge_dir_count(dir_out_tag_t, Graph const& graph,
                           Node const& node) noexcept {
  return out_degree(node, graph);
}
template <typename Graph, typename Node>
std::size_t edge_dir_count(dir_in_tag_t, Graph const& graph,
                           Node const& node) noexcept {
  return in_degree(node, graph);
}

template <typename Graph>
void init_graph(Graph& graph, std::size_t& num_vertices,
                std::size_t& num_edges) noexcept {

  IDLE_ASSERT(num_vertices == 0);
  IDLE_ASSERT(num_edges == 0);

#if !defined(NDEBUG) && defined(IDLE_DETAIL_GRAPH_VERIFY)
  std::size_t all_in_degree = 0;
  std::size_t all_out_degree = 0;
#endif

  for (auto const& current : vertices(graph)) {
    ++num_vertices;
    num_edges += edge_dir_count(default_edge_traversal_dir{}, graph, current);

#if !defined(NDEBUG) && defined(IDLE_DETAIL_GRAPH_VERIFY)
    std::size_t const in_degree_count = in_degree(current, graph);
    std::size_t const test_in_edges = in_edges(current, graph).size();
    IDLE_ASSERT(in_degree_count == test_in_edges);
    all_in_degree += in_degree_count;

    std::size_t const test_out_degree = out_degree(current, graph);
    std::size_t const test_out_edges = out_edges(current, graph).size();
    IDLE_ASSERT(test_out_degree == test_out_edges);
    all_out_degree += test_out_degree;
#endif
  }

#if !defined(NDEBUG) && defined(IDLE_DETAIL_GRAPH_VERIFY)
  IDLE_ASSERT(num_vertices == vertices(graph).size());

  std::size_t const test_edge_size = edges(graph).size();

  IDLE_ASSERT(num_edges == test_edge_size);
  IDLE_ASSERT(num_edges == all_in_degree);
  IDLE_ASSERT(num_edges == all_out_degree);
#endif
}

DependencyGraph::DependencyGraph(Context& root, GraphFlags flags) noexcept
  : DependencyGraph(root, graph_view, flags) {
  init_graph(*this, num_vertices_, num_edges_);
}

DependencyGraph::DependencyGraph(Context& root, graph_view_t,
                                 GraphFlags flags) noexcept
  : context_(root)
  , flags_(flags) {}

IndexedDependencyGraph::IndexedDependencyGraph(Context& root)
  : DependencyGraph(root) {

  std::size_t index = 0;
  for (Node current : vertices(*this)) {
    mapping_.insert(std::make_pair(std::move(current), index));
    ++index;
  }

#if !defined(NDEBUG) && defined(IDLE_DETAIL_GRAPH_VERIFY)
  IDLE_ASSERT(index == num_vertices(*this));
#endif
}

std::size_t IndexedDependencyGraph::lookup_id(Node const& n) const noexcept {
  auto const itr = mapping_.find(n);
  IDLE_ASSERT(itr != mapping_.end());
  return itr->second;
}

Range<node_iterator> vertices(DependencyGraph const& g) {
  return {node_iterator(Node(*static_cast<Service*>(&g.get_context()))), {}};
}

Range<edge_iterator> edges(DependencyGraph const& g) {
  auto const nodes = vertices(g);
  IDLE_ASSERT(!nodes.empty());
  IDLE_ASSERT(!bool(Edge{}));

  return {edge_iterator{first_edge_in_range(default_edge_traversal_dir{},
                                            nodes.first, nodes.second,
                                            g.get_flags()),
                        g.get_flags()},
          {}};
}

Range<out_edge_iterator> out_edges(Node const& v, DependencyGraph const& g) {
  return {out_edge_iterator{first_edge_of(dir_out_tag_t{}, v, g.get_flags()),
                            g.get_flags()},
          {}};
}

std::size_t out_degree(Node const& v, DependencyGraph const& g) {
  return node_degree(dir_out_tag_t{}, v, g.get_flags());
}

Range<in_edge_iterator> in_edges(Node const& v, DependencyGraph const& g) {
  return {in_edge_iterator{first_edge_of(dir_in_tag_t{}, v, g.get_flags()),
                           g.get_flags()},
          {}};
}

std::size_t in_degree(Node const& v, DependencyGraph const& g) {
  return node_degree(dir_in_tag_t{}, v, g.get_flags());
}

std::size_t degree(Node const& v, DependencyGraph const& g) {
  return in_degree(v, g) + out_degree(v, g);
}

static Node next_child_node(Node const& n) noexcept {
  switch (n.index()) {
    case Node::index_of<Service>(): {
      Service& current = n.get<Service>();
      if (auto extensions = current.imports()) {
        return Node(extensions.front());
      } else if (auto interfaces = current.exports()) {
        return Node(interfaces.front());
      } else {
        return {};
      }
    }
    case Node::index_of<Import>(): {
      Import& current = n.get<Import>();
      if (auto extensions = current.importNeighbors().next()) {
        return Node(extensions.front());
      } else if (auto interfaces = current.owner().exports()) {
        return Node(interfaces.front());
      } else {
        return {};
      }
    }
    case Node::index_of<Export>(): {
      Export& current = n.get<Export>();
      if (auto interfaces = current.exportNeighbors().next()) {
        return Node(interfaces.front());
      } else {
        return {};
      }
    }
    case Node::index_of<Usage>():
      // no break
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

class inner_node_iterator
  : public detail::graph_iterator<inner_node_iterator, Node> {

public:
  using graph_iterator<inner_node_iterator, Node>::graph_iterator;

  void increment() noexcept {
    IDLE_ASSERT(current_);
    current_ = next_child_node(current_);
  }
};

Range<inner_node_iterator> inner_nodes(Service& current) {
  return make_range(inner_node_iterator(Node(current)), {});
}

template <typename Tag>
Edge first_outer_edge_of(Tag tag, Service& current, GraphFlags flags) {
  for (Node const& itr : inner_nodes(current))
    if (Edge first = first_edge_of(tag, itr, flags)) {
      return first;
    }
  return {};
}

template <typename Tag, typename Range>
Edge first_outer_edge_in_range(Tag tag, Range&& range, GraphFlags flags) {

  for (Service& current : range) {
    if (Edge first = first_outer_edge_of(tag, current, flags)) {
      return first;
    }
  }
  return Edge{};
}

template <typename Tag>
Edge advance_outer_edge(Tag tag, Edge const& e, GraphFlags flags) {
  if (Edge next = advance_edge(tag, e, flags)) {
    return next;
  }

  Node const& inner = iteration_node_of(tag, e);
  auto const range = make_range(inner_node_iterator(inner), {}).next();
  for (Node const& neighbor : range) {
    if (Edge first = first_edge_of(tag, neighbor, flags)) {
      return first;
    }
  }

  return {};
}

service_node_iterator::service_node_iterator() = default;

void service_edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);

  constexpr default_edge_traversal_dir tag;

  if (Edge next = advance_outer_edge(tag, current_, flags_)) {
    current_ = next;
  } else {
    Node const& inner = iteration_node_of(tag, current_);
    Service& owner = inner.owner();
    current_ = first_outer_edge_in_range(tag, transitive_services(owner).next(),
                                         flags_);
  }
}

void service_out_edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);
  constexpr dir_out_tag_t dir;

  current_ = advance_outer_edge(dir, current_, flags_);
}

void service_in_edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);
  current_ = advance_outer_edge(dir_in_tag_t{}, current_, flags_);
}

ServiceDependencyGraph::ServiceDependencyGraph(Context& root,
                                               GraphFlags flags) noexcept
  : ServiceDependencyGraph(root, graph_view, flags) {
  init_graph(*this, num_vertices_, num_edges_);
}

ServiceDependencyGraph::ServiceDependencyGraph(Context& root, graph_view_t,
                                               GraphFlags flags) noexcept
  : context_(root)
  , flags_(GraphFlags(flags | GraphFlags::service_graph())) {}

Range<service_node_iterator> vertices(ServiceDependencyGraph const& g) {
  return make_range(service_node_iterator(g.get_context()), {});
}

Service* source(Edge const& e, ServiceDependencyGraph const&) {
  Node const& n = e.source();

  switch (n.index()) {
    case Node::index_of<Service>(): {
      return &(n.get<Service>());
    }
    case Node::index_of<Import>(): {
      return &(n.get<Import>().owner());
    }
    case Node::index_of<Export>(): {
      return &(n.get<Export>().owner());
    }
    case Node::index_of<Usage>(): {
      return &(n.get<Usage>().used().owner());
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

Service* target(Edge const& e, ServiceDependencyGraph const&) {
  Node const& n = e.target();

  switch (n.index()) {
    case Node::index_of<Service>(): {
      return &(n.get<Service>());
    }
    case Node::index_of<Import>(): {
      return &(n.get<Import>().owner());
    }
    case Node::index_of<Export>(): {
      return &(n.get<Export>().owner());
    }
    case Node::index_of<Usage>(): {
      return &(n.get<Usage>().user().owner());
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

Range<service_edge_iterator> edges(ServiceDependencyGraph const& g) {
  constexpr default_edge_traversal_dir dir;
  auto const first = first_outer_edge_in_range(
      dir, transitive_services(g.get_context()), g.get_flags());

  return make_range(service_edge_iterator(first, g.get_flags()), {});
}

Range<service_out_edge_iterator> out_edges(Service* v,
                                           ServiceDependencyGraph const& g) {
  IDLE_ASSERT(v);
  auto const first = first_outer_edge_of(dir_out_tag_t{}, *v, g.get_flags());
  return make_range(service_out_edge_iterator(first, g.get_flags()), {});
}

std::size_t out_degree(Service* v, ServiceDependencyGraph const& g) {
  std::size_t count = 0;

  for (Node current : inner_nodes(*v)) {
    count += node_degree(dir_out_tag_t{}, current, g.get_flags());
  }

  return count;
}

Range<service_in_edge_iterator> in_edges(Service* v,
                                         ServiceDependencyGraph const& g) {

  IDLE_ASSERT(v);
  auto const first = first_outer_edge_of(dir_in_tag_t{}, *v, g.get_flags());
  return make_range(service_in_edge_iterator(first, g.get_flags()), {});
}

std::size_t in_degree(Service* v, ServiceDependencyGraph const& g) {
  IDLE_ASSERT(v);
  std::size_t count = 0;

  for (Node const& current : inner_nodes(*v)) {
    count += node_degree(dir_in_tag_t{}, current, g.get_flags());
  }

  return count;
}

std::size_t degree(Service* v, ServiceDependencyGraph const& g) {
  return in_degree(v, g) + out_degree(v, g);
}

cluster_node_iterator::cluster_node_iterator() = default;

void cluster_edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);

  constexpr default_edge_traversal_dir tag;

  if (Edge next = advance_outer_edge(tag, current_, flags_)) {
    current_ = next;
  } else {
    Node const& inner = iteration_node_of(tag, current_);
    Service& owner = inner.owner();
    current_ = first_outer_edge_in_range(tag, transitive_services(owner).next(),
                                         flags_);
  }
}

void cluster_out_edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);
  constexpr dir_out_tag_t dir;

  if (Edge next = advance_outer_edge(dir, current_, flags_)) {
    current_ = next;
  } else {
    Node const& inner = iteration_node_of(dir, current_);
    Service& owner = inner.owner();

    current_ = first_outer_edge_in_range(dir, cluster_members(owner).next(),
                                         flags_);
  }
}

void cluster_in_edge_iterator::increment() noexcept {
  IDLE_ASSERT(current_);
  constexpr dir_in_tag_t dir;

  if (Edge next = advance_outer_edge(dir, current_, flags_)) {
    current_ = next;
  } else {
    Node const& inner = iteration_node_of(dir, current_);
    Service& owner = inner.owner();

    current_ = first_outer_edge_in_range(dir, cluster_members(owner).next(),
                                         flags_);
  }
}

ClusterDependencyGraph::ClusterDependencyGraph(Context& root,
                                               GraphFlags flags) noexcept
  : ClusterDependencyGraph(root, graph_view, flags) {
  init_graph(*this, num_vertices_, num_edges_);
}

ClusterDependencyGraph::ClusterDependencyGraph(Context& root, graph_view_t,
                                               GraphFlags flags) noexcept
  : context_(root)
  , flags_(GraphFlags(flags | GraphFlags::service_graph() |
                      GraphFlags::external_graph())) {}

Range<cluster_node_iterator> vertices(ClusterDependencyGraph const& g) {
  return make_range(cluster_node_iterator(g.get_context()), {});
}

Service* source(Edge const& e, ClusterDependencyGraph const&) {
  Node const& n = e.source();

  switch (n.index()) {
    case Node::index_of<Service>(): {
      return &get_cluster_head_of(n.get<Service>());
    }
    case Node::index_of<Import>(): {
      return &get_cluster_head_of(n.get<Import>().owner());
    }
    case Node::index_of<Export>(): {
      return &get_cluster_head_of(n.get<Export>().owner());
    }
    case Node::index_of<Usage>(): {
      return &get_cluster_head_of(n.get<Usage>().used().owner());
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

Service* target(Edge const& e, ClusterDependencyGraph const&) {
  Node const& n = e.target();

  switch (n.index()) {
    case Node::index_of<Service>(): {
      return &get_cluster_head_of(n.get<Service>());
    }
    case Node::index_of<Import>(): {
      return &get_cluster_head_of(n.get<Import>().owner());
    }
    case Node::index_of<Export>(): {
      return &get_cluster_head_of(n.get<Export>().owner());
    }
    case Node::index_of<Usage>(): {
      return &get_cluster_head_of(n.get<Usage>().user().owner());
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

Range<cluster_edge_iterator> edges(ClusterDependencyGraph const& g) {
  constexpr default_edge_traversal_dir dir;

  auto const first = first_outer_edge_in_range(
      dir, transitive_services(g.get_context()), g.get_flags());
  return make_range(cluster_edge_iterator(first, g.get_flags()), {});
}

Range<cluster_out_edge_iterator> out_edges(Service* v,
                                           ClusterDependencyGraph const& g) {
  IDLE_ASSERT(v);

  for (Service& children : cluster_members(*v)) {
    if (auto const first = first_outer_edge_of(dir_out_tag_t{}, children,
                                               g.get_flags())) {
      return make_range(cluster_out_edge_iterator(first, g.get_flags()), {});
    }
  }

  return {};
}

std::size_t out_degree(Service* v, ClusterDependencyGraph const& g) {
  IDLE_ASSERT(v);

  std::size_t count = 0;

  for (Service& children : cluster_members(*v)) {
    for (Node current : inner_nodes(children)) {
      count += node_degree(dir_out_tag_t{}, current, g.get_flags());
    }
  }

  return count;
}

Range<cluster_in_edge_iterator> in_edges(Service* v,
                                         ClusterDependencyGraph const& g) {
  IDLE_ASSERT(v);

  for (Service& children : cluster_members(*v)) {
    if (auto const first = first_outer_edge_of(dir_in_tag_t{}, children,
                                               g.get_flags())) {
      return make_range(cluster_in_edge_iterator(first, g.get_flags()), {});
    }
  }
  return {};
}

std::size_t in_degree(Service* v, ClusterDependencyGraph const& g) {
  IDLE_ASSERT(v);
  std::size_t count = 0;

  for (Service& children : cluster_members(*v)) {
    for (Node const& current : inner_nodes(children)) {
      count += node_degree(dir_in_tag_t{}, current, g.get_flags());
    }
  }

  return count;
}

std::size_t degree(Service* v, ClusterDependencyGraph const& g) {
  return in_degree(v, g) + out_degree(v, g);
}
} // namespace idle
