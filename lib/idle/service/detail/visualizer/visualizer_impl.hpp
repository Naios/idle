
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

#ifndef IDLE_SERVICE_DETAIL_VISUALIZER_VISUALIZER_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_VISUALIZER_VISUALIZER_IMPL_HPP_INCLUDED

#include <idle/core/parts/component.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/service/process_group.hpp>
#include <idle/service/visualizer.hpp>

namespace idle {
class VisualizerImpl : public Visualizer, public Upcastable<VisualizerImpl> {

public:
  using Visualizer::Visualizer;

  continuable<> show_graph_impl(Options opt);

  continuable<> show_graph_impl(std::string file_path, Options opt);

private:
  Component<ProcessGroup> process_group_{*this};
};
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_VISUALIZER_VISUALIZER_IMPL_HPP_INCLUDED
