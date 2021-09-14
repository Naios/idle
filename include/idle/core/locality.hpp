
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

#ifndef IDLE_CORE_LOCALITY_HPP_INCLUDED
#define IDLE_CORE_LOCALITY_HPP_INCLUDED

#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>

namespace idle {
class Part;

/// Represents a dynamic locality which can dynamically loaded and unloaded
/// from the application.
class IDLE_API(idle) Locality {
public:
  Locality() = default;
  virtual ~Locality();

  static Ref<Locality> permanent() noexcept {
    return {};
  }

  /// Returns the exporter in which automatically created services
  /// are registered.
  virtual Part& anchor() noexcept = 0;
  /// \copydoc anchor
  virtual Part const& anchor() const noexcept = 0;
};

IDLE_API_HIDDEN Ref<Locality> this_locality() noexcept;
} // namespace idle

#endif // IDLE_CORE_LOCALITY_HPP_INCLUDED
