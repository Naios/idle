
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

#include <idle/core/detail/unreachable.hpp>
#include <idle/extra/color_graph.hpp>

namespace idle {
// static constexpr color color_edge_active = "#3F3D3D";
static constexpr Color color_edge_active = "#AFD3D3D3";
static constexpr Color color_edge_inactive = "#C63741";
static constexpr Color color_edge_weak_active = "#878146";
static constexpr Color color_edge_weak_inactive = "#FF6D46";
static constexpr Color color_interface = "#3396D7";
static constexpr Color color_exporter = "#7196D7";
static constexpr Color color_importer = "#21A8A6";
static constexpr Color color_importer_satisfied = "#7782AFE0";
static constexpr Color color_importer_unsatisfied = "#AD758AE0";
static constexpr Color color_interface_hidden = "#5A9CA850";
static constexpr Color color_interface_visible = "#5A9CA8F0";
static constexpr Color color_service_outdated = "#CDDC39";
static constexpr Color color_service_root = "#795548";
static constexpr Color color_service_running = "#4DB6AC";
static constexpr Color color_service_pending = "#FF9800";
static constexpr Color color_service_stopped = "#9E9E9E";
static constexpr Color color_inh_cluster = "#FF102B";
static constexpr Color color_inh_component = "#FF802B";
static constexpr Color color_usage = "#69AD9EE0";

Color nodeColor(Guid guid, Service::Phase phase) noexcept {
  Color cc = color_service_root;

  if (guid.low() == 0) {
    return color_service_root;
  }

  switch (phase) {
    case Service::Phase::locked:
    case Service::Phase::stopping:
    case Service::Phase::starting:
    case Service::Phase::pending:
      return color_service_pending;
    case Service::Phase::running:
      return color_service_running;
    default:
      return color_service_stopped;
  }
}

Color inheritanceColor(Guid guid) noexcept {
  if (guid.isCluster()) {
    return color_inh_cluster;
  } else {
    return color_inh_component;
  }
}

Color partColor(Guid guid, Part::Kind type) noexcept {
  switch (type) {
    case Part::Kind::kImport:
      return color_importer;
    case Part::Kind::kExport:
      return color_exporter;
    case Part::Kind::kInterface: {
      if (guid.isComponent()) {
        return color_interface; // hidden
      } else {
        return color_interface;
      }
    }
  }
  IDLE_DETAIL_UNREACHABLE();
}

Color edgeColor(EdgeProperties config) noexcept {
  if (config.is_weak) {
    if (config.is_active) {
      return color_edge_weak_active;
    } else {
      return color_edge_weak_inactive;
    }
  } else {
    if (config.is_active) {
      return color_edge_active;
    } else {
      return color_edge_inactive;
    }
  }
}
} // namespace idle
