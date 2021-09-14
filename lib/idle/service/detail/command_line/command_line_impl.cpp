
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

#include <csignal>
#include <functional>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>
#include <boost/algorithm/string/replace.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/core/api.hpp>
#include <idle/service/detail/command_line/command_line_impl.hpp>
#include <replxx/replxx.hxx>
#include <replxx/util.h>

#ifdef IDLE_PLATFORM_WINDOWS
#  include <Windows.h>
#  undef DELETE
#endif

using namespace replxx;
using namespace std::placeholders;

namespace idle {
continuable<> CommandLineImpl::onStart() {
  return async([this] {
    repl_.emplace();

    record_ = store_->get(*this, [this](Store::BufferView buffer) {
      using namespace boost::iostreams;

      basic_array_source<char> source(buffer.data(), buffer.size());
      stream<basic_array_source<char>> stream(source);

      repl_->history_load(stream);
    });

    return launch("idle::CommandLine");
  });
}

continuable<> CommandLineImpl::onStop() {
  return requestStop().then(
      [this] {
        std::ostringstream ss;
        repl_->history_save(ss);
        store_->set(std::move(record_), ss.str());
        repl_.reset();
      },
      root().event_loop().through_dispatch());
}

static std::array<StringView, static_cast<std::size_t>(LogLevel::disabled)>
    log_names = {"trace", "debug", "info", "warn", "error", "fatal"};

static std::array<fmt::text_style, std::size_t(LogLevel::disabled)> log_styles =
    {fmt::emphasis::bold | fmt::fg(fmt::terminal_color::white),
     fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_white),
     fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_cyan),
     fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_yellow),
     fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_red),
     fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_magenta)};

template <typename... Args>
std::string format_prompt(Args&&... args) {
  return format(fmt::emphasis::bold | fmt::fg(fmt::terminal_color::bright_cyan),
                std::forward<Args>(args)...);
}

void CommandLineImpl::onLaunch() {
  scheduled_ = false;

  for (std::size_t i = 0; i < names_.size(); ++i) {
    names_[i] = format(log_styles[i], FMT_STRING("{:>5} "), log_names[i]);
  }

  default_prompt_ = format_prompt("> ");

  prompt_ = default_prompt_;

  constexpr std::size_t indentation = 6;
  indentation_ = "\n";
  indentation_.append(std::string(indentation, ' '));

  repl_->install_window_change_handler();

  repl_->set_max_history_size(128);

  repl_->set_max_hint_rows(3);

  repl_->set_completion_callback(
      std::bind(&CommandLineImpl::hook_completion, this, _1, _2));
  repl_->set_highlighter_callback(
      std::bind(&CommandLineImpl::hook_color, this, _1, _2));
  repl_->set_hint_callback(
      std::bind(&CommandLineImpl::hook_hint, this, _1, _2, _3));

  repl_->set_word_break_characters(" \t.,-%!;:=*~^'\"/?<>|[](){}");
  repl_->set_completion_count_cutoff(32);
  repl_->set_double_tab_completion(false);
  repl_->set_complete_on_empty(true);
  repl_->set_beep_on_ambiguous_completion(false);
  repl_->set_no_color(false);

  repl_->bind_key(Replxx::KEY::control('C'),
                  [handle = handleOf(*this)](
                      char32_t) -> Replxx::ACTION_RESULT {
                    if (auto me = handle.lock()) {
                      me->onCtrlCPressed();
                    }

                    return Replxx::ACTION_RESULT::RETURN;
                  });

  repl_->bind_key_internal(Replxx::KEY::BACKSPACE,
                           "delete_character_left_of_cursor");
  repl_->bind_key_internal(Replxx::KEY::DELETE,
                           "delete_character_under_cursor");
  repl_->bind_key_internal(Replxx::KEY::LEFT, "move_cursor_left");
  repl_->bind_key_internal(Replxx::KEY::RIGHT, "move_cursor_right");
  repl_->bind_key_internal(Replxx::KEY::UP, "history_previous");
  repl_->bind_key_internal(Replxx::KEY::DOWN, "history_next");
  repl_->bind_key_internal(Replxx::KEY::PAGE_UP, "history_first");
  repl_->bind_key_internal(Replxx::KEY::PAGE_DOWN, "history_last");
  repl_->bind_key_internal(Replxx::KEY::HOME,
                           "move_cursor_to_begining_of_line");
  repl_->bind_key_internal(Replxx::KEY::END, "move_cursor_to_end_of_line");
  repl_->bind_key_internal(Replxx::KEY::TAB, "complete_line");
  repl_->bind_key_internal(Replxx::KEY::control(Replxx::KEY::LEFT),
                           "move_cursor_one_word_left");
  repl_->bind_key_internal(Replxx::KEY::control(Replxx::KEY::RIGHT),
                           "move_cursor_one_word_right");
  repl_->bind_key_internal(Replxx::KEY::control(Replxx::KEY::UP),
                           "hint_previous");
  repl_->bind_key_internal(Replxx::KEY::control(Replxx::KEY::DOWN),
                           "hint_next");
  repl_->bind_key_internal(Replxx::KEY::control(Replxx::KEY::ENTER),
                           "commit_line");
  repl_->bind_key_internal(Replxx::KEY::control('R'),
                           "history_incremental_search");
  repl_->bind_key_internal(Replxx::KEY::control('W'),
                           "kill_to_begining_of_word");
  repl_->bind_key_internal(Replxx::KEY::control('U'),
                           "kill_to_begining_of_line");
  repl_->bind_key_internal(Replxx::KEY::control('K'), "kill_to_end_of_line");
  repl_->bind_key_internal(Replxx::KEY::control('Y'), "yank");
  repl_->bind_key_internal(Replxx::KEY::control('L'), "clear_screen");
  repl_->bind_key_internal(Replxx::KEY::control('D'), "send_eof");
  repl_->bind_key_internal(Replxx::KEY::control('T'), "transpose_characters");

#ifndef _WIN32
  repl_->bind_key_internal(Replxx::KEY::control('V'), "verbatim_insert");
  repl_->bind_key_internal(Replxx::KEY::control('Z'), "suspend");
#endif

  repl_->bind_key_internal(Replxx::KEY::meta(Replxx::KEY::BACKSPACE),
                           "kill_to_whitespace_on_left");
  repl_->bind_key_internal(Replxx::KEY::meta('p'),
                           "history_common_prefix_search");
  repl_->bind_key_internal(Replxx::KEY::meta('n'),
                           "history_common_prefix_search");
  repl_->bind_key_internal(Replxx::KEY::meta('d'), "kill_to_end_of_word");
  repl_->bind_key_internal(Replxx::KEY::meta('y'), "yank_cycle");
  repl_->bind_key_internal(Replxx::KEY::meta('u'), "uppercase_word");
  repl_->bind_key_internal(Replxx::KEY::meta('l'), "lowercase_word");
  repl_->bind_key_internal(Replxx::KEY::meta('c'), "capitalize_word");
  repl_->bind_key_internal('a', "insert_character");
  repl_->bind_key_internal(Replxx::KEY::INSERT, "toggle_overwrite_mode");

  // TODO Enable this after we fixed the issue that messages get eaten if
  // we rewire std::cout like this. Needs a rework of StreambufToSink.
  /*sink_out_ = StreambufToSink(sink(LogLevel::info));
  sink_err_ = StreambufToSink(sink(LogLevel::error));
  save_out_.set(sink_out_);
  save_err_.set(sink_err_);*/
}

