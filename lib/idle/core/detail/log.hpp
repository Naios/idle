
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

#ifndef IDLE_CORE_DETAIL_LOG_HPP_INCLUDED
#define IDLE_CORE_DETAIL_LOG_HPP_INCLUDED

#include <idle/core/dep/format.hpp>
#include <idle/core/detail/macros.hpp>
#include <idle/core/platform.hpp>
#include <idle/core/util/source_location.hpp>
#include <idle/core/util/string_view.hpp>

#define IDLE_DETAIL_LOG_LEVEL_TRACE 0
#define IDLE_DETAIL_LOG_LEVEL_DEBUG 1
#define IDLE_DETAIL_LOG_LEVEL_ERROR 2
#define IDLE_DETAIL_LOG_LEVEL_DISABLED 3

#ifndef IDLE_DETAIL_HAS_LOG_LEVEL
#  ifndef NDEBUG
#    define IDLE_DETAIL_HAS_LOG_LEVEL IDLE_DETAIL_LOG_LEVEL_TRACE
#  else // NDEBUG
#    define IDLE_DETAIL_HAS_LOG_LEVEL IDLE_DETAIL_LOG_LEVEL_DISABLED
#  endif // NDEBUG
#endif

#if IDLE_DETAIL_HAS_LOG_LEVEL < IDLE_DETAIL_LOG_LEVEL_DISABLED
namespace idle {
namespace detail {
enum class loglevel : std::uint8_t {
  trace = IDLE_DETAIL_LOG_LEVEL_TRACE,
  debug = IDLE_DETAIL_LOG_LEVEL_DEBUG,
  error = IDLE_DETAIL_LOG_LEVEL_ERROR,
  disabled = IDLE_DETAIL_LOG_LEVEL_DISABLED
};

bool is_internal_loglevel_enabled(loglevel level) noexcept;
void set_internal_loglevel(loglevel level);

void internal_log(loglevel level, SourceLocation loc, StringView msg);
} // namespace detail
} // namespace idle

#  define IDLE_DETAIL_LOG_IMPL(LEVEL, ...)                                     \
    do {                                                                       \
      if (!::idle::detail::is_internal_loglevel_enabled(LEVEL)) {              \
        break;                                                                 \
      }                                                                        \
      ::fmt::memory_buffer out__;                                              \
      ::fmt::format_to(out__, __VA_ARGS__);                                    \
      ::idle::detail::internal_log(LEVEL, IDLE_CURRENT_SOURCE_LOCATION(),      \
                                   ::idle::StringView(out__.data(),            \
                                                      out__.size()));          \
    } while (false)

#  ifdef IDLE_COMPILER_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#  endif
#  define IDLE_DETAIL_LOG(LEVEL, MESSAGE, ...)                                 \
    IDLE_DETAIL_LOG_IMPL(LEVEL, FMT_STRING(MESSAGE), ##__VA_ARGS__)
#  ifdef IDLE_COMPILER_CLANG
#    pragma clang diagnostic pop
#  endif

#else
#  define IDLE_DETAIL_LOG(...)
#endif

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_TRACE
#  define IDLE_DETAIL_LOG_TRACE(...)                                           \
    IDLE_DETAIL_LOG(::idle::detail::loglevel::trace, __VA_ARGS__)
#else
#  define IDLE_DETAIL_LOG_TRACE(...)
#endif // IDLE_DETAIL_HAS_CORE_TRACE

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_DEBUG
#  define IDLE_DETAIL_LOG_DEBUG(...)                                           \
    IDLE_DETAIL_LOG(::idle::detail::loglevel::debug, __VA_ARGS__)
#else
#  define IDLE_DETAIL_LOG_DEBUG(...)
#endif // IDLE_DETAIL_HAS_LOG_CORE_DEBUG

#if IDLE_DETAIL_HAS_LOG_LEVEL <= IDLE_DETAIL_LOG_LEVEL_ERROR
#  define IDLE_DETAIL_LOG_ERROR(...)                                           \
    IDLE_DETAIL_LOG(::idle::detail::loglevel::error, __VA_ARGS__)
#else
#  define IDLE_DETAIL_LOG_ERROR(...)
#endif // IDLE_DETAIL_HAS_LOG_CORE_DEBUG

#ifndef NDEBUG
namespace idle {
class Context;

namespace detail {
void internal_show_graph(SourceLocation const& loc, Context& context);
} // namespace detail
} // namespace idle

#  define IDLE_DETAIL_SHOW_GRAPH(IDLE_CONTEXT)                                 \
    ::idle::detail::internal_show_graph(IDLE_CURRENT_SOURCE_LOCATION(),        \
                                        IDLE_CONTEXT)
#else
#  define IDLE_DETAIL_SHOW_GRAPH(IDLE_CONTEXT) (void)0
#endif

#endif // IDLE_CORE_DETAIL_LOG_HPP_INCLUDED
