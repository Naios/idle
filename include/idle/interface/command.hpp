
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

#ifndef IDLE_INTERFACE_COMMAND_HPP_INCLUDED
#define IDLE_INTERFACE_COMMAND_HPP_INCLUDED

#include <string>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/string_view.hpp>

namespace idle {
class Arguments;
class Sink;

class IDLE_API(idle) Session {
public:
  virtual ~Session();

  virtual Sink& sink() noexcept = 0;
};

class IDLE_API(idle) Command : public Interface {
public:
  explicit Command(Service& owner);

  virtual continuable<> invoke(Ref<Session> session, Arguments&& args) = 0;

  /// Returns the current instruction name used during the lifetime
  /// of this command.
  StringView current_command_name() const noexcept;

  bool operator>(Interface const& other) const noexcept final;

protected:
  /// Provides the instruction name on initialization of this command
  ///
  /// This can be overwritten to provide a custom name,
  /// otherwise the name is generated from the service::name of the owner.
  virtual std::string command_name() const noexcept;

  void onPartInit() noexcept override;

private:
  std::string current_command_name_;

  IDLE_INTERFACE
};
} // namespace idle

#endif // IDLE_INTERFACE_COMMAND_HPP_INCLUDED
