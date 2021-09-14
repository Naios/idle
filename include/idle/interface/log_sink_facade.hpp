
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

#ifndef IDLE_INTERFACE_LOG_SINK_FACADE_HPP_INCLUDED
#define IDLE_INTERFACE_LOG_SINK_FACADE_HPP_INCLUDED

#include <idle/interface/log.hpp>
#include <idle/interface/log_facade.hpp>
#include <idle/service/sink.hpp>

namespace idle {
namespace detail {
template <typename Parent, LogLevel level>
class LogSink : public Sink {
public:
  void write(StringView data) override {
    Parent& parent = static_cast<Parent&>(*this);

    if (parent.is_enabled(level)) {
      parent.log(level, LogMessage{data});
    }
  }
};
template <typename Parent>
class LogSink<Parent, LogLevel::disabled> : public Sink {
public:
  void write(StringView data) override {
    (void)data;
  }
};
} // namespace detail

template <typename Parent>
class LogSinkFacade : public detail::LogSink<Parent, LogLevel::trace>,
                      public detail::LogSink<Parent, LogLevel::debug>,
                      public detail::LogSink<Parent, LogLevel::info>,
                      public detail::LogSink<Parent, LogLevel::warn>,
                      public detail::LogSink<Parent, LogLevel::error>,
                      public detail::LogSink<Parent, LogLevel::fatal>,
                      public detail::LogSink<Parent, LogLevel::disabled> {

public:
  Sink& sink(LogLevel level) noexcept {
    switch (level) {
      case LogLevel::trace:
        return *static_cast<detail::LogSink<Parent, LogLevel::trace>*>(this);
      case LogLevel::debug:
        return *static_cast<detail::LogSink<Parent, LogLevel::debug>*>(this);
      case LogLevel::info:
        return *static_cast<detail::LogSink<Parent, LogLevel::info>*>(this);
      case LogLevel::warn:
        return *static_cast<detail::LogSink<Parent, LogLevel::warn>*>(this);
      case LogLevel::error:
        return *static_cast<detail::LogSink<Parent, LogLevel::error>*>(this);
      case LogLevel::fatal:
        return *static_cast<detail::LogSink<Parent, LogLevel::fatal>*>(this);
      case LogLevel::disabled:
        return *static_cast<detail::LogSink<Parent, LogLevel::disabled>*>(this);
    }

    IDLE_DETAIL_UNREACHABLE();
  }
  Sink& sinkOptimized(LogLevel level) noexcept {
    Parent& parent = static_cast<Parent&>(*this);

    if (parent.is_enabled(level)) {
      return sink(level);
    } else {
      return *static_cast<detail::LogSink<Parent, LogLevel::disabled>*>(this);
    }
  }
};
} // namespace idle

#endif // IDLE_INTERFACE_LOG_SINK_FACADE_HPP_INCLUDED