void CommandLineImpl::onRun() {
  for (;;) {
    char const* cinput;

    do {
      try {
        std::string current;
        {
          std::lock_guard<std::mutex> lock_(prompt_mutex_);
          current = prompt_;
        }

        cinput = repl_->input(current);
      } catch (std::runtime_error const&) {
        // Currently write fails spuriously?
        // Probably the best here is to try it again
        cinput = nullptr;
      }
    } while ((cinput == nullptr) && (errno == EAGAIN) && !isStopRequested());

    if (isStopRequested()) {
      break;
    }

    if (cinput == nullptr) {
      continue;
    }

    std::string input{cinput};

    if (input.empty()) {
      continue;
    }

    repl_->history_add(input);

    { // Save the history
      std::ostringstream ss;
      repl_->history_save(ss);

      root().event_loop().post(
          wrap(*this, [data = ss.str()](auto&& cli) mutable {
            IDLE_ASSERT(cli->owner().root().is_on_event_loop());
            IDLE_ASSERT(cli->record_);

            // record_ is save to use since it is only mutated on
            // the event loop.

            cli->store_->update(cli->record_, std::move(data));
          }));
    }

    if (input == "clear") {
      repl_->clear_screen();
      continue;
    }

    bool const is_exit = (input == "exit");

    root().event_loop().post(
        weakly(handleOf(*command_processor_),
               [session = session(), input = std::move(input)](auto&& cmd) {
                 cmd->invoke(std::move(session), std::move(input));
               }));

    if (is_exit) {
      break;
    }
  }

  onTeardown();
}

