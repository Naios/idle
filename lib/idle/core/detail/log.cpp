
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

#include <atomic>
#include <chrono>
#include <exception>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/process.hpp>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/open_file.hpp>
#include <idle/core/detail/streams.hpp>
#include <idle/core/util/string_view.hpp>
#include <idle/core/util/thread_name.hpp>

#include <idle/core/graph.hpp>

// TODO Remove this cyclic dependency
#include <idle/extra/graphviz.hpp>

#if IDLE_DETAIL_HAS_LOG_LEVEL < IDLE_DETAIL_LOG_LEVEL_DISABLED
#  ifdef IDLE_PLATFORM_WINDOWS
#    include <Windows.h>
#  else
#    include <sys/ioctl.h>
#  endif

static std::uint32_t get_terminal_width() {
#  ifdef IDLE_PLATFORM_WINDOWS
  CONSOLE_SCREEN_BUFFER_INFO info;
  HANDLE const h = GetStdHandle(STD_ERROR_HANDLE);
  if (h && GetConsoleScreenBufferInfo(h, &info) != 0) {
    return info.srWindow.Right - info.srWindow.Left + 1;
  } else {
    return 120;
  }
#  else
  struct winsize ws;
  if (ioctl(1, TIOCGWINSZ, &ws) >= 0) {
    return ws.ws_col;
  } else {
    return 80;
  }
#  endif
}

static std::uint32_t const terminal_width = get_terminal_width();

namespace idle {
namespace detail {
static fmt::terminal_color color_of_level(loglevel level) noexcept {
  switch (level) {
    case loglevel::debug:
      return fmt::terminal_color::bright_cyan;
    case loglevel::trace:
      return fmt::terminal_color::bright_yellow;
    case loglevel::error:
      return fmt::terminal_color::bright_red;
    default:
      return fmt::terminal_color::bright_white;
  }
}

static auto print_this_thread_name_or_id() {
  return printable([](std::ostream& os) {
    if (StringView thread_name = this_thread_name()) {
      os << thread_name;
    } else {
      os << std::this_thread::get_id();
    }
  });
}

void internal_log(loglevel level, SourceLocation loc, StringView msg) {
  std::string fm = format(fmt::emphasis::bold | fmt::fg(color_of_level(level)),
                          FMT_STRING("[{:<4}] {}: {}\n"),
                          print_this_thread_name_or_id(), loc, msg);

  /*std::ostringstream ss;
  indentation_stream indentation(ss, {11});
  guidline_stream guidline(indentation, {terminal_width});
  guidline << fm;
  ss << '\n';

  std::cerr << ss.str();*/
  std::cerr << fm;

  std::cerr.flush();
}

static loglevel get_default_loglevel() {
  using namespace boost::process;
  native_environment const environment = boost::this_process::environment();
  auto const var = environment.find("IDLE_INTERNAL_LOGLEVEL");

  if (var != environment.end() && !var->empty()) {
    std::string const value = var->to_string();
    if (value.size() == 1) {
      switch (value[0]) {
        case '0':
          return loglevel::trace;
        case '1':
          return loglevel::debug;
        case '2':
          return loglevel::error;
        default:
          return loglevel::disabled;
      }
    }
  }

#  ifndef IDLE_DETAIL_HAS_DEFAULT_LOG_LEVEL
  return loglevel::disabled;
#  else
  return static_cast<loglevel>(IDLE_DETAIL_HAS_DEFAULT_LOG_LEVEL);
#  endif
}

static std::atomic<loglevel> internal_loglevel(get_default_loglevel());

bool is_internal_loglevel_enabled(loglevel level) noexcept {
  return internal_loglevel.load(std::memory_order_relaxed) <= level;
}

void set_internal_loglevel(loglevel level) {
  internal_loglevel.store(level, std::memory_order_release);
}
} // namespace detail
} // namespace idle

#endif

#ifndef NDEBUG
namespace idle {
namespace detail {
static std::string internal_graph_file_impl(SourceLocation const& loc) {
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);

  return format("{:%Y_%m_%d_%H_%M_%S}_{}_L{}.gv", fmt::localtime(t),
                loc.file_name(), loc.line());
}

void internal_show_graph(SourceLocation const& loc, Context& context) {
  namespace bp = boost::process;

  auto const path = internal_graph_file_impl(loc);

  std::cerr << format(
      fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_magenta),
      FMT_STRING("[{:<7}] {}: Showing graphviz graph '{}'...\n"), "dump", loc,
      path);
  std::cerr.flush();

  {
    std::ofstream file(path, std::ios::trunc);
    graphviz(file, DependencyGraph(context, graph_view));
  }

  boost::filesystem::path src(boost::filesystem::absolute(path));

  boost::filesystem::path tar = src;
  tar.replace_extension(".pdf");

  try {
    bp::child c(bp::search_path("dot"), "-Tpdf", src.generic_string(), "-o",
                tar.generic_string());
    c.wait();

    if (c.exit_code() != 0) {
      throw std::exception();
    }
  } catch (...) {
    std::cerr << format(
        fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_magenta),
        FMT_STRING("[{:<7}] {}: Failed to convert the graph with dot ({})\n"),
        "dump", loc, src.generic_string());
    std::cerr.flush();
    return;
  }

  open_file(tar.generic_string().c_str());
}

} // namespace detail
} // namespace idle

#endif
