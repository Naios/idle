
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

#ifndef IDLE_SERVICE_DETAIL_PRESETS_APPLICATION_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_PRESETS_APPLICATION_IMPL_HPP_INCLUDED

#include <idle/core/context.hpp>
#include <idle/core/parts/component.hpp>
#include <idle/core/parts/introduce.hpp>
#include <idle/core/parts/link.hpp>
#include <idle/core/platform.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/logger.hpp>
#include <idle/interface/program_arguments.hpp>
#include <idle/service/autostarter.hpp>
#include <idle/service/command_line.hpp>
#include <idle/service/commands.hpp>
#include <idle/service/presets/application.hpp>
#include <idle/service/signal_set.hpp>

namespace idle {
class CtrlCHandler : public Service {
public:
  using Service::Service;

  void onSetup() override {
    signal_set_->setup(SignalSet::Config::signal_on_break());
  }

  continuable<> onStart() override {
    return async([this]() mutable {
      signal_set_->wait().then(
          [r = &root()] {
            return r->stop();
          },
          root().event_loop().through_post());
    });
  }

  Component<SignalSet> signal_set_{*this};

  IDLE_SERVICE
};

class ApplicationPresetImpl final : public ApplicationPreset,
                                    public Upcastable<ApplicationPresetImpl> {
public:
  ApplicationPresetImpl(Inheritance inh, int argc, char** argv)
    : ApplicationPreset(std::move(inh))
    , args_(*this, parent(), argc, argv) {}

  Link<DefaultService<ProgramArguments>> args_;

  Component<Autostarter> autostarter_{*this};

  Introduce<Commands> commands_{*this};

  Link<CommandLine> command_line_{*this};
};

class ConsolePresetImpl final : public ConsolePreset,
                                public Upcastable<ConsolePresetImpl> {
public:
  explicit ConsolePresetImpl(Inheritance parent)
    : ConsolePreset(std::move(parent)) {}

  Component<CtrlCHandler> signal_set_{*this};
  // Use<FileLogger> default_log_sink_{*this};
};
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_PRESETS_APPLICATION_IMPL_HPP_INCLUDED
