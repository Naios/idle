
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

#ifndef IDLE_CORE_GRAPH_HPP_INCLUDED
#define IDLE_CORE_GRAPH_HPP_INCLUDED

#include <cstddef>
#include <iosfwd>
#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/detail/pointer_union.hpp>
#include <idle/core/detail/unordered_map.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/fwd.hpp>
#include <idle/core/iterators.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/bitset.hpp>
#include <idle/core/util/iterator_facade.hpp>
#include <idle/core/util/printable.hpp>
#include <idle/core/util/range.hpp>

namespace idle {
/// Specifies flags stored in \see graph_flags
enum class GraphFlag : std::uint8_t {
  // service -> *
  filter_child_importer_edge,
  filter_owner_exporter_edge,
  // exporter -> *
  filter_exporter_child_edge,
  filter_exporter_usage_edge,
  // importer -> *
  filter_importer_owner_edge,
  // usage -> *
  filter_usage_importer_edge,
  // * <- usage -> *
  filter_usage_strong_edge,
  filter_usage_weak_edge,
};

/// Can be used to filter a graph specifically for certain edges
class GraphFlags : public BitSet<GraphFlag> {
public:
  GraphFlags() = default;
  explicit constexpr GraphFlags(BitSet<GraphFlag> other) noexcept
    : BitSet<GraphFlag>(other) {}
  explicit constexpr GraphFlags(std::initializer_list<GraphFlag> list) noexcept
    : BitSet<GraphFlag>(list) {}

  GraphFlags(GraphFlags const&) = default;
  GraphFlags& operator=(GraphFlags const&) = default;

  constexpr bool has_child_importer_edge() const noexcept {
    return !contains(GraphFlag::filter_child_importer_edge);
  }
  constexpr bool has_owner_exporter_edge() const noexcept {
    return !contains(GraphFlag::filter_owner_exporter_edge);
  }
  constexpr bool has_exporter_child_edge() const noexcept {
    return !contains(GraphFlag::filter_exporter_child_edge);
  }
  constexpr bool has_exporter_usage_edge() const noexcept {
    return !contains(GraphFlag::filter_exporter_usage_edge) &&
           has_any_usage_edge();
  }
  constexpr bool has_importer_owner_edge() const noexcept {
    return !contains(GraphFlag::filter_importer_owner_edge);
  }
  constexpr bool has_usage_importer_edge() const noexcept {
    return !contains(GraphFlag::filter_usage_importer_edge) &&
           has_any_usage_edge();
  }
  constexpr bool has_any_usage_edge() const noexcept {
    return !contains_all({GraphFlag::filter_usage_weak_edge,
                          GraphFlag::filter_usage_strong_edge});
  }
  constexpr bool has_all_usage_edge() const noexcept {
    return !contains_any({GraphFlag::filter_usage_weak_edge,
                          GraphFlag::filter_usage_strong_edge});
  }
  constexpr bool has_usage_weak_edge() const noexcept {
    return !contains(GraphFlag::filter_usage_weak_edge);
  }
  constexpr bool has_usage_strong_edge() const noexcept {
    return !contains(GraphFlag::filter_usage_strong_edge);
  }

  /// Flags to build a graph that has no inner connected service edges
  /// like edges from and to the importer and exporter of a service.
  static constexpr GraphFlags service_graph() noexcept {
    return GraphFlags{GraphFlag::filter_owner_exporter_edge,
                      GraphFlag::filter_importer_owner_edge};
  }
  /// Flags to build a graph that has no edges to services
  /// outside of its belonging cluster.
  static constexpr GraphFlags cluster_graph() noexcept {
    return GraphFlags{GraphFlag::filter_exporter_child_edge,
                      GraphFlag::filter_usage_importer_edge,
                      GraphFlag::filter_exporter_usage_edge};
  }
  /// Flags to build a graph that has only edges to services
  /// outside of its belonging cluster.
  static constexpr GraphFlags external_graph() noexcept {
    return GraphFlags{GraphFlag::filter_child_importer_edge};
  }
  /// Flags to build a graph that has no strong usage edges
  static constexpr GraphFlags weak_usage_graph() noexcept {
    return GraphFlags{GraphFlag::filter_usage_strong_edge};
  }
  /// Flags to build a graph that has no weak usage edges
  static constexpr GraphFlags strong_usage_graph() noexcept {
    return GraphFlags{GraphFlag::filter_usage_weak_edge};
  }

