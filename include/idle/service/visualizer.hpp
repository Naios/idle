
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

#ifndef IDLE_SERVICE_VISUALIZER_HPP_INCLUDED
#define IDLE_SERVICE_VISUALIZER_HPP_INCLUDED

#include <string>
#include <idle/core/api.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/service.hpp>

namespace idle {
class IDLE_API(idle) Visualizer : public Service {
  friend class VisualizerImpl;

protected:
  using Service::Service;

public:
  struct Options {
    constexpr Options() noexcept {} // Clang bug workaround

    enum class GraphType {
      dependency_graph, //
      service_graph,    //
      cluster_graph
    };

    GraphType type{GraphType::dependency_graph};
    bool reverse = false;
    GraphFlags flags;
  };

  /// Saves the \see dependency_graph into a .gv file and opens it
  ///
  /// If the `dot` application from graphviz is found the graphviz
  /// file is converted into `.pdf` and opened with your default viewer.
  continuable<> showGraph(Options opt = {});

  /// \copydoc showGraph
  continuable<> showGraph(std::string file_path, Options opt = {});

  static Ref<Visualizer> create(Inheritance parent);

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_SERVICE_VISUALIZER_HPP_INCLUDED
