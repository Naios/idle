
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

#ifndef IDLE_PLUGIN_PLUGIN_LOADER_HPP_INCLUDED
#define IDLE_PLUGIN_PLUGIN_LOADER_HPP_INCLUDED

#include <iosfwd>
#include <string>
#include <vector>
#include <idle/core/api.hpp>
#include <idle/core/dep/variant.hpp>
#include <idle/core/ref.hpp>
#include <idle/plugin/plugin.hpp>
#include <idle/service/file_watcher.hpp>

namespace idle {
namespace plugin_event {
struct PluginAdded {
  Ref<Plugin> source;
};
struct PluginRemoved {
  Ref<Plugin> source;
};
struct PluginModified {
  Ref<Plugin> from;
  Ref<Plugin> to;
};
} // namespace plugin_event

class IDLE_API(idle) PluginLoader : public Service {
  friend class PluginLoaderImpl;

protected:
  using Service::Service;

public:
  PluginLoader(PluginLoader&&) = delete;
  PluginLoader(PluginLoader const&) = delete;
  PluginLoader& operator=(PluginLoader&&) = delete;
  PluginLoader& operator=(PluginLoader const&) = delete;

  struct Config {
    /// The directories to watch for plugins
    std::vector<FileWatcher::Entry> directories;

    /// Plugins contained in the directory are loaded initially
    bool initial_load{false};

    /// Specifies a temporary location for plugin caching for
    /// working around write protection after load.
    /// Can be left empty for no caching
    std::string cache_directory;
  };

  using PluginOperation = variant<plugin_event::PluginAdded,
                                  plugin_event::PluginRemoved,
                                  plugin_event::PluginModified>;

  using PluginChanges = std::vector<PluginOperation>;

  /// Configures the plugin service
  void setup(Config config);

  /// Starts to watch for file changes
  continuable<PluginChanges> watch();

  static Ref<PluginLoader> create(Inheritance inh);

  IDLE_SERVICE
};

IDLE_API(idle)
std::ostream& operator<<(std::ostream&, plugin_event::PluginAdded const&);
IDLE_API(idle)
std::ostream& operator<<(std::ostream&, plugin_event::PluginRemoved const&);
IDLE_API(idle)
std::ostream& operator<<(std::ostream&, plugin_event::PluginModified const&);

} // namespace idle

#endif // IDLE_PLUGIN_PLUGIN_LOADER_HPP_INCLUDED
