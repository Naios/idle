
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

#ifndef IDLE_PLUGIN_DETAIL_PLUGIN_HOTSWAP_PLUGIN_HOTSWAP_IMPL_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_PLUGIN_HOTSWAP_PLUGIN_HOTSWAP_IMPL_HPP_INCLUDED

#include <boost/filesystem/path.hpp>
#include <idle/core/detail/unordered_map.hpp>
#include <idle/core/parts/component.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/core/views/filter.hpp>
#include <idle/core/views/transform.hpp>
#include <idle/interface/activity.hpp>
#include <idle/interface/io_context.hpp>
#include <idle/interface/logger.hpp>
#include <idle/plugin/plugin.hpp>
#include <idle/plugin/plugin_hotswap.hpp>
#include <idle/plugin/plugin_loader.hpp>
#include <idle/plugin/plugin_recompiler.hpp>
#include <idle/service/process_group.hpp>

namespace idle {
class PluginHotswapImpl : public PluginHotSwap,
                          public Upcastable<PluginHotswapImpl> {

  using plugin_added = plugin_event::PluginAdded;
  using plugin_modified = plugin_event::PluginModified;
  using plugin_removed = plugin_event::PluginRemoved;

public:
  using PluginHotSwap::PluginHotSwap;

  PluginHotswapImpl(PluginHotswapImpl&&) = delete;
  PluginHotswapImpl(PluginHotswapImpl const&) = delete;
  PluginHotswapImpl& operator=(PluginHotswapImpl&&) = delete;
  PluginHotswapImpl& operator=(PluginHotswapImpl const&) = delete;

  void onSetup() override;

  continuable<> onStart() override;

  void setupImpl(Config config);

  continuable<> run_impl();

private:
  continuable<> dispatch_op_eager(plugin_added const& op);
  continuable<> dispatch_op_eager(plugin_removed const& op);
  continuable<> dispatch_op_eager(plugin_modified const& op);

  continuable<> watch_plugins_and_reload_loop();
  continuable<> watch_sources_and_rebuild_for_first_successful_loop();

  continuable<> watch_sources_and_rebuild_loop();

  Config config_;
  boost::filesystem::path artifact_dir_;
  Component<PluginRecompiler> recompiler_{*this};
  Component<PluginLoader> loader_{*this};
  Dependency<IOContext> io_context_{*this};
  Dependency<Log> log_{*this};
  Dependency<Activities> activities_{*this};

  detail::unordered_map<Plugin const*, Ref<Bundle>> bundles_;
};
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_PLUGIN_HOTSWAP_PLUGIN_HOTSWAP_IMPL_HPP_INCLUDED
