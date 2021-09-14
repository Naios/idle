
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
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/plugin/detail/plugin/plugin_impl.hpp>
#include <idle/plugin/plugin.hpp>

namespace idle {
Ref<Plugin> PluginSource::load(std::string path,
                               optional<std::string> cache_path,
                               Plugin::Generation generation) {

  PluginPaths paths;
  paths.path = std::move(path);
  paths.cache_path = std::move(cache_path);

  return PluginSourceImpl::from(this)->load_impl(std::move(paths), generation);
}

void PluginSource::banSideload(std::string module_name) {
  PluginSourceImpl::from(this)->banSideloadImpl(std::move(module_name));
}

void PluginSource::unbanSideload(std::string const& module_name) {
  PluginSourceImpl::from(this)->unbanSideloadImpl(module_name);
}

Plugin& Bundle::parent() noexcept {
  return *static_cast<Plugin*>(static_cast<Collection*>(&Service::parent()));
}

Plugin const& Bundle::parent() const noexcept {
  return *static_cast<Plugin const*>(
      static_cast<Collection const*>(&Service::parent()));
}

Plugin& PluginContext::operator*() const noexcept {
  return PluginContextImpl::from(this)->get_plugin();
}

Plugin* PluginContext::operator->() const noexcept {
  return &PluginContextImpl::from(this)->get_plugin();
}

Ref<Bundle> Plugin::createBundle() {
  return BundleImpl::create_from(*PluginImpl::from(this));
}

std::vector<std::string> const&
Plugin::exportedFunctions(bool undecorate) const {
  return PluginImpl::from(this)->exported_functions_impl(undecorate);
}

StringView Plugin::path() const noexcept {
  return PluginImpl::from(this)->path_impl();
}

StringView Plugin::cachePath() const noexcept {
  return PluginImpl::from(this)->cache_path_impl();
}

StringView Plugin::location() const noexcept {
  return PluginImpl::from(this)->location_impl();
}

PluginSource& Plugin::parent() noexcept {
  return static_cast<PluginSourceImpl&>(Service::parent().owner());
}

PluginSource const& Plugin::parent() const noexcept {
  return static_cast<PluginSourceImpl const&>(Service::parent().owner());
}

std::ostream& operator<<(std::ostream& os, Bundle const& obj) {
  return os << static_cast<Service const&>(obj);
}

std::ostream& operator<<(std::ostream& os, Plugin const& obj) {
  return os << static_cast<Service const&>(obj);
}

void* Plugin::symbolPtrOf(char const* name, bool undecorate) const {
  return PluginImpl::from(this)->symbol_ptr_of_impl(name, undecorate);
}

void* Plugin::aliasPtrOf(char const* name) const {
  return PluginImpl::from(this)->alias_ptr_of_impl(name);
}

Ref<PluginSource> PluginSource::create(Inheritance parent) {
  return spawn<PluginSourceImpl>(std::move(parent));
}
} // namespace idle
