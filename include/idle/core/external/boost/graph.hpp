
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

#ifndef IDLE_CORE_EXTERNAL_BOOST_GRAPH_HPP_INCLUDED
#define IDLE_CORE_EXTERNAL_BOOST_GRAPH_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
#include <utility>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <idle/core/detail/unordered_map.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/service.hpp>

namespace idle {
enum edge_properties_t { edge_property };

template <typename KeyType, typename ValueType,
          typename ReferenceType = ValueType const&>
struct node_map {
  using key_type = KeyType;
  using value_type = ValueType;
  using reference = ReferenceType;

  using map_t = detail::unordered_map<key_type, value_type>;

  node_map() = default;

  map_t& get() noexcept {
    return map_;
  }

  map_t const& get() const noexcept {
    return map_;
  }

private:
  map_t map_;
};

template <typename KeyType, typename ValueType, typename ReferenceType>
ValueType
get(node_map<KeyType, ValueType, ReferenceType> const& map,
    typename node_map<KeyType, ValueType, ReferenceType>::key_type const& key) {
  auto const itr = map.get().find(key);
  if (itr != map.get().end()) {
    return itr->second;
  } else {
    return typename node_map<ValueType, ReferenceType>::value_type{};
  }
}

template <typename KeyType, typename ValueType, typename ReferenceType>
void put(
    node_map<KeyType, ValueType, ReferenceType>& map,
    typename node_map<KeyType, ValueType, ReferenceType>::key_type const& key,
    typename node_map<KeyType, ValueType, ReferenceType>::value_type val) {
  auto const itr = map.get().find(key);
  if (itr != map.get().end()) {
    itr->second = std::move(val);
  } else {
    map.get().insert(std::make_pair(key, std::move(val)));
  }
}

/// Remove is not part of the boost::property_map concept but sometimes usable
template <typename KeyType, typename ValueType, typename ReferenceType>
void remove(
    node_map<KeyType, ValueType, ReferenceType>& map,
    typename node_map<KeyType, ValueType, ReferenceType>::key_type const& key) {
  map.get().erase(key);
}

/// Clear is not part of the boost::property_map concept but sometimes usable
template <typename KeyType, typename ValueType, typename ReferenceType>
void clear(node_map<KeyType, ValueType, ReferenceType>& map) {
  map.get().clear();
}

using node_color_map = node_map<Node, boost::default_color_type>;
using service_color_map = node_map<Service*, boost::default_color_type>;

struct indexed_service_id_map {
  using category = boost::readable_property_map_tag;

  using key_type = Node;
  using value_type = std::size_t;
  using reference = std::size_t;

  IndexedDependencyGraph const* graph_;
};

inline indexed_service_id_map::value_type
get(indexed_service_id_map const& map,
    indexed_service_id_map::key_type const& k) noexcept {
  return map.graph_->lookup_id(k);
}

struct node_guid_map {
  using category = boost::readable_property_map_tag;

  using key_type = Node;
  using value_type = Guid;
  using reference = Guid;
};
inline node_guid_map::reference get(node_guid_map const& /*map*/,
                                    Node const& n) noexcept {
  return n.guid();
}

struct guid_map {
  using category = boost::readable_property_map_tag;

  using key_type = Service*;
  using value_type = Guid;
  using reference = Guid;
};
inline guid_map::reference get(guid_map const& /*map*/, Service* k) noexcept {
  return k->guid();
}
} // namespace idle

namespace boost {
inline idle::indexed_service_id_map
get(vertex_index_t, idle::IndexedDependencyGraph const& graph) noexcept {
  return idle::indexed_service_id_map{std::addressof(graph)};
}
inline idle::indexed_service_id_map::value_type
get(vertex_index_t, idle::IndexedDependencyGraph const& g,
    idle::indexed_service_id_map::key_type const& k) noexcept {
  return g.lookup_id(k);
}

inline idle::node_guid_map get(vertex_index_t,
                               idle::DependencyGraph const&) noexcept {
  return {};
}
inline idle::node_guid_map::value_type
get(vertex_index_t, idle::DependencyGraph const&,
    idle::node_guid_map::key_type const& k) noexcept {
  return k.guid();
}

inline idle::guid_map get(vertex_index_t,
                          idle::ServiceDependencyGraph const&) noexcept {
  return {};
}
inline idle::guid_map::value_type
get(vertex_index_t, idle::ServiceDependencyGraph const&,
    idle::guid_map::key_type const& k) noexcept {
  return k->guid();
}

inline idle::guid_map get(vertex_index_t,
                          idle::ClusterDependencyGraph const&) noexcept {
  return {};
}
inline idle::guid_map::value_type
get(vertex_index_t, idle::ClusterDependencyGraph const&,
    idle::guid_map::key_type const& k) noexcept {
  return k->guid();
}
} // namespace boost

namespace idle {
struct edge_property_map {
  using category = boost::readable_property_map_tag;

  using key_type = Edge;
  using value_type = EdgeProperties;
  using reference = EdgeProperties;
};

inline edge_property_map::value_type
get(edge_property_map const&, edge_property_map::key_type const& k) noexcept {
  return k.properties();
}
inline edge_property_map::value_type
get(edge_properties_t, edge_property_map const&,
    edge_property_map::key_type const& k) noexcept {
  return k.properties();
}

inline edge_property_map get(edge_properties_t,
                             DependencyGraph const&) noexcept {
  return {};
}
inline edge_property_map get(edge_properties_t,
                             ServiceDependencyGraph const&) noexcept {
  return {};
}
inline edge_property_map get(edge_properties_t,
                             ClusterDependencyGraph const&) noexcept {
  return {};
}
} // namespace idle

