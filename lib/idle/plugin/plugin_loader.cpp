
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

#include <ostream>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/formatter.hpp>
#include <idle/plugin/detail/plugin_loader/plugin_loader_impl.hpp>
#include <idle/plugin/plugin.hpp>
#include <idle/plugin/plugin_loader.hpp>

namespace idle {
continuable<PluginLoader::PluginChanges> PluginLoader::watch() {
  return PluginLoaderImpl::from(this)->watch_impl();
}

void PluginLoader::setup(Config config) {
  PluginLoaderImpl::from(this)->setupImpl(std::move(config));
}

Ref<PluginLoader> PluginLoader::create(Inheritance inh) {
  return spawn<PluginLoaderImpl>(std::move(inh));
}

std::ostream& operator<<(std::ostream& os,
                         plugin_event::PluginAdded const& event) {
  fmt::print(os, FMT_STRING("plugin_event::plugin_added(source: '{}')"),
             *event.source);
  return os;
}

std::ostream& operator<<(std::ostream& os,
                         plugin_event::PluginRemoved const& event) {
  fmt::print(os, FMT_STRING("plugin_event::plugin_removed(source: '{}')"),
             *event.source);
  return os;
}

std::ostream& operator<<(std::ostream& os,
                         plugin_event::PluginModified const& event) {
  fmt::print(os,
             FMT_STRING("plugin_event::plugin_modified(from: '{}', to: '{}')"),
             *event.from, *event.to);
  return os;
}
} // namespace idle
