
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

#ifndef IDLE_PLUGIN_DETAIL_PLUGIN_RECOMPILER_PLUGIN_RECOMPILER_IMPL_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_PLUGIN_RECOMPILER_PLUGIN_RECOMPILER_IMPL_HPP_INCLUDED

#include <idle/core/parts/component.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/plugin/plugin_compiler.hpp>
#include <idle/plugin/plugin_recompiler.hpp>
#include <idle/service/file_watcher.hpp>
#include <idle/service/process_group.hpp>

namespace idle {
class PluginRecompilerImpl : public PluginRecompiler,
                             public Upcastable<PluginRecompilerImpl> {

public:
  using PluginRecompiler::PluginRecompiler;

  PluginRecompilerImpl(PluginRecompilerImpl&&) = delete;
  PluginRecompilerImpl(PluginRecompilerImpl const&) = delete;
  PluginRecompilerImpl& operator=(PluginRecompilerImpl&&) = delete;
  PluginRecompilerImpl& operator=(PluginRecompilerImpl const&) = delete;

  void setupImpl(Config config);

  continuable<> watch_and_compile_impl(bool force_regenerate);

  continuable<> compile_impl(bool reconfigure);

protected:
  void onSetup() override;

private:
  Config config_;

  Component<PluginCompiler> plugin_compiler_{*this};
  Component<FileWatcher> file_watcher_{*this};

  bool failed_last_{false};
};
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_PLUGIN_RECOMPILER_PLUGIN_RECOMPILER_IMPL_HPP_INCLUDED
