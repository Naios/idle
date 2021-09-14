
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

#ifndef IDLE_INTERFACE_COMMAND_PROCESSOR_HPP_INCLUDED
#define IDLE_INTERFACE_COMMAND_PROCESSOR_HPP_INCLUDED

#include <string>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
class Session;

class IDLE_API(idle) CommandProcessor : public Interface {
public:
  using Interface::Interface;

  virtual continuable<> invoke(Ref<Session> session, std::string args) = 0;

  virtual std::vector<std::string> complete(Session& session,
                                            StringView command) noexcept = 0;

  static Ref<CommandProcessor> create(Inheritance parent);

  IDLE_INTERFACE
};
} // namespace idle

#endif // IDLE_INTERFACE_COMMAND_PROCESSOR_HPP_INCLUDED
