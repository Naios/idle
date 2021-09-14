
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

#ifndef IDLE_PLUGIN_DETAIL_PLUGIN_COMPILER_PLUGIN_COMPILER_IMPL_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_PLUGIN_COMPILER_PLUGIN_COMPILER_IMPL_HPP_INCLUDED

#include <string>
#include <boost/filesystem/path.hpp>
#include <idle/core/parts/component.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/activity.hpp>
#include <idle/interface/log_sink_facade.hpp>
#include <idle/interface/logger.hpp>
#include <idle/plugin/plugin_compiler.hpp>
#include <idle/service/process_group.hpp>
#include <idle/service/sink.hpp>

namespace idle {
class PluginCompilerImpl : public PluginCompiler,
                           public LogSinkFacade<PluginCompilerImpl>,
                           public Upcastable<PluginCompilerImpl> {

  using path_t = boost::filesystem::path;

  friend class ColorErrSink;
  friend class ColorOutSink;

public:
  using PluginCompiler::PluginCompiler;

  PluginCompilerImpl(PluginCompilerImpl&&) = delete;
  PluginCompilerImpl(PluginCompilerImpl const&) = delete;
  PluginCompilerImpl& operator=(PluginCompilerImpl&&) = delete;
  PluginCompilerImpl& operator=(PluginCompilerImpl const&) = delete;

  void setupImpl(Config config);

  continuable<> onStart() override;

  continuable<> generate_impl();

  continuable<> build_impl(std::string target);
  continuable<> build_install_impl();
  continuable<> install_component_impl(std::string component);

  void log(LogLevel level, LogMessage const& message) noexcept;
  bool is_enabled(LogLevel) const noexcept {
    return true;
  }

private:
  continuable<> cmake_configure();
  continuable<> cmake_regenerate();

#ifdef IDLE_PLATFORM_WINDOWS
  bool writeCompilerEnvCache(SpawnOptions::Environment const& env) const;
  optional<SpawnOptions::Environment> readCompilerEnvCache() const noexcept;
#endif

  StreamingOptions stream();

  Config config_;
  bool is_config_generator_{false};
  Component<ProcessGroup> process_group_{*this};
  Dependency<Log> log_{*this};
  Dependency<Activities> activities_{*this};

  SpawnOptions::Environment compiler_env_;
};
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_PLUGIN_COMPILER_PLUGIN_COMPILER_IMPL_HPP_INCLUDED
