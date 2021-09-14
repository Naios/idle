
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

#include <idle/idle.hpp>
// Make sure to also move the abstract directory into the project source dir!
#include "../ex_03_01_abstract/abstract.hpp"

using namespace idle;

namespace examples {
class FirstImplementation final : public Implements<AbstractInterface> {
public:
  using Super::Super;

  std::string message() const noexcept override {
    return "Implemented in ExampleAbstractImplementation 1";
  }

  int priority() const noexcept override {
    return 1;
  }
};

IDLE_DECLARE(FirstImplementation)

class SecondImplementation final : public Implements<AbstractInterface> {
public:
  using Super::Super;

  std::string message() const noexcept override {
    return "Implemented in ExampleAbstractImplementation 2";
  }

  int priority() const noexcept override {
    return 2;
  }
};

IDLE_DECLARE(SecondImplementation)
} // namespace examples