void CommandLineImpl::onStopRequested() noexcept {
#ifdef IDLE_PLATFORM_WINDOWS
  // https://stackoverflow.com/questions/31657775/exit-application-while-stdin-blocking-on-windows
  DWORD dwTmp;
  INPUT_RECORD ir[2];
  ir[0].EventType = KEY_EVENT;
  ir[0].Event.KeyEvent.bKeyDown = TRUE;
  ir[0].Event.KeyEvent.dwControlKeyState = 0;
  ir[0].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
  ir[0].Event.KeyEvent.wRepeatCount = 1;
  ir[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
  ir[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN,
                                                        MAPVK_VK_TO_VSC);
  ir[1] = ir[0];
  ir[1].Event.KeyEvent.bKeyDown = FALSE;
  WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ir, 2, &dwTmp);
#endif
}

void CommandLineImpl::onTeardown() noexcept {
  try {
    save_out_.reset();
  } catch (std::exception const& e) {
    IDLE_LOG_ERROR(this,
                   "Failed to rewire std::cout to its original state ('{}')!",
                   e.what());
  }

  try {
    save_err_.reset();
  } catch (std::exception const& e) {
    IDLE_LOG_ERROR(this,
                   "Failed to rewire std::cerr to its original state ('{}')!",
                   e.what());
  }

  sink_out_.reset();
  sink_err_.reset();
}

void CommandLineImpl::log(LogLevel level, LogMessage const& message) noexcept {
  IDLE_ASSERT(level < LogLevel::disabled);
  IDLE_ASSERT(is_enabled(level));

  std::string msg(message.message.data(),
                  message.message.data() + message.message.size());

  msg.erase(std::remove_if(msg.begin(), msg.end(),
                           [](char s) {
                             return s == '\r';
                           }),
            msg.end());

  boost::algorithm::replace_all(msg, "\n", indentation_);

  fmt::memory_buffer buffer;
  auto const& name = names_[std::size_t(level)];
  format_to(buffer, FMT_STRING("{}{}\n"), name, msg);

  repl_->write(buffer.data(), buffer.size());

  /*if (!message.message.empty()) {
    char const last = message.message[message.message.size() - 1];
    if (last == '\r') {
      if (message.message.size() > 1) {
        repl_.write(message.message.data(), message.message.size() - 1);
      }
    } else {
      repl_.write(message.message.data(), message.message.size());
    }
  }*/
}

void CommandLineImpl::setPrompt(std::string prompt) {
  std::lock_guard<std::mutex> lock_(prompt_mutex_);

  repl_->set_prompt(prompt);
  prompt_ = std::move(prompt);
}

void CommandLineImpl::setDefaultPrompt() {
  setPrompt(default_prompt_);
}

continuable<> CommandLineImpl::invoke(Ref<Session> session, Arguments&& args) {
  // This is just a dummy
  (void)session;
  (void)args;
  return make_ready_continuable();
}

void CommandLineImpl::onActivityAdd(Activity const& activity) noexcept {
  IDLE_ASSERT(root().is_on_event_loop());

  (void)activity;

  debounce();
}

void CommandLineImpl::onActivityDel(Activity const& activity) noexcept {
  IDLE_ASSERT(root().is_on_event_loop());

  (void)activity;

  debounce();
}

void CommandLineImpl::onActivityUpdate(Activity const& activity) noexcept {
  IDLE_ASSERT(root().is_on_event_loop());

  (void)activity;

  debounce();
}

static std::string const progress_bar = "-\\|/-\\|/";

void CommandLineImpl::update(Clock::time_point now) noexcept {
  IDLE_ASSERT(root().is_on_event_loop());

  auto const& activities = activities_->activities();

  if (activities.empty()) {
    setDefaultPrompt();
  } else {
    auto const ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch());

    auto const step = step_ % progress_bar.size();
    step_ = step + 1U;

    char const current = progress_bar.at(step);

    setPrompt(fmt::format(FMT_STRING("{} [{}{}] {}"), current, //
                          (*activities.begin())->name(),
                          ((activities.size() > 1) ? "..." : ""),
                          default_prompt_));

    schedule(frequency_ - (ms % frequency_));
  }
}

void CommandLineImpl::schedule(Clock::duration next) noexcept {
  IDLE_ASSERT(root().is_on_event_loop());

  scheduled_ = true;

  timer_->waitFor(next).then(wrap(*this,
                                  [](auto&& me) {
                                    me->scheduled_ = false;
                                    me->update(Clock::now());
                                  }),
                             root().event_loop().through_post());
}

void CommandLineImpl::debounce() noexcept {
  IDLE_ASSERT(root().is_on_event_loop());

  auto const now = Clock::now();
  auto const diff = now - last_;

  if (diff > frequency_) {
    if (!scheduled_) {
      update(now);
    }
  } else {
    schedule(frequency_ - diff);
  }
}

Ref<Session> CommandLineImpl::session() noexcept {
  return refOf(this);
}

