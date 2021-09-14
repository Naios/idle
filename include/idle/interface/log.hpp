
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

#ifndef IDLE_INTERFACE_LOG_HPP_INCLUDED
#define IDLE_INTERFACE_LOG_HPP_INCLUDED

#include <fmt/format.h>
#include <idle/core/api.hpp>
#include <idle/core/detail/macros.hpp>
#include <idle/core/platform.hpp>
#include <idle/core/util/source_location.hpp>

#define IDLE_LOG_LEVEL_TRACE 0
#define IDLE_LOG_LEVEL_DEBUG 1
#define IDLE_LOG_LEVEL_INFO 2
#define IDLE_LOG_LEVEL_WARN 3
#define IDLE_LOG_LEVEL_ERROR 4
#define IDLE_LOG_LEVEL_FATAL 5
#define IDLE_LOG_LEVEL_DISABLED 6

namespace idle {
enum class LogLevel : std::uint8_t {
  trace = IDLE_LOG_LEVEL_TRACE,
  debug = IDLE_LOG_LEVEL_DEBUG,
  info = IDLE_LOG_LEVEL_INFO,
  warn = IDLE_LOG_LEVEL_WARN,
  error = IDLE_LOG_LEVEL_ERROR,
  fatal = IDLE_LOG_LEVEL_FATAL,
  disabled = IDLE_LOG_LEVEL_DISABLED
};
}

#ifndef IDLE_HAS_LOG_LEVEL
#  ifndef NDEBUG
#    define IDLE_HAS_LOG_LEVEL IDLE_LOG_LEVEL_TRACE
#  else // NDEBUG
#    define IDLE_HAS_LOG_LEVEL IDLE_LOG_LEVEL_INFO
#  endif // NDEBUG
#endif   // IDLE_HAS_LOG_LEVEL

#define IDLE_DETAIL_DEF_MUT_LOG(LEVEL, LOGGER, ...)                            \
  do {                                                                         \
    if ((LOGGER)->is_enabled(::idle::LogLevel::LEVEL)) {                       \
      (LOGGER)->LEVEL(IDLE_CURRENT_SOURCE_LOCATION(), __VA_ARGS__);            \
    }                                                                          \
  } while (false)

#ifdef IDLE_COMPILER_CLANG
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif
#define IDLE_DETAIL_DEF_LOG(LEVEL, LOGGER, MESSAGE, ...)                       \
  IDLE_DETAIL_DEF_MUT_LOG(LEVEL, LOGGER, FMT_STRING(MESSAGE), ##__VA_ARGS__)
#ifdef IDLE_COMPILER_CLANG
#  pragma clang diagnostic pop
#endif

#if IDLE_HAS_LOG_LEVEL <= IDLE_LOG_LEVEL_TRACE
#  define IDLE_LOG_TRACE(...) IDLE_DETAIL_DEF_LOG(trace, __VA_ARGS__)
#  define IDLE_LOG_MUT_TRACE(...) IDLE_DETAIL_DEF_MUT_LOG(trace, __VA_ARGS__)
#else
#  define IDLE_LOG_TRACE(...) (void)0
#  define IDLE_LOG_MUT_TRACE(...) (void)0
#endif

#if IDLE_HAS_LOG_LEVEL <= IDLE_LOG_LEVEL_DEBUG
#  define IDLE_LOG_DEBUG(...) IDLE_DETAIL_DEF_LOG(debug, __VA_ARGS__)
#  define IDLE_LOG_MUT_DEBUG(...) IDLE_DETAIL_DEF_MUT_LOG(debug, __VA_ARGS__)
#else
#  define IDLE_LOG_DEBUG(...) (void)0
#  define IDLE_LOG_MUT_DEBUG(...) (void)0
#endif

#if IDLE_HAS_LOG_LEVEL <= IDLE_LOG_LEVEL_INFO
#  define IDLE_LOG_INFO(...) IDLE_DETAIL_DEF_LOG(info, __VA_ARGS__)
#  define IDLE_LOG_MUT_INFO(...) IDLE_DETAIL_DEF_MUT_LOG(info, __VA_ARGS__)
#else
#  define IDLE_LOG_INFO(...) (void)0
#  define IDLE_LOG_MUT_INFO(...) (void)0
#endif

#if IDLE_HAS_LOG_LEVEL <= IDLE_LOG_LEVEL_WARN
#  define IDLE_LOG_WARN(...) IDLE_DETAIL_DEF_LOG(warn, __VA_ARGS__)
#  define IDLE_LOG_MUT_WARN(...) IDLE_DETAIL_DEF_MUT_LOG(warn, __VA_ARGS__)
#else
#  define IDLE_LOG_WARN(...) (void)0
#  define IDLE_LOG_MUT_WARN(...) (void)0
#endif

#if IDLE_HAS_LOG_LEVEL <= IDLE_LOG_LEVEL_ERROR
#  define IDLE_LOG_ERROR(...) IDLE_DETAIL_DEF_LOG(error, __VA_ARGS__)
#  define IDLE_LOG_MUT_ERROR(...) IDLE_DETAIL_DEF_MUT_LOG(error, __VA_ARGS__)
#else
#  define IDLE_LOG_ERROR(...) (void)0
#  define IDLE_LOG_MUT_ERROR(...) (void)0
#endif

#if IDLE_HAS_LOG_LEVEL <= IDLE_LOG_LEVEL_FATAL
#  define IDLE_LOG_FATAL(...) IDLE_DETAIL_DEF_LOG(fatal, __VA_ARGS__)
#  define IDLE_LOG_MUT_FATAL(...) IDLE_DETAIL_DEF_MUT_LOG(fatal, __VA_ARGS__)
#else
#  define IDLE_LOG_FATAL(...) (void)0
#  define IDLE_LOG_MUT_FATAL(...) (void)0
#endif

#endif // IDLE_INTERFACE_LOG_HPP_INCLUDED
