
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

#include <boost/filesystem.hpp>
#include <idle/idle.hpp>

using namespace std::chrono_literals;
using namespace idle;

class CliTool final : public Implements<Autostarted> {
public:
  using Super::Super;

  struct Config {
    PluginHotSwap::Config hotswap;
  };

  void onSetup() override {
    plugin_hotswap_->setup(config_->hotswap);
  }

  continuable<> onStart() override {
    return async([this] {
      IDLE_LOG_INFO(log_, R"(
 _____    _ _        .      .    .
|_   _|  | | |  .       .           .
  | |  __| | | ___         .    .        .
  | | / _` | |/ _ \ {:<14} .
 _| || (_| | |  __/ github.com/Naios/idle
|_____\__,_|_|\___| AGPL v3 (Early Access)

- You are using the early access release of Idle
- This version is unstable and the API might change!
- Bugs reports & feature requests: github.com/Naios/idle/issues
)",
                    "v0.1");
    });
  }

private:
  Dependency<Log> log_{*this};

  Component<PluginHotSwap> plugin_hotswap_{*this};

  Var<Config> config_{*this, "idle"};

  IDLE_SERVICE
};

IDLE_REFLECT(CliTool::Config, hotswap)

IDLE_DECLARE(CliTool)

int main(int argc, char** argv) {
  std::ios::sync_with_stdio(false);
  enableTerminalColorSupport();

  Persistent<Context> context;
  Persistent<ApplicationPreset> application(*context, argc, argv);
  Persistent<ConsolePreset> console(*context);
  Persistent<ReloadableProperties> properties(*context);

  auto const current = boost::filesystem::system_complete(
      (argc > 1) ? argv[1] : ReloadableProperties::defaultLocation());
  auto const etc_dir = current.parent_path();
  boost::filesystem::create_directories(etc_dir);
  current_path(etc_dir);

  // Change the current path to the directory where the config is located
  ReloadableProperties::Config config;
  config.path = current.generic_string();
  properties->setup(std::move(config));

  (properties->start() && application->start() && console->start()).then([&] {
    // Do something
  });

  int const exit_code = context->run();
  return exit_code;
}
