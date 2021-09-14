
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

#ifndef EX_03_01_ABSTRACT_ABSTRACT_HPP_INCLUDED
#define EX_03_01_ABSTRACT_ABSTRACT_HPP_INCLUDED

#include <limits>
#include <string>
#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>

namespace examples {
class IDLE_API(ex_03_01_abstract) AbstractInterface : public idle::Interface {
public:
  // It is advised to anchor the v-table of the Interface
  // inside a compilation unit through defining at least one
  // of its methods inside the compilation unit (.cpp file).
  explicit AbstractInterface(idle::Service& owner);

  // A simple message that can be specifically implemented
  virtual std::string message() const noexcept {
    // You can try out to change this default method.
    // It will cause all depending plugins to re-compile.
    return "Default Implementation";
  }

  // Returns a custom implemented priority for the Interface.
  virtual int priority() const noexcept {
    return std::numeric_limits<int>::max();
  }

  // Ranks the Interface using our priority method above.
  bool operator>(Interface const& other) const noexcept final;

  IDLE_INTERFACE
};

class IDLE_API(ex_03_01_abstract) AbstractDefaultInterface
  : public idle::Interface {

public:
  explicit AbstractDefaultInterface(idle::Service& owner);

  static idle::Ref<AbstractDefaultInterface> create(idle::Inheritance parent);

  IDLE_INTERFACE
};
} // namespace examples

#endif // EX_03_01_ABSTRACT_ABSTRACT_HPP_INCLUDED
