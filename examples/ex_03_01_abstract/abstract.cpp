
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

#include "abstract.hpp"

using namespace idle;

namespace examples {
AbstractInterface::AbstractInterface(idle::Service& owner)
  : Interface(owner) {}

bool AbstractInterface::operator>(Interface const& other) const noexcept {
  return priority() < idle::cast<AbstractInterface>(other).priority();
}

AbstractDefaultInterface::AbstractDefaultInterface(idle::Service& owner)
  : Interface(owner) {}

idle::Ref<AbstractDefaultInterface>
AbstractDefaultInterface::create(idle::Inheritance parent) {
  return idle::spawn<DefaultService<AbstractDefaultInterface>>(
      std::move(parent));
}
} // namespace examples
