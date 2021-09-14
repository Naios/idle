
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

#include <ostream>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/core/context.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/extra/graph_print.hpp>

namespace idle {
static void print_hierarchy(std::ostream& os, StringView str,
                            std::size_t depth) {
  print(os, FMT_STRING("{:>{}}\n"), fmt::string_view(str.data(), str.size()),
        str.size() + (depth - 1) * 3);
}

static std::size_t depth_of(Service& current) {
  return parent_services(current).size() - 1;
}

void print_flat(std::ostream& os, DependencyGraph const& graph) {
  auto const nodes = vertices(graph);
  node_iterator const end = nodes.second;

  for (node_iterator itr = nodes.first; itr != end; ++itr) {
    switch (itr->index()) {
      case Node::index_of<Service>(): {
        Service& current = itr->get<Service>();
        print_hierarchy(os, fmt::format(FMT_STRING("* {}"), current),
                        depth_of(current) * 2);
        break;
      }
      case Node::index_of<Import>(): {
        Import& current = itr->get<Import>();
        print_hierarchy(os, fmt::format(FMT_STRING("<- {}"), current),
                        depth_of(current.owner()) * 2 + 3);
        break;
      }
      case Node::index_of<Export>(): {
        Export& current = itr->get<Export>();
        print_hierarchy(os, fmt::format(FMT_STRING("-> {}"), current),
                        depth_of(current.owner()) * 2 + 3);
        break;
      }
      case Node::index_of<Usage>(): {
        // Skip usage nodes
        break;
      }
      default: {
        IDLE_DETAIL_UNREACHABLE();
      }
    }
  }
}

static void print_service(std::ostream& os, Service& current) {
  print(os, FMT_STRING("- {} ({}, {})\n"), current, current.stats().state(),
        current.stats().usage());
}

static void print_part(std::ostream& os, Part& current) {
  print(os, FMT_STRING("         * {} {}\n"),
        isa<Import>(current) ? "<-" : "->", current.partName());
}

void print_list(std::ostream& os, DependencyGraph const& graph) {
  for (Service& current : transitive_services(graph.get_context())) {
    IDLE_ASSERT(current.state().isInitializedUnsafe());
    print_service(os, current);

    for (Part& p : current.parts()) {
      print_part(os, p);
    }
  }
}

static void print_service_recursive_impl(std::ostream& os, Service& current) {
  print_service(os, current);

  for (Part& p : current.parts()) {
    print_part(os, p);

    for (Service& child : p.children()) {
      IDLE_ASSERT(child.state().isInitializedUnsafe());
      print_service_recursive_impl(os, child);
    }
  }
}

void print_list_recursive(std::ostream& os, DependencyGraph const& graph) {
  print_service_recursive_impl(os, graph.get_context());
}

void print_topological_sorted(std::ostream& os, DependencyGraph const& graph) {
  (void)os;
  (void)graph;

  /*auto rev = boost::make_reverse_graph(*this);

  detail::unordered_set<service*> visited;
  node_color_map cm;
  topological_sort(rev,
  boost::make_function_output_iterator([&](node const& n) {
  if (auto current = n.try_get<service>()) {
  if (visited.find(&(*current)) == visited.end()) {
  visited.insert(&(*current));
  print_service(os, *current);
  }
  }
  }),
  color_map(cm));*/
  (void)os;
}
} // namespace idle