std::vector<std::pair<std::string, Replxx::Color>> completion_expression_color{
    // single chars
    {"\\`", Replxx::Color::BRIGHTCYAN},
    {"\\'", Replxx::Color::BRIGHTBLUE},
    {"\\\"", Replxx::Color::BRIGHTBLUE},
    {"\\-", Replxx::Color::BRIGHTBLUE},
    {"\\+", Replxx::Color::BRIGHTBLUE},
    {"\\=", Replxx::Color::BRIGHTBLUE},
    {"\\/", Replxx::Color::BRIGHTBLUE},
    {"\\*", Replxx::Color::BRIGHTBLUE},
    {"\\^", Replxx::Color::BRIGHTBLUE},
    {"\\.", Replxx::Color::BRIGHTMAGENTA},
    {"\\(", Replxx::Color::BRIGHTMAGENTA},
    {"\\)", Replxx::Color::BRIGHTMAGENTA},
    {"\\[", Replxx::Color::BRIGHTMAGENTA},
    {"\\]", Replxx::Color::BRIGHTMAGENTA},
    {"\\{", Replxx::Color::BRIGHTMAGENTA},
    {"\\}", Replxx::Color::BRIGHTMAGENTA},

    // numbers
    {"[\\-|+]{0,1}[0-9]+", Replxx::Color::YELLOW},          // integers
    {"[\\-|+]{0,1}[0-9]*\\.[0-9]+", Replxx::Color::YELLOW}, // decimals
    {"[\\-|+]{0,1}[0-9]+e[\\-|+]{0,1}[0-9]+",
     Replxx::Color::YELLOW}, // scientific notation

    // strings
    {"\".*?\"", Replxx::Color::BRIGHTGREEN}, // double quotes
    {"\'.*?\'", Replxx::Color::BRIGHTGREEN}, // single quotes
};

static std::string::size_type context_offset(std::string const& str) noexcept {
  auto const pos = str.find_last_of(' ');
  if (pos != std::string::npos) {
    return pos + 1;
  } else {
    return 0;
  }
}

static std::string&&
cut_to_context_length(std::string&& str,
                      std::string::size_type offset) noexcept {
  IDLE_ASSERT(str.size() > offset);

  str.erase(0, offset);
  return std::move(str);
}

// Tab
Replxx::completions_t
CommandLineImpl::hook_completion(std::string const& context, int& contextLen) {

  std::string::size_type const offset = context_offset(context);

  Replxx::completions_t completions;
  auto completed = command_processor_->complete(*this, context);
  completions.reserve(completed.size());

  for (std::string& completion : completed) {
    std::string str = std::move(completion);
    if (str.size() <= contextLen) {
      continue;
    }

    completions.emplace_back(cut_to_context_length(std::move(str), offset),
                             Replxx::Color::DEFAULT);
  }

  return completions;
}

// On each keystroke
Replxx::hints_t CommandLineImpl::hook_hint(std::string const& context,
                                           int& contextLen,
                                           Replxx::Color& color) {

  // Never complete when no user input is given
  if (context.empty()) {
    return {};
  }

  std::string::size_type const offset = context_offset(context);

  Replxx::hints_t hints;
  auto completed = command_processor_->complete(*this, context);
  hints.reserve(completed.size());

  for (std::string& completion : completed) {
    std::string str = std::move(completion);
    if (str.size() <= contextLen) {
      continue;
    }

    hints.emplace_back(cut_to_context_length(std::move(str), offset));
  }

  if (hints.size() == 1) {
    color = Replxx::Color::GREEN;
  }

  return hints;
}

static void highlight(std::string const& context, Replxx::colors_t& colors,
                      std::pair<std::string, Replxx::Color> matcher) {

  size_t pos{0};
  std::string str = context;
  std::smatch match;

  while (std::regex_search(str, match, std::regex(matcher.first))) {
    std::string c{match[0]};
    std::string prefix(match.prefix().str());
    pos += utf8str_codepoint_len(prefix.c_str(),
                                 static_cast<int>(prefix.length()));
    int len(utf8str_codepoint_len(c.c_str(), static_cast<int>(c.length())));

    for (int i = 0; i < len; ++i) {
      colors.at(pos + i) = matcher.second;
    }

    pos += len;
    str = match.suffix();
  }
}

void CommandLineImpl::hook_color(std::string const& context,
                                 Replxx::colors_t& colors) {

  auto const completion = command_processor_->complete(*this, context);

  for (auto& e : completion) {
    highlight(context, colors,
              std::make_pair(std::move(e), Replxx::Color::BRIGHTCYAN));
  }

  for (auto const& e : completion_expression_color) {
    highlight(context, colors, e);
  }
}

void CommandLineImpl::onCtrlCPressed() {
  if (isRunning() && !isStopRequested()) {
    repl_->print("exit");

    std::raise(SIGINT);
  }
}
} // namespace idle
