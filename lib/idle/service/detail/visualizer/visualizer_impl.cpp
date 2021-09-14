
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

#include <chrono>
#include <fstream>
#include <utility>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <idle/core/async.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/unreachable.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/use.hpp>
#include <idle/extra/graphviz.hpp>
#include <idle/service/detail/visualizer/visualizer_impl.hpp>

#ifdef _MSC_VER
#  pragma warning(disable : 4127)
#  pragma warning(disable : 4996)
#endif // _MSC_VER

namespace idle {
StringView graph_type_name(Visualizer::Options::GraphType type) {
  switch (type) {
    case Visualizer::Options::GraphType::dependency_graph: {
      return "dep";
    }
    case Visualizer::Options::GraphType::service_graph: {
      return "service";
    }
    case Visualizer::Options::GraphType::cluster_graph: {
      return "cluster";
    }
    default: {
      IDLE_DETAIL_UNREACHABLE();
    }
  }
}

static std::string
generate_graph_filename(Visualizer::Options::GraphType type) {
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  return format("graph_{:%Y_%m_%d_%H_%M_%S}_{}.gv", fmt::localtime(t),
                graph_type_name(type));
}

continuable<> VisualizerImpl::show_graph_impl(Options opt) {
  return asyncWrap(*this, [opt = std::move(opt)](auto&& me) mutable {
    auto const type = opt.type;
    return me->show_graph_impl(generate_graph_filename(type), std::move(opt));
  });
}

continuable<> VisualizerImpl::show_graph_impl(std::string file_path,
                                              Options opt) {
  return root().event_loop().async_dispatch(wrap(
      *this,
      [file_path = std::move(file_path),
       opt = std::move(opt)](auto&& me) mutable -> continuable<> {
        IDLE_ASSERT(me->state().isRunning());

        {
          std::ofstream file(file_path, std::ios::trunc);

          switch (opt.type) {
            case Options::GraphType::dependency_graph: {
              DependencyGraph graph(me->root(), graph_view, opt.flags);
              if (opt.reverse) {
                graphvizReverse(file, graph);
              } else {
                graphviz(file, graph);
              }
              break;
            }
            case Options::GraphType::service_graph: {
              ServiceDependencyGraph graph(me->root(), graph_view, opt.flags);
              if (opt.reverse) {
                graphvizReverse(file, graph);
              } else {
                graphviz(file, graph);
              }
              break;
            }
            case Options::GraphType::cluster_graph: {
              ClusterDependencyGraph graph(me->root(), graph_view, opt.flags);
              if (opt.reverse) {
                graphvizReverse(file, graph);
              } else {
                graphviz(file, graph);
              }
              break;
            }
            default: {
              IDLE_DETAIL_UNREACHABLE();
            }
          }
        }

        std::string dot = "dot"_path;
        if (dot.empty()) {
          return me->process_group_->open(std::move(file_path)).template as<>();
        } else {
          boost::filesystem::path name = file_path;
          name.replace_extension(".pdf");
          std::string const name_out = name.generic_string();
          return me->process_group_
              ->spawnProcess(dot,
                             {"-Tpdf", std::move(file_path), "-o", name_out})
              .then(me->process_group_->open(name_out))
              .template as<>();
        }
      }));
}
} // namespace idle
