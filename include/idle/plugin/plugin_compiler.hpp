
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

#ifndef IDLE_PLUGIN_PLUGIN_COMPILER_HPP_INCLUDED
#define IDLE_PLUGIN_PLUGIN_COMPILER_HPP_INCLUDED

#include <chrono>
#include <string>
#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/plugin/plugin.hpp>
#include <idle/service/file_watcher.hpp>

namespace idle {
class IDLE_API(idle) PluginCompiler : public Service {
  friend class PluginCompilerImpl;

protected:
  using Service::Service;

public:
  PluginCompiler(PluginCompiler&&) = delete;
  PluginCompiler(PluginCompiler const&) = delete;
  PluginCompiler& operator=(PluginCompiler&&) = delete;
  PluginCompiler& operator=(PluginCompiler const&) = delete;

  struct Config {
    std::string cmake_exe;

    std::string source_dir;
    std::string build_dir;
    std::string install_dir;

    std::string build_type;

    /// CMake specific options
    /// The CMake extra module directory
    std::string module_dir;

    /// The CMake generator to  use such as
    /// "Visual Studio 15 2017 Win64"
    std::string generator;

    /// Cache compiler environment variables on some platforms to speedup the
    /// recompiler startup. Defaults to true, disable it if you change your env
    /// variables regularly.
    ///
    /// The cache is located at `${BUILD_DIR}/IdleCompilerEnvCache_${ARCH}.json`
    bool cache_build_env{true};

    /// The time its takes the compile environment variables cache to expire
    std::chrono::seconds cache_expiry{std::chrono::hours(1)};

    // Attempt to colorize the output on best effort
    bool colorize{true};
  };

  void setup(Config config);

  continuable<> generate();
  continuable<> build(std::string target = "ALL_BUILD");
  continuable<> buildInstall();
  continuable<> installComponent(std::string component);

  static Ref<PluginCompiler> create(Inheritance parent);

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_PLUGIN_PLUGIN_COMPILER_HPP_INCLUDED
