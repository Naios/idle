
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

#include <idle/core/service.hpp>
#include <idle/plugin/detail/plugin_hotswap/plugin_hotswap_impl.hpp>
#include <idle/plugin/plugin_hotswap.hpp>
#include <idle/service/art/reflection_tree.hpp>

namespace idle {
void PluginHotSwap::setup(Config config) {
  return PluginHotswapImpl::from(this)->setupImpl(std::move(config));
}

Ref<PluginHotSwap> PluginHotSwap::create(Inheritance parent) {
  return spawn<PluginHotswapImpl>(std::move(parent));
}

IDLE_REFLECT(
    PluginHotSwap::Config, //
    (source_dir, R"(The source directory which is compiled on change.
                    Defaults to `${executable_dir}/../project` if left empty)"),
    (watch, R"(The directory and files that are watched for changes
               Defaults to `${source_dir}` if left empty)"),
    (workspaces,
     R"(The directories containing plugin projects or single source files
        that are directly compiled into plugins.)"),
    (module_dir, R"(The CMake additional module directory
                    Defaults to `${executable_dir}/../share/cmake` if left empty)"),
    (build_dir, R"(The directory the source is built to
                   Defaults to `${source_dir}/build` if left empty)"),
    (cache_dir, R"(The cache directory used for plugin caching
                   Defaults to `${tmp}/${executable_name}` if left empty)"),
    (clean_artifacts, R"(Defines whether artifacts shall be cleaned on start)"),
    (generator, R"(The CMake generator target (can be left empty))"))
} // namespace idle
