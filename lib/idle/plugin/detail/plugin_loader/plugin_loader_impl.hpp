
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

#ifndef IDLE_PLUGIN_DETAIL_PLUGIN_LOADER_PLUGIN_LOADER_IMPL_HPP_INCLUDED
#define IDLE_PLUGIN_DETAIL_PLUGIN_LOADER_PLUGIN_LOADER_IMPL_HPP_INCLUDED

#include <string>
#include <unordered_map>
#include <vector>
#include <boost/filesystem/path.hpp>
#include <idle/core/api.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/parts/component.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/plugin/plugin.hpp>
#include <idle/plugin/plugin_loader.hpp>
#include <idle/service/file_watcher.hpp>

namespace idle {
class PluginLoaderImpl : public PluginLoader,
                         public Upcastable<PluginLoaderImpl> {

  using path_t = boost::filesystem::path;

public:
  using PluginLoader::PluginLoader;

  PluginLoaderImpl(PluginLoaderImpl&&) = delete;
  PluginLoaderImpl(PluginLoaderImpl const&) = delete;
  PluginLoaderImpl& operator=(PluginLoaderImpl&&) = delete;
  PluginLoaderImpl& operator=(PluginLoaderImpl const&) = delete;

  void setupImpl(Config config);
  void onSetup() override;

  continuable<> onStart() override;

  continuable<PluginChanges> watch_impl();

private:
  void updateSideloadBannList(FileWatcher::FileChanges const& changes);
  PluginChanges convert(FileWatcher::FileChanges changes);
  path_t unique_path_cache_of(path_t const& path);
  Ref<Plugin> load(std::string path);

  std::vector<FileWatcher::Entry> dirs_;
  bool initial_load_{true};
  path_t cache_directory_;

  Component<FileWatcher> file_watcher_{*this};
  Component<PluginSource> plugin_source_{*this};

  Plugin::Generation generation_{0U};
  std::unordered_map<std::string, WeakRef<Plugin>> plugins_;
};
} // namespace idle

#endif // IDLE_PLUGIN_DETAIL_PLUGIN_LOADER_PLUGIN_LOADER_IMPL_HPP_INCLUDED
