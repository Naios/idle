
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

#ifndef IDLE_EXTRA_GRAPH_PRINT_HPP_INCLUDED
#define IDLE_EXTRA_GRAPH_PRINT_HPP_INCLUDED

#include <iosfwd>
#include <idle/core/api.hpp>

namespace idle {
class DependencyGraph;

/// Prints the graph as a readable flat representation without dependencies
IDLE_API(idle) void print_flat(std::ostream& os, DependencyGraph const& graph);
/// Prints the services in the graph as simple list
IDLE_API(idle) void print_list(std::ostream& os, DependencyGraph const& graph);
/// Prints the services in the graph as simple list (recursively)
IDLE_API(idle)
void print_list_recursive(std::ostream& os, DependencyGraph const& graph);
/// Prints the topological order of services
IDLE_API(idle)
void print_topological_sorted(std::ostream& os, DependencyGraph const& graph);
} // namespace idle

#endif // IDLE_EXTRA_GRAPH_PRINT_HPP_INCLUDED
