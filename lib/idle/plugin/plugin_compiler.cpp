
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
#include <idle/plugin/detail/plugin_compiler/plugin_compiler_impl.hpp>
#include <idle/plugin/plugin.hpp>
#include <idle/plugin/plugin_compiler.hpp>

namespace idle {
void PluginCompiler::setup(Config config) {
  return PluginCompilerImpl::from(this)->setupImpl(std::move(config));
}

continuable<> PluginCompiler::generate() {
  return PluginCompilerImpl::from(this)->generate_impl();
}

continuable<> PluginCompiler::build(std::string target) {
  return PluginCompilerImpl::from(this)->build_impl(std::move(target));
}

continuable<> PluginCompiler::buildInstall() {
  return PluginCompilerImpl::from(this)->build_install_impl();
}

continuable<> PluginCompiler::installComponent(std::string component) {
  return PluginCompilerImpl::from(this)->install_component_impl(
      std::move(component));
}

Ref<PluginCompiler> PluginCompiler::create(Inheritance parent) {
  return spawn<PluginCompilerImpl>(std::move(parent));
}
} // namespace idle
