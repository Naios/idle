
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

#ifndef IDLE_INTERFACE_LOG_FACADE_HPP_INCLUDED
#define IDLE_INTERFACE_LOG_FACADE_HPP_INCLUDED

#include <atomic>
#include <cstddef>
#include <idle/core/dep/format.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/source_location.hpp>
#include <idle/interface/log.hpp>

namespace idle {
/// Represents a non owning log action
/// with an optional source_location attached.
struct LogMessage {
  constexpr LogMessage(StringView message_,
                       SourceLocation const& location_ = {}) noexcept
    : message(message_)
    , location(location_) {}

  StringView message;
  SourceLocation location;
};

template <typename Parent>
class LogFacade {
public:
  LogFacade() noexcept
    : log_level_(LogLevel::trace) {}

  bool is_enabled(LogLevel level) const noexcept {
    return log_level_.load(std::memory_order_relaxed) <= level;
  }

  void set_log_level(LogLevel level) noexcept {
    log_level_.store(level, std::memory_order_release);
  }

  /*template <typename FormatStr, typename... Args>
  void trace(FormatStr&& format_str, Args&&... args) {
  if (is_enabled(log_level::trace)) {
  dispatch(log_level::trace, source_location{},
  std::forward<FormatStr>(format_str),
  std::forward<Args>(args)...);
  }
  }*/
  template <typename FormatStr, typename... Args>
  void trace(SourceLocation const& loc, FormatStr&& format_str,
             Args&&... args) {
    if (is_enabled(LogLevel::trace)) {
      dispatch(LogLevel::trace, loc, std::forward<FormatStr>(format_str),
               std::forward<Args>(args)...);
    }
  }

  /*template <typename FormatStr, typename... Args>
  void debug(FormatStr&& format_str, Args&&... args) {
  if (is_enabled(log_level::debug)) {
  dispatch(log_level::debug, source_location{},
  std::forward<FormatStr>(format_str),
  std::forward<Args>(args)...);
  }
  }*/
  template <typename FormatStr, typename... Args>
  void debug(SourceLocation const& loc, FormatStr&& format_str,
             Args&&... args) {
    if (is_enabled(LogLevel::debug)) {
      dispatch(LogLevel::debug, loc, std::forward<FormatStr>(format_str),
               std::forward<Args>(args)...);
    }
  }

  /*template <typename FormatStr, typename... Args>
  void info(FormatStr&& format_str, Args&&... args) {
  if (is_enabled(log_level::info)) {
  dispatch(log_level::info, source_location{},
  std::forward<FormatStr>(format_str),
  std::forward<Args>(args)...);
  }
  }*/
  template <typename FormatStr, typename... Args>
  void info(SourceLocation const& loc, FormatStr&& format_str, Args&&... args) {
    if (is_enabled(LogLevel::info)) {
      dispatch(LogLevel::info, loc, std::forward<FormatStr>(format_str),
               std::forward<Args>(args)...);
    }
  }

  /*template <typename FormatStr, typename... Args>
  void warn(FormatStr&& format_str, Args&&... args) {
  if (is_enabled(log_level::warn)) {
  dispatch(log_level::warn, source_location{},
  std::forward<FormatStr>(format_str),
  std::forward<Args>(args)...);
  }
  }*/
  template <typename FormatStr, typename... Args>
  void warn(SourceLocation const& loc, FormatStr&& format_str, Args&&... args) {
    if (is_enabled(LogLevel::warn)) {
      dispatch(LogLevel::warn, loc, std::forward<FormatStr>(format_str),
               std::forward<Args>(args)...);
    }
  }

  /*template <typename FormatStr, typename... Args>
  void error(FormatStr&& format_str, Args&&... args) {
  if (is_enabled(log_level::error)) {
  dispatch(log_level::error, source_location{},
  std::forward<FormatStr>(format_str),
  std::forward<Args>(args)...);
  }
  }*/
  template <typename FormatStr, typename... Args>
  void error(SourceLocation const& loc, FormatStr&& format_str,
             Args&&... args) {
    if (is_enabled(LogLevel::error)) {
      dispatch(LogLevel::error, loc, std::forward<FormatStr>(format_str),
               std::forward<Args>(args)...);
    }
  }

  /*template <typename FormatStr, typename... Args>
  void fatal(FormatStr&& format_str, Args&&... args) {
  if (is_enabled(log_level::fatal)) {
  dispatch(log_level::fatal, source_location{},
  std::forward<FormatStr>(format_str),
  std::forward<Args>(args)...);
  }
  }*/
  template <typename FormatStr, typename... Args>
  void fatal(SourceLocation const& loc, FormatStr&& format_str,
             Args&&... args) {
    if (is_enabled(LogLevel::fatal)) {
      dispatch(LogLevel::fatal, loc, std::forward<FormatStr>(format_str),
               std::forward<Args>(args)...);
    }
  }

private:
  template <typename FormatStr, typename... Args>
  void dispatch(LogLevel level, SourceLocation const& loc,
                FormatStr&& format_str, Args&&... args) {

    fmt::memory_buffer buf;
    fmt::format_to(buf, std::forward<FormatStr>(format_str),
                   std::forward<Args>(args)...);

    LogMessage const message(StringView(buf.begin(), buf.size()), loc);
    static_cast<Parent*>(this)->log(level, message);
  }

  std::atomic<LogLevel> log_level_;
};
} // namespace idle

#endif // IDLE_INTERFACE_LOG_FACADE_HPP_INCLUDED
