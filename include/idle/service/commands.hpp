
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

#ifndef IDLE_SERVICE_COMMANDS_HPP_INCLUDED
#define IDLE_SERVICE_COMMANDS_HPP_INCLUDED

#include <string>
#include <idle/core/api.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/parts/introduce.hpp>
#include <idle/core/service.hpp>
#include <idle/interface/command.hpp>

namespace idle {
class IDLE_API(idle) HelpCommand final : public Implements<Command> {
public:
  using Implements<Command>::Implements;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  std::string command_name() const noexcept override {
    return "help";
  }

  IDLE_SERVICE
};

class IDLE_API(idle) ExitCommand final : public Implements<Command> {
public:
  using Implements<Command>::Implements;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  std::string command_name() const noexcept override {
    return "exit";
  }

  IDLE_SERVICE
};

class IDLE_API(idle) VersionCommand final : public Implements<Command> {
public:
  using Implements<Command>::Implements;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  std::string command_name() const noexcept override {
    return "idle version";
  }

  IDLE_SERVICE
};

class IDLE_API(idle) StartCommand final : public Implements<Command> {
public:
  using Implements<Command>::Implements;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  std::string command_name() const noexcept override {
    return "idle start";
  }

  IDLE_SERVICE
};

class IDLE_API(idle) StopCommand final : public Implements<Command> {
public:
  using Implements<Command>::Implements;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  std::string command_name() const noexcept override {
    return "idle stop";
  }

  IDLE_SERVICE
};

class IDLE_API(idle) RestartCommand final : public Implements<Command> {
public:
  using Implements<Command>::Implements;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  std::string command_name() const noexcept override {
    return "idle restart";
  }

  IDLE_SERVICE
};

class IDLE_API(idle) LSCommand : public Implements<Command> {
public:
  using Implements<Command>::Implements;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  std::string command_name() const noexcept override {
    return "idle ls";
  }

  IDLE_SERVICE
};

class IDLE_API(idle) TreeCommand : public Implements<Command> {
public:
  using Implements<Command>::Implements;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  std::string command_name() const noexcept override {
    return "idle tree";
  }

  IDLE_SERVICE
};

class IDLE_API(idle) VerifyCommand : public Implements<Command> {
  using Implements<Command>::Implements;

public:
  std::string command_name() const noexcept override {
    return "idle verify";
  }

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  IDLE_SERVICE
};

class IDLE_API(idle) GraphShowCommand : public Implements<Command> {
  friend class graph_show_command_impl;
  using Implements<Command>::Implements;

public:
  std::string command_name() const noexcept override {
    return "idle graph show";
  }

  static Ref<GraphShowCommand> create(Inheritance parent);

  IDLE_SERVICE
};

namespace detail {
using CommandComposition = Implements<
    Introduce<ExitCommand>, Introduce<GraphShowCommand>, Introduce<HelpCommand>,
    Introduce<LSCommand>, Introduce<RestartCommand>, Introduce<StartCommand>,
    Introduce<StopCommand>, Introduce<TreeCommand>, Introduce<VerifyCommand>,
    Introduce<VersionCommand>>;
}

class IDLE_API(idle) Commands : public detail::CommandComposition {
public:
  explicit Commands(Inheritance inh)
    : detail::CommandComposition(std::move(inh)) {}

  IDLE_SERVICE
};
} // namespace idle

#endif // IDLE_SERVICE_COMMANDS_HPP_INCLUDED