  constexpr GraphFlags operator|(GraphFlags right) const noexcept {
    return GraphFlags{BitSet::operator|(right)};
  }
  constexpr GraphFlags operator&(GraphFlags right) const noexcept {
    return GraphFlags{BitSet::operator&(right)};
  }
};

namespace detail {
using node_union_t = pointer_union<Service, Import, Export, Usage>;
}

class IDLE_API(idle) Node : public detail::node_union_t {
  friend class node_iterator;

public:
  // Sentinel
  Node() = default;
  explicit Node(Service& s)
    : detail::node_union_t(s) {}
  explicit Node(Import& e)
    : detail::node_union_t(e) {}
  explicit Node(Export& i)
    : detail::node_union_t(i) {}
  explicit Node(Usage& u)
    : detail::node_union_t(u) {}

  static Node from_part(Part& current) noexcept {
    return visit(current, [](auto& p) -> Node {
      return Node(p);
    });
  }

  bool is_service() const noexcept {
    return is<Service>();
  }
  bool is_importer() const noexcept {
    return is<Import>();
  }
  bool is_exporter() const noexcept {
    return is<Export>();
  }
  bool is_usage() const noexcept {
    return is<Usage>();
  }

  bool has_owner() const noexcept;

  Service& owner() const noexcept;

  Node& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }
  Node& operator=(Service& s) noexcept {
    set(s);
    return *this;
  }
  Node& operator=(Import& e) noexcept {
    set(e);
    return *this;
  }
  Node& operator=(Export& i) noexcept {
    set(i);
    return *this;
  }
  Node& operator=(Interface& i) noexcept {
    set(*static_cast<Export*>(std::addressof(i)));
    return *this;
  }
  Node& operator=(Usage& u) noexcept {
    set(u);
    return *this;
  }
  Node& operator=(Part& p) noexcept {
    visit(p, [&](auto& current) mutable {
      *this = current;
    });
    return *this;
  }

  Guid guid() const noexcept;

  auto name() const {
    return printable([this](std::ostream& os) {
      print_node_name(os);
    });
  }

  friend IDLE_API(idle) std::ostream& operator<<(std::ostream& os,
                                                 Node const& n);

private:
  void print_node_name(std::ostream& os) const;
};
} // namespace idle

namespace std {
template <>
struct hash<idle::Node> {
  size_t operator()(idle::Node const& n) const noexcept {
    return hash<void*>{}(n.raw());
  }
};
} // namespace std

namespace idle {
namespace detail {
template <typename Parent, typename T>
class graph_iterator
  : public iterator_facade<Parent, std::forward_iterator_tag, T, std::ptrdiff_t,
                           T const*, T const&> {

public:
  graph_iterator() = default;

  explicit graph_iterator(T current) noexcept
    : current_(std::move(current)) {}

  T const& dereference() const noexcept {
    return current_;
  }

  bool equal(graph_iterator const& other) const noexcept {
    return current_ == other.current_;
  }

protected:
  T current_;
};

template <typename Parent, typename T>
class graph_flagged_iterator : public graph_iterator<Parent, T> {
public:
  graph_flagged_iterator() = default;

