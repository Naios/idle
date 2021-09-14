
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

#ifndef IDLE_EXTRA_COLOR_GRAPH_HPP_INCLUDED
#define IDLE_EXTRA_COLOR_GRAPH_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/service.hpp>
#include <idle/extra/color.hpp>

namespace idle {
/// Returns the color of a specific node inside the graph
IDLE_API(idle) Color nodeColor(Guid guid, Service::Phase phase) noexcept;

/// Returns the color of a specific parent/child hierarchy inside the graph
IDLE_API(idle) Color inheritanceColor(Guid guid) noexcept;

/// Returns the color of a specific part inside the graph
IDLE_API(idle) Color partColor(Guid guid, Part::Kind type) noexcept;

/// Returns the color of a specific edge inside the graph
IDLE_API(idle) Color edgeColor(EdgeProperties config) noexcept;
} // namespace idle

#endif // IDLE_EXTRA_COLOR_GRAPH_HPP_INCLUDED
