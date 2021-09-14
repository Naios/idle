
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

// clang-format off
#ifndef IDLE_CORE_EMBED_LOCALITY_PLUGIN_HPP_INCLUDED
#define IDLE_CORE_EMBED_LOCALITY_PLUGIN_HPP_INCLUDED
// clang-format on

// clang-format off
#include <idle/core/util/assert.hpp>
#include <idle/core/locality.hpp>
#include <idle/core/ref.hpp>
#include <idle/plugin/alias.hpp>
#include <idle/plugin/plugin.hpp>
// clang-format on

namespace idle {
namespace detail {
static WeakRef<PluginContext> this_plugin_context;

extern "C" {
IDLE_API_EXPORT void set_context(WeakRef<PluginContext> this_context) {
  IDLE_ASSERT(this_plugin_context.expired());
  IDLE_ASSERT(!this_context.expired());
  this_plugin_context = std::move(this_context);
}
}

IDLE_ALIAS(set_context);
} // namespace detail

Ref<Locality> this_locality() noexcept {
  IDLE_ASSERT(detail::this_plugin_context);
  IDLE_ASSERT(!detail::this_plugin_context.expired());

  auto context = detail::this_plugin_context.lock();
  IDLE_ASSERT(context);
  return std::move(context);
}
} // namespace idle

// clang-format off
#else // IDLE_LOCALITY_PLUGIN_HPP_INCLUDED
#  error 'idle/core/locality_plugin.hpp' should only be included once \
       per shared library or executable in one compilation unit!
#endif // IDLE_CORE_EMBED_LOCALITY_PLUGIN_HPP_INCLUDED
// clang-format on
