
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

using namespace idle;

namespace examples {
/// A configurable component Service that gets passed its
/// configurable values from its parent.
class ConfiguredComponent final : public Implements<> {
public:
  using Super::Super;

  /// Is called from the parent ConfiguredService,
  /// which receives the message string from the global config.
  void setup(std::string message) {
    message_ = std::move(message);
  }

  continuable<> onStart() override {
    return async([this] {
      IDLE_LOG_INFO(log_, "The configured component greets you with '{}'!",
                    message_);
    });
  }

private:
  Dependency<Log> log_{*this};

  std::string message_;

  IDLE_SERVICE
};

/// Implements an eager service is configured through the global variables.
class ConfiguredService final : public Implements<Autostarted> {
public:
  using Super::Super;

  void onSetup() override {
    Config const& config = *var_;

    // Print the current values of the config through the logger
    IDLE_LOG_INFO(log_, "Setting up ConfiguredService with config values:\n{}",
                  ConstReflectionPtr(config));

    component_->setup(var_->message);
  }

  /// Some sample enum values used in the config
  enum class Options : std::uint8_t {
    NoOption,
    SecondOption,
    AnotherOption,
    ThirdAndLastOption
  };

  /// The config values used by this service
  struct Config {
    std::string message{"Hello!"};
    int value{0};
    Options option{Options::NoOption};

    // Only required to forward declare the reflection function because
    // we define an inner class here.
    // If Config is defined out-of-class, this is not needed.
    friend Reflection const& reflect(Config const*) noexcept;
  };

private:
  Var<Config> var_{*this, "examples.configuration"};
  Component<ConfiguredComponent> component_{*this};
  Dependency<Log> log_{*this};

  IDLE_SERVICE
};

IDLE_REFLECT(ConfiguredService::Config, //
             (message, "A simple welcome message"),
             (value, "A configurable value"),
             (option, "An enum option to configure"))

IDLE_DECLARE(ConfiguredService)
} // namespace examples
