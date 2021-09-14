
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

#ifndef IDLE_PLUGIN_PLUGIN_RECOMPILER_HPP_INCLUDED
#define IDLE_PLUGIN_PLUGIN_RECOMPILER_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/plugin/plugin_compiler.hpp>

namespace idle {
class IDLE_API(idle) PluginRecompiler : public Service {
  friend class PluginRecompilerImpl;

protected:
  using Service::Service;

public:
  PluginRecompiler(PluginRecompiler&&) = delete;
  PluginRecompiler(PluginRecompiler const&) = delete;
  PluginRecompiler& operator=(PluginRecompiler&&) = delete;
  PluginRecompiler& operator=(PluginRecompiler const&) = delete;

  struct Config : public PluginCompiler::Config {
    std::vector<std::string> watch;
    std::vector<std::string> workspaces;
  };

  void setup(Config config);

  continuable<> watchAndCompile(bool force_regenerate = false);

  static Ref<PluginRecompiler> create(Inheritance inh);

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_PLUGIN_PLUGIN_RECOMPILER_HPP_INCLUDED
