
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

#ifndef IDLE_INTERFACE_LOGGER_HPP_INCLUDED
#define IDLE_INTERFACE_LOGGER_HPP_INCLUDED

#include <idle/core/api.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/source_location.hpp>
#include <idle/interface/log_facade.hpp>
#include <idle/service/sink.hpp>

namespace idle {
/// Implements a single threaded logger (log sink)
///
/// \attention The Logger::log method must only be called from one thread
///            simultaneously at a given time! If you want to log to a
///            single logger concurrently define your own Logger Interface
///            using a LoggerFacade!
///            Additionally a Logger shall no call another Log action during
///            logging to avoid deadlocks!
class IDLE_API(idle) Logger : public Interface, public LogFacade<Logger> {
public:
  using Interface::Interface;
  virtual ~Logger();

  /// Logs the given message
  ///
  /// \attention It is required that is_enabled(level) returned true previously.
  virtual void log(LogLevel level, LogMessage const& message) noexcept = 0;

  /// Returns true when this logger is immediate.
  ///
  /// Immediate means that it is guaranteed that all LogMessage's that are
  /// sinked when the Logger is initialized are passed to this running Logger.
  ///
  /// Otherwise it's possible that LogMessages are dropped because the Logger
  /// might not be running when the Log is running already!
  ///
  /// \attention This will make the Logger a strong dependency of the Log!
  virtual bool isImmediateLogger() const noexcept {
    return false;
  }

  IDLE_INTERFACE
};

/// A Log that sinks all log message to every Logger in a synchronized way
///
/// The benefit of this method is that only one mutex is acquired instead of
/// one for each logger per log call.
///
/// It is really doable not to use a mutex because the list of loggers might be
/// changed at runtime. TODO: This can be optimized for static builds!
///
/// \attention By default no Logger is present in the Context, even if you
///            want to log to the terminal you must provide your own dedicated
///            logger for that! A default terminal logger is provided through
///            the ConsoleLogger class.
class IDLE_API(idle) Log : public Interface, public LogFacade<Log> {
  friend class LogImpl;
  using Interface::Interface;

public:
  void log(LogLevel level, LogMessage const& message) noexcept;

  /// Returns a Sink that sinks into the given log level.
  Sink& sink(LogLevel level) noexcept;

  static Ref<Log> create(Inheritance parent);

  IDLE_INTERFACE
};

/// Implements a Logger which logs to a file through spdlog
/*class IDLE_API(idle) FileLogger : public Implements<Logger> {
public:
  using Implements<Logger>::Implements;

  struct Config {
    std::string file;
  };

  void setup(Config config);

  static Ref<FileLogger> create(Inheritance parent);
};

/// Implements a Logger which logs to the console through spdlog
class IDLE_API(idle) ConsoleLogger : public Implements<Logger> {
public:
  using Implements<Logger>::Implements;

  static Ref<ConsoleLogger> create(Inheritance parent);
};*/
} // namespace idle

#endif // IDLE_INTERFACE_LOGGER_HPP_INCLUDED
