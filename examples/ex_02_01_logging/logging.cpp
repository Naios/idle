
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

#include <string>
#include <idle/idle.hpp>

using namespace idle;

namespace examples {
/// Implements an eager service that logs some messages to the global log.
class LoggingService final : public Implements<Autostarted> {
public:
  using Super::Super;

  continuable<> onStart() override {
    return async([this] {
      // The default log, that distributes the messages to an arbitrary count of
      // loggers can received messages of varying levels:
      IDLE_LOG_TRACE(log_, "This is a trace message");
      IDLE_LOG_DEBUG(log_, "This is a debug message");
      IDLE_LOG_WARN(log_, "This is an info message");
      IDLE_LOG_INFO(log_, "This is an info message");
      IDLE_LOG_ERROR(log_, "This is an error message");
      IDLE_LOG_FATAL(log_, "This is a fatal message");

      // Logs can be formatted with fmtlib:
      IDLE_LOG_INFO(log_, "Some string: '{}', another string centered '{:^10}'",
                    "hello", "world");

      // Formatting string are constant and compile-time checked by default.
      // If you want to change your formatting string on runtime,
      // use the IDLE_LOG_MUT_* macros:
      std::string str = "Runtime format string: '{}'";
      IDLE_LOG_MUT_INFO(log_, str, "hello");
    });
  }

private:
  Dependency<Log> log_{*this};

  IDLE_SERVICE
};

IDLE_DECLARE(LoggingService)

/// Implements custom a Logger which receives all log messages.
///
/// Such a logger can be used to send the logs to a specific sink, like a file.
class ExampleLogger : public Implements<Logger> {
public:
  using Super::Super;

  void log(LogLevel level, LogMessage const& message) noexcept override {
    (void)level;
    (void)message;
  }

  /// Specifies whether the logger is not weak, and has to be running
  /// before the Log is running. \see Logger::isImmediateLogger for details.
  bool isImmediateLogger() const noexcept override {
    return false;
  }
};

IDLE_DECLARE(ExampleLogger)
} // namespace examples
