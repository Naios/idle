
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

#ifndef IDLE_PLUGIN_PLUGIN_HOTSWAP_HPP_INCLUDED
#define IDLE_PLUGIN_PLUGIN_HOTSWAP_HPP_INCLUDED

#include <string>
#include <vector>
#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/service/art/reflection.hpp>

namespace idle {
class IDLE_API(idle) PluginHotSwap : public Service {
  friend class PluginHotswapImpl;

protected:
  using Service::Service;

public:
  PluginHotSwap(PluginHotSwap&&) = delete;
  PluginHotSwap(PluginHotSwap const&) = delete;
  PluginHotSwap& operator=(PluginHotSwap&&) = delete;
  PluginHotSwap& operator=(PluginHotSwap const&) = delete;

  struct Config {
    /// The source directory which is compiled on change
    ///
    /// \note defaults to `${executable_dir}/../project` if left empty
    std::string source_dir{"../project"};

    /// The directory and files that are watched for changes
    ///
    /// \note defaults to `${source_dir}` if left empty
    std::vector<std::string> watch{"../project/src"};

    /// The directories containing plugin projects or single source files
    /// that are directly compiled into plugins.
    std::vector<std::string> workspaces{"../project/src"};

    /// The CMake additional module directory
    ///
    /// \note defaults to `${executable_dir}/../share/cmake` if left empty
    std::string module_dir;

    /// The directory the source is built to
    ///
    /// \note defaults to `${source_dir}/build` if left empty
    std::string build_dir;

    /// Defines whether artifacts shall be cleaned on start.
    ///
    /// This is not needed if your build system keeps
    /// the artifact directory up-to-date.
    bool clean_artifacts{false};

    /// The cache directory used for plugin caching
    ///
    /// \note defaults to `${tmp}/${executable_name}` if left empty
    std::string cache_dir;

    /// The CMake generator target (can be left empty)
    std::string generator;
  };

  void setup(Config config);

  static Ref<PluginHotSwap> create(Inheritance inh);

  IDLE_SERVICE
};

IDLE_API(idle) Reflection const& reflect(PluginHotSwap::Config const*) noexcept;
} // namespace idle

#endif // IDLE_PLUGIN_PLUGIN_HOTSWAP_HPP_INCLUDED