  explicit graph_flagged_iterator(T current, GraphFlags flags) noexcept
    : graph_iterator<Parent, T>(std::move(current))
    , flags_(flags) {}

protected:
  GraphFlags flags_;
};
} // namespace detail

class IDLE_API(idle) node_iterator
  : public detail::graph_iterator<node_iterator, Node> {

public:
  using graph_iterator<node_iterator, Node>::graph_iterator;

  void increment() noexcept;
};

template <typename T>
decltype(auto) visit_node(detail::node_union_t const& current, T&& visitor) {
  switch (current.index()) {
    case Node::index_of<Service>(): {
      return std::forward<T>(visitor)(current.get<Service>());
    }
    case Node::index_of<Import>(): {
      return std::forward<T>(visitor)(current.get<Import>());
    }
    case Node::index_of<Export>(): {
      return std::forward<T>(visitor)(current.get<Export>());
    }
    case Node::index_of<Usage>(): {
      return std::forward<T>(visitor)(current.get<Usage>());
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

/// Represents the relation of a given edge
enum class EdgeRelation : std::uint8_t {
  implicit,   //< Edge between interfaces, extensions and services
  parent,     //< Explicitly parent relation
  dependency, //< Automatically selected dependency relation
};

IDLE_API(idle)
EdgeRelation edge_relation_between(Guid source, Guid target) noexcept;

/// Represents certain properties of an edge
struct EdgeProperties {
  EdgeRelation relation : 6;
  bool is_active : 1;
  bool is_weak : 1;
};

class IDLE_API(idle) Edge {
  friend class edge_iterator;
  friend class in_edge_iterator;
  friend class out_edge_iterator;

public:
  /// Sentinel
  Edge() = default;
  explicit Edge(Node source, Node target)
    : source_(std::move(source))
    , target_(std::move(target)) {
    IDLE_ASSERT(static_cast<bool>(source_) == static_cast<bool>(target_));
  }

  Edge(Edge const&) noexcept = default;

  Edge& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }
  Edge& operator=(Edge const& e) noexcept {
    source_ = e.source();
    target_ = e.target();
    return *this;
  }

  bool operator==(Edge const& e) const noexcept {
    return (source() == e.source()) && (target() == e.target());
  }
  bool operator!=(Edge const& e) const noexcept {
    return !(*this == e);
  }

  void reset() noexcept {
    source_.reset();
    target_.reset();
  }

  Node const& source() const noexcept {
    return source_;
  }

  Node const& target() const noexcept {
    return target_;
  }

  void set(Node source, Node target) noexcept {
    target_ = std::move(target);
    source_ = std::move(source);
    IDLE_ASSERT(bool(source_) == bool(target_));
  }

  void set_source(Node source) noexcept {
    source_ = std::move(source);
    IDLE_ASSERT(bool(source_) == bool(target_));
  }

  void set_target(Node target) noexcept {
    target_ = std::move(target);
    IDLE_ASSERT(bool(source_) == bool(target_));
  }

  EdgeRelation relation() const noexcept;
  bool is_active() const noexcept;
  bool isWeak() const noexcept;

  EdgeProperties properties() const noexcept;

  explicit operator bool() const noexcept {
    IDLE_ASSERT(bool(source_) == bool(target_));
    return bool(source_);
  }

  IDLE_API(idle)
  friend std::ostream& operator<<(std::ostream& os, Edge const& current);

private:
  Node source_;
  Node target_;
};

class IDLE_API(idle) edge_iterator
  : public detail::graph_flagged_iterator<edge_iterator, Edge> {

public:
  using graph_flagged_iterator<edge_iterator, Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

class IDLE_API(idle) out_edge_iterator
  : public detail::graph_flagged_iterator<out_edge_iterator, Edge> {

public:
  using graph_flagged_iterator<out_edge_iterator, Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

class IDLE_API(idle) in_edge_iterator
  : public detail::graph_flagged_iterator<in_edge_iterator, Edge> {

public:
  using graph_flagged_iterator<in_edge_iterator, Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

/// A tag which makes the idle graphs not gather information of its
/// edge and vertex count on construction.
struct graph_view_t {};
constexpr graph_view_t graph_view{};

/// A boost::graph compatible class which models the service dependency graph.
///
/// This class models the following concepts from boost::graph:
/// - VertexListGraphConcept
/// - EdgeListGraphConcept
/// - IncidenceGraphConcept
/// - BidirectionalGraphConcept
///
/// \attention In order to use this class properly with boost::graph you
///            have to `#include <idle/core/external/boost/graph.hpp>` too!
///
class IDLE_API(idle) DependencyGraph {
public:
  using vertex_descriptor = Node;
  using edge_descriptor = Edge;

  /// Creates a dependency_graph which models the given context.
  ///
  /// \attention In order to be boost::graph compatible the construction
  ///            of this graph involves counting vertices and edges!
  ///            Use graph_view_t to construct an inexpensive view of the graph!
  explicit DependencyGraph(Context& root, GraphFlags flags = {}) noexcept;
  /// Creates a dependency_graph which models the given context without
  /// analyzing the number of nodes and edges is counted at construction.
  explicit DependencyGraph(Context& root, graph_view_t,
                           GraphFlags flags = {}) noexcept;

  std::size_t number_of_nodes() const noexcept {
    return num_vertices_;
  }
  std::size_t number_of_edges() const noexcept {
    return num_edges_;
  }

  Context& get_context() const noexcept {
    return context_;
  }

  GraphFlags get_flags() const noexcept {
    return flags_;
  }

private:
  Context& context_;
  std::size_t num_vertices_{0};
  std::size_t num_edges_{0};
  GraphFlags flags_;
};

// - VertexListGraph
inline std::size_t num_vertices(DependencyGraph const& g) {
  return g.number_of_nodes();
}

IDLE_API(idle) Range<node_iterator> vertices(DependencyGraph const& g);

// - EdgeListGraph
inline Node source(Edge const& e, DependencyGraph const&) {
  return e.source();
}
inline Node target(Edge const& e, DependencyGraph const&) {
  return e.target();
}

IDLE_API(idle) Range<edge_iterator> edges(DependencyGraph const& g);

inline std::size_t num_edges(DependencyGraph const& g) {
  return g.number_of_edges();
}

// - IncidenceGraph
IDLE_API(idle)
Range<out_edge_iterator> out_edges(Node const& v, DependencyGraph const& g);

IDLE_API(idle) std::size_t out_degree(Node const& v, DependencyGraph const& g);

// - BidirectionalGraph
IDLE_API(idle)
Range<in_edge_iterator> in_edges(Node const& v, DependencyGraph const& g);

IDLE_API(idle) std::size_t in_degree(Node const& v, DependencyGraph const& g);

IDLE_API(idle) std::size_t degree(Node const& v, DependencyGraph const& g);

// - VertexIndexGraph
// This is actually not needed because the vertex count stays constant
// but required by the concept for some reason, which is illogical for
// an immutable graph concept.
inline void renumber_vertex_indices(DependencyGraph&) {}

/// A dependency graph where every node has an incremental fixed integral index
/// that is required for some boost::graph algorithms.
///
/// This class burdens construction overhead through the indexing process.
/// Also looking up the integral id of a node does come at some costs!
///
/// By default node::id_t is hashable and totally ordered but not fixed
/// to a certain integral type which makes it impossible to be used in
/// an array or matrix.
class IDLE_API(idle) IndexedDependencyGraph : public DependencyGraph {
public:
  explicit IndexedDependencyGraph(Context& root);

  /// Returns the id to the corresponding node
  ///
  /// \attention It's required that the node is part of this graph!
  std::size_t lookup_id(Node const& n) const noexcept;

private:
  detail::unordered_map<Node, std::size_t> mapping_;
};

// - VertexIndexGraph
inline void renumber_vertex_indices(IndexedDependencyGraph&) {}

class IDLE_API(idle) service_node_iterator
  : public iterator_facade<service_node_iterator, std::forward_iterator_tag,
                           Service*, std::ptrdiff_t, Service*, Service*> {

public:
  service_node_iterator();

  explicit service_node_iterator(Service& current) noexcept
    : current_(transitive_service_iterator(current)) {}

  Service* dereference() const noexcept {
    return std::addressof(*current_);
  }

  void increment() noexcept {
    ++current_;
  }

  bool equal(service_node_iterator const& other) const noexcept {
    return current_ == other.current_;
  }

protected:
  transitive_service_iterator current_;
};

class IDLE_API(idle) service_edge_iterator
  : public detail::graph_flagged_iterator<service_edge_iterator, Edge> {

public:
  using graph_flagged_iterator<service_edge_iterator,
                               Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

class IDLE_API(idle) service_out_edge_iterator
  : public detail::graph_flagged_iterator<service_out_edge_iterator, Edge> {

public:
  using graph_flagged_iterator<service_out_edge_iterator,
                               Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

class IDLE_API(idle) service_in_edge_iterator
  : public detail::graph_flagged_iterator<service_in_edge_iterator, Edge> {

public:
  using graph_flagged_iterator<service_in_edge_iterator,
                               Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

/// A boost::graph compatible class which models the minimal dependencies
/// between services
class IDLE_API(idle) ServiceDependencyGraph {
public:
  using vertex_descriptor = Service*;
  using edge_descriptor = Edge;

  /// Creates a dependency_graph which models the given context.
  explicit ServiceDependencyGraph(Context& root,
                                  GraphFlags flags = {}) noexcept;
  explicit ServiceDependencyGraph(Context& root, graph_view_t,
                                  GraphFlags flags = {}) noexcept;

  std::size_t number_of_nodes() const noexcept {
    return num_vertices_;
  }
  std::size_t number_of_edges() const noexcept {
    return num_edges_;
  }

  Context& get_context() const noexcept {
    return context_;
  }

  GraphFlags get_flags() const noexcept {
    return flags_;
  }

private:
  Context& context_;
  std::size_t num_vertices_{0};
  std::size_t num_edges_{0};
  GraphFlags flags_;
};

// - VertexListGraph
inline std::size_t num_vertices(ServiceDependencyGraph const& g) {
  return g.number_of_nodes();
}

IDLE_API(idle)
Range<service_node_iterator> vertices(ServiceDependencyGraph const& g);

// - EdgeListGraph
IDLE_API(idle) Service* source(Edge const& e, ServiceDependencyGraph const&);

IDLE_API(idle) Service* target(Edge const& e, ServiceDependencyGraph const&);

IDLE_API(idle)
Range<service_edge_iterator> edges(ServiceDependencyGraph const& g);

inline std::size_t num_edges(ServiceDependencyGraph const& g) {
  return g.number_of_edges();
}

// - IncidenceGraph
IDLE_API(idle)
Range<service_out_edge_iterator> out_edges(Service* v,
                                           ServiceDependencyGraph const& g);

IDLE_API(idle)
std::size_t out_degree(Service* v, ServiceDependencyGraph const& g);

// - BidirectionalGraph
IDLE_API(idle)
Range<service_in_edge_iterator> in_edges(Service*,
                                         ServiceDependencyGraph const& g);

IDLE_API(idle)
std::size_t in_degree(Service* v, ServiceDependencyGraph const& g);

IDLE_API(idle)
std::size_t degree(Service* v, ServiceDependencyGraph const& g);

// - VertexIndexGraph
inline void renumber_vertex_indices(ServiceDependencyGraph&) {}

class IDLE_API(idle) cluster_node_iterator
  : public iterator_facade<cluster_node_iterator, std::forward_iterator_tag,
                           Service*, std::ptrdiff_t, Service*, Service*> {

public:
  cluster_node_iterator();

  explicit cluster_node_iterator(Service& current) noexcept
    : current_(cluster_iterator(current)) {}

  Service* dereference() const noexcept {
    return std::addressof(*current_);
  }

  void increment() noexcept {
    ++current_;
  }

  bool equal(cluster_node_iterator const& other) const noexcept {
    return current_ == other.current_;
  }

protected:
  cluster_iterator current_;
};

class IDLE_API(idle) cluster_edge_iterator
  : public detail::graph_flagged_iterator<cluster_edge_iterator, Edge> {

public:
  using graph_flagged_iterator<cluster_edge_iterator,
                               Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

class IDLE_API(idle) cluster_out_edge_iterator
  : public detail::graph_flagged_iterator<cluster_out_edge_iterator, Edge> {

public:
  using graph_flagged_iterator<cluster_out_edge_iterator,
                               Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

class IDLE_API(idle) cluster_in_edge_iterator
  : public detail::graph_flagged_iterator<cluster_in_edge_iterator, Edge> {

public:
  using graph_flagged_iterator<cluster_in_edge_iterator,
                               Edge>::graph_flagged_iterator;

  void increment() noexcept;
};

/// A boost::graph compatible class which models the dependencies
/// between clusters
class IDLE_API(idle) ClusterDependencyGraph {
public:
  using vertex_descriptor = Service*;
  using edge_descriptor = Edge;

  /// Creates a dependency_graph which models the given context.
  explicit ClusterDependencyGraph(Context& root,
                                  GraphFlags flags = {}) noexcept;
  explicit ClusterDependencyGraph(Context& root, graph_view_t,
                                  GraphFlags flags = {}) noexcept;

  std::size_t number_of_nodes() const noexcept {
    return num_vertices_;
  }
  std::size_t number_of_edges() const noexcept {
    return num_edges_;
  }

  Context& get_context() const noexcept {
    return context_;
  }

  GraphFlags get_flags() const noexcept {
    return flags_;
  }

private:
  Context& context_;
  std::size_t num_vertices_{0};
  std::size_t num_edges_{0};
  GraphFlags flags_;
};

// - VertexListGraph
inline std::size_t num_vertices(ClusterDependencyGraph const& g) {
  return g.number_of_nodes();
}

IDLE_API(idle)
Range<cluster_node_iterator> vertices(ClusterDependencyGraph const& g);

// - EdgeListGraph
IDLE_API(idle) Service* source(Edge const& e, ClusterDependencyGraph const&);

IDLE_API(idle) Service* target(Edge const& e, ClusterDependencyGraph const&);

IDLE_API(idle)
Range<cluster_edge_iterator> edges(ClusterDependencyGraph const& g);

inline std::size_t num_edges(ClusterDependencyGraph const& g) {
  return g.number_of_edges();
}

// - IncidenceGraph
IDLE_API(idle)
Range<cluster_out_edge_iterator> out_edges(Service* v,
                                           ClusterDependencyGraph const& g);

IDLE_API(idle)
std::size_t out_degree(Service* v, ClusterDependencyGraph const& g);

// - BidirectionalGraph
IDLE_API(idle)
Range<cluster_in_edge_iterator> in_edges(Service*,
                                         ClusterDependencyGraph const& g);

IDLE_API(idle)
std::size_t in_degree(Service* v, ClusterDependencyGraph const& g);

IDLE_API(idle)
std::size_t degree(Service* v, ClusterDependencyGraph const& g);

// - VertexIndexGraph
inline void renumber_vertex_indices(ClusterDependencyGraph&) {}
} // namespace idle

#endif // IDLE_CORE_GRAPH_HPP_INCLUDED