namespace boost {
template <>
struct property_kind<idle::edge_properties_t> {
  using type = edge_property_tag;
};

template <>
struct graph_traits<idle::DependencyGraph> {
  // Graph
  using vertex_descriptor = idle::DependencyGraph::vertex_descriptor;
  using edge_descriptor = idle::DependencyGraph::edge_descriptor;
  using directed_category = directed_tag;
  using edge_parallel_category = disallow_parallel_edge_tag;

  struct traversal_category : edge_list_graph_tag,
                              vertex_list_graph_tag,
                              bidirectional_graph_tag {};

  static vertex_descriptor null_vertex() noexcept {
    return vertex_descriptor{};
  }

  // VertexListGraph
  using vertices_size_type = std::size_t;
  using vertex_iterator = idle::node_iterator;

  // EdgeListGraph
  using edges_size_type = std::size_t;
  using edge_iterator = idle::edge_iterator;

  // IncidenceGraph
  using degree_size_type = std::size_t;
  using out_edge_iterator = idle::out_edge_iterator;

  // BidirectionalGraph
  using in_edge_iterator = idle::in_edge_iterator;
};

template <>
struct graph_traits<idle::IndexedDependencyGraph>
  : graph_traits<idle::DependencyGraph> {};

template <>
struct graph_traits<idle::ServiceDependencyGraph> {
  // Graph
  using vertex_descriptor = idle::ServiceDependencyGraph::vertex_descriptor;
  using edge_descriptor = idle::ServiceDependencyGraph::edge_descriptor;
  using directed_category = directed_tag;
  using edge_parallel_category = allow_parallel_edge_tag;

  struct traversal_category : edge_list_graph_tag,
                              vertex_list_graph_tag,
                              bidirectional_graph_tag {};

  static vertex_descriptor null_vertex() noexcept {
    return nullptr;
  }

  // VertexListGraph
  using vertices_size_type = std::size_t;
  using vertex_iterator = idle::service_node_iterator;

  // EdgeListGraph
  using edges_size_type = std::size_t;
  using edge_iterator = idle::service_edge_iterator;

  // IncidenceGraph
  using degree_size_type = std::size_t;
  using out_edge_iterator = idle::service_out_edge_iterator;

  // BidirectionalGraph
  using in_edge_iterator = idle::service_in_edge_iterator;
};

template <>
struct graph_traits<idle::ClusterDependencyGraph> {
  // Graph
  using vertex_descriptor = idle::ClusterDependencyGraph::vertex_descriptor;
  using edge_descriptor = idle::ClusterDependencyGraph::edge_descriptor;
  using directed_category = directed_tag;
  using edge_parallel_category = allow_parallel_edge_tag;

  struct traversal_category : edge_list_graph_tag,
                              vertex_list_graph_tag,
                              bidirectional_graph_tag {};

  static vertex_descriptor null_vertex() noexcept {
    return nullptr;
  }

  // VertexListGraph
  using vertices_size_type = std::size_t;
  using vertex_iterator = idle::cluster_node_iterator;

  // EdgeListGraph
  using edges_size_type = std::size_t;
  using edge_iterator = idle::cluster_edge_iterator;

  // IncidenceGraph
  using degree_size_type = std::size_t;
  using out_edge_iterator = idle::cluster_out_edge_iterator;

  // BidirectionalGraph
  using in_edge_iterator = idle::cluster_in_edge_iterator;
};

template <>
struct property_map<idle::DependencyGraph, vertex_index_t> {
  using type = idle::node_guid_map;
  using const_type = type;
};

template <>
struct property_map<idle::IndexedDependencyGraph, vertex_index_t> {
  using type = idle::indexed_service_id_map;
  using const_type = type;
};

template <>
struct property_map<idle::ServiceDependencyGraph, vertex_index_t> {
  using type = idle::guid_map;
  using const_type = type;
};

template <>
struct property_map<idle::ClusterDependencyGraph, vertex_index_t> {
  using type = idle::guid_map;
  using const_type = type;
};

template <>
struct property_map<idle::DependencyGraph, idle::edge_properties_t> {
  using type = idle::edge_property_map;
  using const_type = type;
};

template <>
struct property_map<idle::IndexedDependencyGraph, idle::edge_properties_t>
  : property_map<idle::DependencyGraph, idle::edge_properties_t> {};

template <>
struct property_map<idle::ServiceDependencyGraph, idle::edge_properties_t>
  : property_map<idle::DependencyGraph, idle::edge_properties_t> {};

template <>
struct property_map<idle::ClusterDependencyGraph, idle::edge_properties_t>
  : property_map<idle::DependencyGraph, idle::edge_properties_t> {};

template <typename KeyType, typename ValueType, typename ReferenceType>
struct property_traits<idle::node_map<KeyType, ValueType, ReferenceType>> {
  using category = read_write_property_map_tag;

  using key_type = typename idle::node_map<KeyType, ValueType,
                                           ReferenceType>::key_type;
  using value_type = typename idle::node_map<KeyType, ValueType,
                                             ReferenceType>::value_type;
  using reference = typename idle::node_map<KeyType, ValueType,
                                            ReferenceType>::reference;
};

inline void
renumber_vertex_indices(reverse_graph<idle::IndexedDependencyGraph>&) {
  // See renumber_vertex_indices(idle::indexed_dependency_graph&)
}

inline void
renumber_vertex_indices(reverse_graph<idle::ServiceDependencyGraph>&) {
  // See renumber_vertex_indices(idle::indexed_dependency_graph&)
}

inline void
renumber_vertex_indices(reverse_graph<idle::ClusterDependencyGraph>&) {
  // See renumber_vertex_indices(idle::indexed_dependency_graph&)
}
} // namespace boost

#endif // IDLE_CORE_EXTERNAL_BOOST_GRAPH_HPP_INCLUDED
